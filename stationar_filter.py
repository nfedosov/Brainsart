# -*- coding: utf-8 -*-
"""
Created on Thu Jan 19 02:12:48 2023

@author: Fedosov
"""



import numpy as np
import scipy.signal as sn
from scipy.optimize import nnls
from tqdm import trange


def cfir_grid_search(signal, fs, band = [8.0,12.0], order_range = [10,300]):
    
    
    gt = band_hilbert(signal, fs, )
    
    metrics = np.zeros(len(order_range))
    for i,order in enumerate(order_range):
        cfir = CFIRBandEnvelopeDetector(band = band, fs = fs, delay_ms=0, n_taps=order, n_fft=fs, reg_coeff=0)
        filtered = cfir.apply(signal)
        
        envelope = np.abs(filtered)
        
        metrics = np.sum(gt*envelope)/(np.linalg.norm(envelope)*np.linalg.norm(gt))
        
    return metrics
        

    
    
def fit_envelope():
    pass



class CFIRBandEnvelopeDetector:
    def ideal_H(self,n_fft, fs, band, delay=0):
            """
            Estimate ideal delayed analytic filter freq. response
            :param n_fft: length of freq. grid
            :param fs: sampling frequency
            :param band: freq. range to apply band-pass filtering
            :param delay: delay in samples
            :return: freq. response
            """
            w = np.arange(n_fft)
            H = 2*np.exp(-2j*np.pi*w/n_fft*delay)
            H[(w/n_fft*fs<band[0]) | (w/n_fft*fs>band[1])] = 0
            return H
    
    
    def __init__(self, band, fs, delay_ms=0, n_taps=500, n_fft=2000, reg_coeff=0):
        """
        Complex-valued FIR envelope detector based on analytic signal reconstruction
        :param band: freq. range to apply band-pass filtering, Hz
        :param fs: sampling frequency, Hz
        :param delay_ms: delay of ideal filter in ms
        :param n_taps: length of FIR
        :param n_fft: length of freq. grid to estimate ideal freq. response
        :param reg_coeff: least squares L2 regularisation coefficient
        """
        H = self.ideal_H(n_fft, fs, band, int(delay_ms*fs/1000))
        F = np.array([np.exp(-2j * np.pi / n_fft * k * np.arange(n_taps)) for k in np.arange(n_fft)])
        self.b = cLS(F, H, reg_coeff)
        self.a = np.array([1.])
        #self.zi = np.zeros(len(self.b)-1)
        #self.smoother = smoother

    def apply(self, chunk: np.ndarray):
        y= sn.lfilter(self.b, self.a, chunk,axis = -1)
        #y = self.smoother.apply(np.abs(y))
        return y
    
    
    
    
    
    
def cLS(X, Y, lam=0):
    """
    Complex valued Least Squares with L2 regularisation
    """
    reg = lam*np.eye(X.shape[1])
    b = np.linalg.inv(X.T@X.conj()+reg)@X.T.conj()@Y
    return b



def band_hilbert(x, fs, band, N=None, axis=-1):
    """
    Non-causal bandpass Hilbert transform to reconstruct analytic narrow-band signal
    :param x: input signal
    :param fs: sampling frequency
    :param band: band of interest
    :param N: fft n parameter
    :param axis: fft axis parameter
    :return: analytic narrow-band signal
    """
    #x = np.asarray(x)
    Xf = np.fft.fft(x, N, axis=axis)
    w = np.fft.fftfreq(N or x.shape[0], d=1. / fs)
    Xf[(w < band[0]) | (w > band[1])] = 0
    x = np.fft.ifft(Xf, axis=axis)[:x.shape[0]]
    return 2*x



#def temporal_fitting_envelope(x, ideal_env):
    
    
    # MSE FITTING of envelope 
    
   
    
def corr_latency(self, filtered, gt, gt_method):
    pass
    
    
    
def generate_matsuda_params(fs: float, fc: float, A: float):
    arg = 2.0*np.pi*fc/fs
    Phi = np.array([[np.cos(arg),-np.sin(arg)],[np.sin(arg),np.cos(arg)]])
    H = np.array([1.0,0.0]).reshape((1,2))
    
    return Phi, H    
    

def fit_kf_grid()


