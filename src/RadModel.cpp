#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "RadModel.hpp"
#include "NuModel.hpp"
#include "io.hpp"
#include "initialize.hpp"
#include "process.hpp"
#include "constants.hpp"
#include "lambda.hpp"

using namespace std;
namespace py = pybind11;

namespace myLib
{
    RadModel::RadModel(const radParams &params)
      : params(params),
        // Allocate vectors
        quadMu(params.nQuad, zero),
        quadW(params.nQuad, zero),
        spectrum(params.nWave, vector<double>(2, zero)),
        tau(params.nZones, zero),
        T(params.nZones, zero)
    {
        initialize(*this);
    }

    RadModel::RadModel(const py::dict &dictParams)
      : params(dictToParams(dictParams)),
        // Allocate vectors
        quadMu(params.nQuad, zero),
        quadW(params.nQuad, zero),
        spectrum(params.nWave, vector<double>(2, zero)),
        tau(params.nZones, zero),
        T(params.nZones, zero)
    {
        initialize(*this);
    }

    vector<vector<double>> RadModel::genSpectrum()
    {
        cout << "Generating spectrum..." << endl;
        auto t0 = chrono::high_resolution_clock::now();

        // Calculate Lambda operator
        vector<double> lambdaA(params.nZones);
        vector<double> lambdaB(params.nZones);
        vector<double> lambdaC(params.nZones);

        calcLambda(*this, lambdaA, lambdaB, lambdaC);

        auto t1 = chrono::high_resolution_clock::now();
        chrono::duration<double> sec = t1 - t0;
        cout << "Calculated Lambda matrix in "<< fixed << setprecision(3)
             << sec.count() << " sec" << endl;

        // Calc flux at each wavepoint - parallelize this in the future?
        for (int i=0; i < params.nWave; i++)
        {
            NuModel nuModel = NuModel(spectrum[i][0], *this);
            spectrum[i][1] = nuModel.getFlux();
        }

        rescaleFlux();

        auto t2 = chrono::high_resolution_clock::now();
        chrono::duration<double> sec = t2 - t1;
        cout << "Generated spectrum in "<< fixed << setprecision(3)
             << sec.count() << " sec" << endl;

        return spectrum;
    }

    void RadModel::rescaleFlux()
    {
        // Rescale with constants from Planck function
        for (int i=0; i < params.nWave; i++)
        {
            // spectrum[i][1] *= 2.0 * hc * c * 1e8;
        }
    }

    // Properties
    std::vector<double> RadModel::getTau() { return tau; }
    std::vector<double> RadModel::getT() { return T; }
}

PYBIND11_MODULE(Rad1D, module_handle) {
    module_handle.doc() = "Radiative transfer model.";
    py::class_<myLib::RadModel>(module_handle, "RadModel")
        .def(py::init<const py::dict &>()
        )
        .def("gen_spectrum", [](myLib::RadModel &self) {
            py::array out = py::cast(self.genSpectrum());
            return out;
        })
        // Properties
        .def_property_readonly("tau", [](myLib::RadModel &self) {
            py::array out = py::cast(self.getTau()); return out;
        })
        .def_property_readonly("T", [](myLib::RadModel &self) {
            py::array out = py::cast(self.getT()); return out;
        })
    ;
}