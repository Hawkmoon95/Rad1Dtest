#include <iostream>
#include <sstream>

#include "gaussianQuadrature.hpp"
#include "RadModel.hpp"

using namespace std;

namespace myLib
{
    void getWeights(RadModel &radModel)
    {
        const int &nQuad = radModel.params.nQuad;
        cout << " case: " << radModel.params.nQuad <<"\n";
        switch(radModel.params.nQuad)
        {
            case 2:
            {
                const double inv_sqrt_3 = 1.0 / sqrt(3.0);
                radModel.quadMu = { inv_sqrt_3, -inv_sqrt_3 };
                radModel.quadW = { 1.0, 1.0 };
                break;
            }
            case 4:
            {
                radModel.quadMu = { 0.8611363115940526, 0.3399810435848563, -0.3399810435848563, -0.8611363115940526 };
                radModel.quadW = { 0.3478548451374538, 0.6521451548625461, 0.6521451548625461, 0.3478548451374538 };
                break;
            }
            case 6:
            {
                radModel.quadMu = { 0.9324695142031521, 0.6612093864662645, 0.2386191860831969, -0.2386191860831969,
                                    -0.6612093864662645, -0.9324695142031521 };
                radModel.quadW = { 0.1713244923791704, 0.3607615730481386, 0.4679139345726910, 0.4679139345726910,
                                   0.3607615730481386, 0.1713244923791704 };
                break;
            }
            case 8:
            {
                radModel.quadMu = { 0.9602898564975363, 0.7966664774136267, 0.5255324099163290, 0.1834346424956498,
                                    -0.1834346424956498, -0.5255324099163290, -0.7966664774136267, -0.9602898564975363 };
                radModel.quadW = { 0.1012285362903763, 0.2223810344533745, 0.3137066458778873, 0.3626837833783620,
                                   0.3626837833783620, 0.3137066458778873, 0.2223810344533745, 0.1012285362903763 };
                break;
            }
            case 32:
            {   // point datat taken from https://pomax.github.io/bezierinfo/legendre-gauss.html
                radModel.quadMu = {0.997263862,0.985611512,0.964762256,0.934906076,0.896321156,0.849367614,0.794483796,
                                    0.732182119,0.663044267,0.587715757,0.506899909,0.421351276,0.331868602,0.239287362,
                                    0.144471962,0.048307666,-0.048307666,-0.144471962,-0.239287362,-0.331868602,-0.421351276,
                                    -0.506899909,-0.587715757,-0.663044267,-0.732182119,-0.794483796,-0.849367614,-0.896321156,
                                    -0.934906076,-0.964762256,-0.985611512,-0.997263862};
                radModel.quadW = {0.00701861,0.016274395,0.025392065,0.034273863,0.042835898,0.050998059,0.058684093,0.065822223,
                                    0.072345794,0.078193896,0.083311924,0.087652093,0.091173879,0.093844399,0.09563872,0.096540089,
                                    0.096540089,0.09563872,0.093844399,0.091173879,0.087652093,0.083311924,0.078193896,0.072345794,
                                    0.065822223,0.058684093,0.050998059,0.042835898,0.034273863,0.025392065,0.016274395,0.00701861};
                cout << "we're in 32 and have gotten W,Mu\n";
                break;
            }
            default:
                ostringstream output;
                output << "nQuad = " << nQuad << " is not valid";
                radModel.log(output);
        }
    }
}