def fit_kf_parameters(meas, kf, cf, A, fs, n_iter: int = 2, tol: float = 1e-3):


    AMP_EPS = 1e-4
    
    prev_freq = cf
    model_error = np.inf
    for _ in (pb := trange(n_iter, desc="Fitting KF parameters")):
        amp, freq, q_s, r_s, x_0, P_0 = em_step(meas, kf, pb)
        #if freq > 13.0:
        #    freq = 13.0
        #if freq <8.0:
        #    freq = 8.0
        if freq != 10.0:
            freq = 10.0
            
        #if amp < 0.95:
        #    amp = 0.95
        if amp != 0.99:
            amp = 0.99
            
        amp = min(amp, 1 - AMP_EPS)
    

        #mp = MatsudaParams(amp, freq, fs)
        Phi, H = generate_matsuda_params(fs, freq, A)
        
        kf = whiteKF(Phi = Phi, H = H, Q = q_s, R = r_s)
        
        
        #KF = whiteKF(mp, H = H, q_s, KF.psi, r_s)
        kf.x = x_0
        kf.P = P_0
        model_error = abs(freq - prev_freq)
        prev_freq = freq
        #if model_error < tol:
        #    break
    else:
        print(f"Didn't converge after {n_iter} iterations; {model_error=}")
    return kf


def em_step(meas, KF, pb):
    n = len(meas)
    Phi, A, Q, R = KF.Phi, KF.H, KF.Q, KF.R
    assert n, "Measurements must be nonempty"

    y = normalize_measurement_dimensions(meas)
    x, P = apply_kf(KF, y)
    nll = compute_kf_nll(y, x, P, KF)
    x_n, P_n, J = apply_kalman_interval_smoother(KF, x, P)
    P_nt = estimate_adjacent_states_covariances(Phi, Q, A, R, P, J)

    S = compute_aux_em_matrices(x_n, P_n, P_nt)
    freq, Amp, q_s, r_s = params_update(S, Phi, n)
    pb.set_description(
        f"Fitting KF parameters: nll={nll:.2f},"
        f"f={freq*1000/2/np.pi:.2f}, A={Amp:.4f}, {q_s:.2f}, {r_s:.2f}"
    )
    x_0_new = x_n[0]
    P_0_new = P_n[0]

    return Amp, freq, q_s, r_s, x_0_new, P_0_new


def normalize_measurement_dimensions(meas):
    # prepend nan for to simplify indexing; 0 index is for x and P prior to the measurements
    n = len(meas)
    y = [np.array([[np.nan]])] * (n + 1)
    for t in range(1, n + 1):
        y[t] = meas[t - 1, np.newaxis, np.newaxis]
    return y


def apply_kf(KF, y):
    n = len(y) - 1
    x = [None] * (n + 1)  # pyright: ignore  # x^t_t
    P = [None] * (n + 1)  # pyright: ignore  # P^t_t
    x[0], P[0] = KF.x, KF.P
    for t in range(1, n + 1):
        x[t], P[t] = KF.step(y[t])
    return x, P


def apply_kalman_interval_smoother(
    KF, x, P
) :
    n = len(x) - 1
    x_n = [None] * (n + 1)  # pyright: ignore  # x^n_t
    P_n= [None] * (n + 1)  # pyright: ignore  # P^n_t
    x_n[n], P_n[n] = x[n], P[n]
    J = [None] * (n + 1)  # pyright: ignore
    for t in range(n, 0, -1):
        x_n[t - 1], P_n[t - 1], J[t - 1] = smoother_step(KF, x[t - 1], P[t - 1], x_n[t], P_n[t])

    return x_n, P_n, J


