from Rad1D import RadModel
import numpy as np


def test_S0_B0():
    min_eps = -4.
    eps_to_check = np.logspace(min_eps, 0., int(abs(min_eps)) + 1)

    print()

    for eps in eps_to_check:
        params = {
            'data_dir'    : '/home/masamune/.bin/Rad1D/data',
            'max_iter'    : max(100, int(1. / eps)),
            'eps'         : eps,
            'verbose'     : False,
        }

        model = RadModel(params)
        results = model.convergence_test(5000.)

        S_B = results[-1][0]
        justify = int(abs(min_eps)) + 2
        print(f'  eps = {eps:<{justify}} : [S(0)/B(0)] / sqrt(eps) = {S_B / np.sqrt(eps):.3f}')
