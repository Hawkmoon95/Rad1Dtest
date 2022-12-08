import numpy as np
import matplotlib.pyplot as plt
from Rad1D import RadModel

from .conftest import test_plot_dir


params = {
    'data_dir'     : '/home/masamune/.bin/Rad1D/data',
    'tau_min'      : 1e-8,
    'tau_max'      : 1e6,
    'T_eff'        : 6000.,
    'max_iter'     : 1000,
    'eps_converge' : 1e-8,
    'Ng_accelerated' : False,
    'verbose'      : False,
}

eps_to_check = (1e0, 1e-1, 1e-4)

hc = 12398.4198
c = 29979.2458
k = 8.617333262e-5
k_hc = k / hc


def plot_eps(ax, model, results, eps):
    if eps == 1.:
        ax.plot(model.tau, results[0], c='r', ls='-', lw=1., alpha=0.4)
        for i in range(1, params['max_iter']):
            ax.plot(model.tau, results[i], c='k', ls='-', lw=1., alpha=0.4)
        ax.set_title(r'$\epsilon = 1.0$')
        return

    J = (results - eps) / (1 - eps)
    diffs = np.abs(J[1:] - J[:-1]) / J[:-1]
    max_diffs = diffs.max(axis=1)

    eps_converge = params['eps_converge']
    max_iter = params['max_iter']
    for i in range(max_iter):
        if max_diffs[i] > eps_converge:
            continue
        ind_needed = i
        break
    else:
        ind_needed = max_iter

    results = results[:ind_needed]

    ax.plot(model.tau, results[0], c='r', ls='-', lw=1., alpha=0.4)
    for i in range(1, ind_needed):
        ax.plot(model.tau, results[i], c='k', ls='-', lw=1., alpha=0.4)

    ax.axhline(np.sqrt(eps), ls='--', color='k')

    ax.text(0.65, 0.1, f'{ind_needed + 1} iterations', transform=ax.transAxes)
    ax.set_title(r'$\epsilon = $' + f'{eps}')


def planck(lam, T):
    bbScale = 2. * hc * c * 1.e8
    bb = bbScale / (pow(lam, 5.) * (np.exp(1. / (k_hc * T * lam)) - 1.))
    return bb


def analytic_J(eps, tau, a=0., b=0.):
    c = np.sqrt(3. * eps)
    J = a + (b - c * a) * np.exp(-c * tau) / (np.sqrt(3.) + c)
    return J


def plot_J(ax, model, results, eps):
    if eps == 1.:
        return

    # Find analytic J
    a = planck(5000., params['T_eff'])
    J_analytic = analytic_J(eps, model.tau, a=a)

    # Find calculated J
    J_calc = (results - eps * a) / (1. - eps)
    diffs = np.abs(J_calc[1:] - J_calc[:-1]) / J_calc[:-1]
    max_diffs = diffs.max(axis=1)

    eps_converge = params['eps_converge']
    max_iter = params['max_iter']
    for i in range(max_iter):
        if max_diffs[i] > eps_converge:
            continue
        ind_needed = i
        break
    else:
        ind_needed = max_iter

    J_calc = J_calc[ind_needed]

    # Plot here
    pct_diff = 100. * (J_analytic - J_calc) / J_calc

    ax.plot(model.tau, pct_diff, '-', label=r'\epsilon = ' + f'{eps}')


def test_epsilon():
    fig1, ax1 = plt.subplots(1, 3, figsize=(12, 4), sharey=True)
    fig2, ax2 = plt.subplots()

    for i, eps in enumerate(eps_to_check):
        params['eps'] = eps
        model = RadModel(params)
        results = model.convergence_test()

        plot_eps(ax1[i], model, results, eps)
        plot_J(ax2, model, results, eps)

    # Fig 1 properties
    [_ax.set_xlim(params['tau_min'], params['tau_max']) for _ax in ax1]
    bottom_convergence = np.sqrt(np.array(eps_to_check).min())
    ax1[0].set_ylim(0.8 * bottom_convergence, 1.2)

    [_ax.set_xscale('log') for _ax in ax1]
    ax1[0].set_yscale('log')

    [_ax.set_xlabel(r'$\tau$') for _ax in ax1]
    ax1[0].set_ylabel('S / B')

    plt.tight_layout()
    fn = f'{test_plot_dir}/eps_convergence.pdf'
    fig1.savefig(fn)                              	

    # Fig 2 properties
    ax2.set_xscale('log')

    fn = f'{test_plot_dir}/J_comparison.pdf'
    fig2.savefig(fn)