def smoother_step(KF, x, P, x_n, P_n) :
    """
    Make one Kalman Smoother step
    Parameters
    ----------
    x : Vec
        State estimate after KF update step after the forward pass, i.e.
        x^{t-1}_{t-1} in eq (6.47) in [1]
    P : Cov
        State covariance after KF update step after the forward pass, i.e.
        P^{t-1}_{t-1} in eq. (6.48) in [1]
    x_n : Vec
        Smoothed state estimate for the time instaint following the one being
        currently processed, i.e. x^{n}_{t} in eq. (6.47) in [1]
    P_n : Cov
        Smoothed state covariance for the time instant following the one being
        currently processed, i.e. P^{n}_{t} in eq. (6.47) in [1]
    Returns
    -------
    x_n : Vec
        Smoothed state estimate for one timestep back, i.e. x^{n}_{t-1} in eq.
        (6.47) in [1]
    P_n : Cov
        Smoothed state covariance for one timestep back, i.e. P^{n}_{t-1} in eq. (6.48) in [1]
    J : Mat
        J_{t-1} in eq. (6.49) in [1]
    Notes
    -----
    Code here follows slightly different notation than in em_step(); e.g. here
    x_n is a state vector for a single time instant compared to an array of
    state vectors in em_step().
    References
    ----------
    [1] .. Shumway, Robert H., and David S. Stoffer. 2011. Time Series Analysis
    and Its Applications. Springer Texts in Statistics. New York, NY: Springer
    New York. https://doi.org/10.1007/978-1-4419-7865-3.
    """
    x_, P_ = KF.predict()#KF.predict(x, P)

    J = np.linalg.solve(P_, KF.Phi @ P).T  # P * Phi^T * P_^{-1}; solve is better than inv

    x_n = x + J @ (x_n - x_)
    P_n = P + J @ (P_n - P_) @ J.T

    return x_n, P_n, J


def estimate_adjacent_states_covariances(
    Phi, Q, A, R, P, J
):
    # estimate P^n_{t-1,t-2}
    n = len(P) - 1
    P_ = Phi @ P[n - 1] @ Phi.T + Q  # P^{n-1}_n
    K = np.linalg.solve(A @ P_ @ A.T + R, A @ P[n]).T  # K_n, eq. (6.23) in [1]
    P_nt = [None] * (n + 1)  # pyright: ignore  # P^n_{t-1, t-2}
    P_nt[n - 1] = (np.eye(K.shape[0]) - K @ A) @ Phi @ P[n - 1]  # P^n_{n, n-1}, eq.(6.55) in [1]

    for t in range(n, 1, -1):
        P_nt[t - 2] = (
            P[t - 1] @ J[t - 2].T + J[t - 1] @ (P_nt[t - 1] - Phi @ P[t - 1]) @ J[t - 2].T
        )
    return P_nt


def compute_aux_em_matrices(x_n, P_n, P_nt):
    n = len(x_n) - 1
    S = {
        "11": np.zeros_like(P_n[0], dtype=np.longdouble),
        "10": np.zeros_like(P_nt[0], dtype=np.longdouble),
        "00": np.zeros_like(P_n[0], dtype=np.longdouble),
    }
    for t in range(1, n + 1):
        S["11"] += x_n[t] @ x_n[t].T + P_n[t]
        S["10"] += x_n[t] @ x_n[t - 1].T + P_nt[t - 1]
        S["00"] += x_n[t - 1] @ x_n[t - 1].T + P_n[t - 1]
    return S


def params_update(S, Phi, n) :
    A = S["00"][0, 0] + S["00"][1, 1]
    B = S["10"][0, 0] + S["10"][1, 1]
    C = S["10"][1, 0] - S["10"][0, 1]
    D = S["11"][0, 0] + S["11"][1, 1]
    f = max(C / B, 0)
    Amp = np.sqrt(B**2 + C**2) / A
    q_s = np.sqrt(max(0.5 * (D - Amp**2 * A) / n, 1e-6))
    r_s = np.sqrt(
        (S["11"][0, 0])# - 2 * S["10"][2, :] @ Phi.T[:, 2] + (Phi[2, :] @ S["00"] @ Phi.T[:, 2])) / n
    )
    return float(f), float(Amp), float(q_s), float(r_s)


def compute_kf_nll(y, x, P, KF) -> float:
    n = len(y) - 1
    negloglikelihood = 0
    r_2: float = 0
    for t in range(1, n + 1):
        x_, P_ = KF.predict()#KF.predict(x[t], P[t])
        eps = y[t] - KF.H @ x_
        r_2 += float(eps @ eps.T)
        Sigma = KF.H @ P_ @ KF.H.T + KF.R
        tmp = np.linalg.solve(Sigma, eps)  # Sigma inversion
        negloglikelihood += 0.5 * (np.log(np.linalg.det(Sigma)) + eps.T @ tmp)
    return float(negloglikelihood)






    
