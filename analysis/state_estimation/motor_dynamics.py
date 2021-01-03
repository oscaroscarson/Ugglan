import argparse
from pathlib import Path
from typing import List, Tuple

import matplotlib.pyplot as plt
import numpy as np

from read_data_log import read_data_log, Signals

MIN_STEP_SIZE_RADPS = 50
MIN_STEP_LEN_S = 3

YR_LIM_UPPER_MLSE = 0.8


def _find_step_indices(t_s: np.array, x: np.array,
                       min_step_size: float,
                       min_step_length_s: float) -> List[Tuple[int, int]]:
    indices = []
    start = None

    for i in range(1, len(x)):

        if abs(x[i] - x[i - 1]) > 0:
            possible_end = i - 1
        elif i == (len(x) - 1):
            possible_end = i
        else:
            possible_end = None

        if start is not None and possible_end is not None:
            if t_s[possible_end] - t_s[start] >= min_step_length_s:
                indices.append((start, possible_end))
            start = None

        if abs(x[i] - x[i - 1]) >= min_step_size:
            start = i - 1

    return indices


def _compute_tau_using_mlse(u_0, u_1, t, y):
    tau = None

    yr = (np.array(y) - u_0) / (u_1 - u_0)
    yr_valid = np.argwhere(yr < YR_LIM_UPPER_MLSE)

    if yr_valid.size is not 0:
        end = yr_valid[-1][0]

        a = np.array(t[:end]) - t[0]
        b = np.array(np.log((u_1 - u_0) / (u_1 - y[:end])))

        valid_idx = np.isfinite(a) & np.isfinite(b)

        if len(valid_idx) > 0:
            res = np.linalg.lstsq(np.array([a[valid_idx]]).T, np.array([b[valid_idx]]).T, rcond=None)
            tau = 1 / float(res[0])

    return tau


def _extract_signals(data, motor_i):
    ang_rate = getattr(data.Esc, 'AngularRate' + str(motor_i))
    motor_cmd = getattr(data.Esc, 'MotorCmd' + str(motor_i))

    ang_rate = np.interp(motor_cmd.t_s, ang_rate.t_s, ang_rate.val)

    return motor_cmd.t_s, motor_cmd.val, ang_rate


def _est_y(t, u_0, u_1, tau):
    dt = np.array(t) - t[0]
    y = u_1 + (u_0 - u_1) * np.exp(-dt / tau)

    return y


def _plot_motor_dynamics(data: Signals, motor_i: int):
    """
    Plots the motor dynamics i.e., step response(s)
    for a given motor_i.

    Estimates the time constant of the systems
    Y(s) = G(s) * U(s), where

                G(s) = 1 / (tau * s + 1)

    for each step response by using MLSE.
    """
    u_0 = None
    fig, ax = plt.subplots()
    t, u_cmd, y = _extract_signals(data, motor_i)

    for start, end in _find_step_indices(t, u_cmd,
                                         min_step_size=MIN_STEP_SIZE_RADPS,
                                         min_step_length_s=MIN_STEP_LEN_S):
        # Estimate u from y (instead of using u
        # directly due to potential discrepancies).
        if u_0 is None:
            u_0 = np.median(y[:(start + 1)])
            u_1 = np.median(y[(start + 1):end])
        else:
            u_0 = u_1
            u_1 = np.median(y[(start + 1):end])

        tau = _compute_tau_using_mlse(u_0, u_1, t[start:end], y[start:end])

        t_step = t[start:end]
        u_step = [u_0] + (u_1 * np.ones((end - start - 1))).tolist()
        y_step = y[start:end]
        y_est_step = _est_y(t_step, u_0, u_1, tau)

        ax.plot(t_step, y_step, color='C0')
        ax.plot(t_step, y_est_step, color='C1')
        ax.plot(t_step, u_step, color='C3')

        ax.text(t_step[len(t_step) // 2], u_step[-1] + 25,
                r'$\tau = {:.2f}$'.format(tau), horizontalalignment='center')

    ax.set_title(r'Step response motor: {}'.format(motor_i))
    ax.set_xlabel('Time [s]')
    ax.set_ylabel('Angular-rate [rad/s]')
    ax.legend((r'$\omega_M$', r'$\widetilde{\omega}_M$', r'$\omega_{M_r}$'))
    ax.grid()


def main():
    parser = argparse.ArgumentParser(description='Plot motor step response and estimate its time constant.')
    parser.add_argument('path', type=Path, help='Path to data log file')
    parser.add_argument('--motor_i', type=int, help='Select which motor (0,1,2,3)', default=0)
    args = parser.parse_args()

    data = read_data_log(args.path)
    _plot_motor_dynamics(data, args.motor_i)
    plt.show()


if __name__ == "__main__":
    main()