class whiteKF:
    """
    Standard Kalman filter implementation
    Implementation follows eq. (2, 3) from [1]
    Parameters
    ----------
    Phi : np.ndarray of shape(n_states, n_states)
        State transfer matrix
    Q : np.ndarray of shape(n_states, n_states)
        Process noise covariance matrix
    H : np.ndarray of shape(n_meas, n_states)
        Matrix of the measurements model; maps state to measurements
    R : np.ndarray of shape(n_meas, n_meas)
        Driving noise covariance matrix for the noise AR model
    init_state : Gaussian, optional, default=None
        Initial state vector and its covariance
    References
    ----------
    .. [1] Wang, Kedong, Yong Li, and Chris Rizos. “Practical Approaches to
    Kalman Filtering with Time-Correlated Measurement Errors.” IEEE
    Transactions on Aerospace and Electronic Systems 48, no. 2 (2012): 1669–81.
    https://doi.org/10.1109/TAES.2012.6178086.
    """
    
    

    def __init__(self, Phi, Q, H, R, init_x = None, init_P = None):
        self.Phi = Phi
        
        
        self.Phi = Phi
        
        if Q is None:
            self.Q = 1e-3
        else:
            self.Q = Q
        self.H = H
        if R is None:
            self.R = 1e-3
        else:
            self.R = R

        n_states = Phi.shape[0]
        if (init_x is None) or (init_P is None):
            
            self.x = np.zeros((n_states,1))#np.random.randn(n_states, 1)
            self.P = np.eye(n_states) * 1e-3
        else:
            self.x = init_x
            self.P = init_P

    def predict(self):
        x_ = self.Phi @ self.x
        P_ = self.Phi @ self.P @ self.Phi.T + self.Q
        return x_, P_

    def update(self, y, x_, P_):
        Sigma = self.H @ P_ @ self.H.T + self.R
        Pxn = P_ @ self.H.T

        K = np.linalg.solve(Sigma, Pxn.T).T
        n = y - self.H @ x_
        self.x = x_ + K @ n
        self.P = P_ - K @ Sigma @ K.T
        
        return self.x, self.P

    def update_no_meas(self, x_, P_):
        """Update step when the measurement is missing"""
        self.x = x_
        self.P = P_
        return self.x, self.P



    def step(self, y):
         x_, P_ = self.predict()
         return self.update(y, x_, P_) if y is not None else self.update_no_meas(x_, P_)

    
    def apply(self, data):
        data = np.array(data)
        Npoints = data.shape[0]
        filtered = np.zeros(Npoints)
        analytic = np.zeros((Npoints,2))
        for i in range(Npoints):
            filtered[i] =  self.H@self.step(data[i])[0]
            analytic[i,:] = self.step(data[i])[0][:,0]
        return filtered, analytic
    
    
    #def fit_params(self, init_params):
        
    
        
    
#LOAD REAL DATA


import mne
from matplotlib import pyplot as plt

def offline_test_kf():
    
    raw = mne.io.read_raw_bdf('C:/Users/Fedosov/Documents/NeoRec/Records/resting_state_prediciton/NeoRec_1.bdf')
    
    data = raw[:,:][0]
    
    fs = raw.info['sfreq']
    
    central_f = 10.0
    A = 0.99
    
    Phi, H = generate_matsuda_params(fs, central_f, A)
    
    
    
    
    
    kalman = whiteKF(Phi = Phi, H = H, Q = None, R = None, init_x = None, init_P = None)
    
    kalman = fit_kf_parameters(data[0,:], kalman, cf = central_f, A = A, fs = fs)
    
    
    _, analytic = kalman.apply(data[0,:])
    
    plt.figure()
    plt.plot(np.linalg.norm(analytic,axis = 1))
  

def offline_test_cfir(): # make path as argument
    raw = mne.io.read_raw_bdf('C:/Users/Fedosov/Documents/NeoRec/Records/resting_state_prediciton/NeoRec_1.bdf')
    
    data = raw[:,:][0]
    
    fs = raw.info['sfreq']
    
    
    cfir = CFIRBandEnvelopeDetector(band = [8.0,12.0], fs = fs, delay_ms=0, n_taps=fs//5, n_fft=fs, reg_coeff=0)
    
    
    filtered = cfir.apply(data)
    
    plt.figure()
    plt.plot(np.abs(filtered[0]))
    #plt.plot(data[0])
    
    
    
  
    
    
    
    



    
    
    
    
    
    

    






