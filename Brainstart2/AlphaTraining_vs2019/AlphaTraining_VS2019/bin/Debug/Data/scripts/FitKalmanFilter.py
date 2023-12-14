import numpy as np
import mne
import pickle
#from utils_EEG_analysis import WhiteKalman
import sys



import numpy as np
import scipy.signal as sn
from mne.channels import make_standard_montage

from matplotlib import pyplot as plt


class BaseKalman:
    def __init__(self, H, Phi, Q, R):
        
        self.x = np.zeros((Phi.shape[0],1))
        self.P = np.eye(Phi.shape[0])
        
        self.Q = Q
        self.R = R
        self.H = H
        self.Phi = Phi
        
        
        
    def step(self, y):
        self.x_ = self.Phi@self.x
        
        self.P_ = self.Phi@self.P@self.Phi.T+self.Q
        
        self.res = y-self.H@self.x_
        
        self.S = self.H@self.P_@self.H.T+self.R
        
        self.K = self.P_@self.H.T/self.S
        
        self.x = self.x_+self.K@self.res
        
        self.P = (np.eye(self.P.shape[0])-self.K@self.H)@self.P_
        #print(self.P[0,0])
        
        
    
        
        
        
class WhiteKalman(BaseKalman):
    def __init__(self, freq0, A, srate, q, r):
        
        self.freq0 = freq0
        self.A = A
        H = np.array([[1.0,0.0]])
        Phi = A*np.array([[np.cos(2.0*np.pi*freq0/srate),-np.sin(2.0*np.pi*freq0/srate)],[np.sin(2.0*np.pi*freq0/srate),np.cos(2.0*np.pi*freq0/srate)]])
        
        self.Q = np.eye(2)*q
        self.R = r
        
        
        
        super().__init__(H, Phi,self.Q, self.R)
        
        
   
    def apply(self, y):
        
        Len = y.shape[0]
        filtered_real =np.zeros(Len)
        filtered_imag = np.zeros(Len)
        envelope =np.zeros(Len)
        phase = np.zeros(Len)
        complex_signal = np.zeros(Len, dtype = 'complex')
        
        for i in range(Len):
            self.step(y[i])
            #filtered_real[i] = self.x[0]
            #filtered_imag[i] = self.x[1]
            #envelope[i] = np.sqrt(self.x[0]**2+self.x[1]**2)
            #phase[i] = np.angle(self.x[0]+self.x[1]*1j)
            complex_signal[i] = self.x[0]+self.x[1]*1j
            
        return complex_signal#,filtered_real, filtered_imag, envelope, phase
       
    
    
class ExponentialSmoother:
    def __init__(self, factor):
        self.a = [1, -factor]
        self.b = [1 - factor]
        self.zi = np.zeros((max(len(self.a), len(self.b)) - 1,))

    def apply(self, chunk: np.ndarray):
        y, self.zi = sn.lfilter(self.b, self.a, chunk, zi=self.zi)
        return y
       
       
       
       
       
class CFIR_firbased:
    def __init__(self, band,numtaps,srate):
        self.band = band
        self.srate = srate
      
        
        self.numtaps = numtaps
        
        self.b = sn.firwin(self.numtaps, cutoff = self.band, window='hamming', pass_zero=False, fs=self.srate)  
        self.b=self.b/np.linalg.norm(self.b)
        
        self.b_cfir = sn.hilbert(self.b)
        
        
        
    #def step(self):
    #    pass
        
    def apply(self,y):
        filtered = sn.lfilter(self.b_cfir, 1.0,y)
        
        return filtered
        
    
    
'''
def apply_firh_and_cfir(signal,band,window,numtaps,srate):
    b = sn.firwin(numtaps, cutoff = band, window=window, pass_zero=False, fs=srate)  
    b_cfir = sn.hilbert(b)
    cfir_filtered = sn.lfilter(b_cfir,1.0,signal)
    fir_filtered = sn.lfilter(b,1.0,signal)
    firh_filtered = sn.hilbert(fir_filtered)
    #firh_filtered = np.zeros(len(signal))
    #for i in range(numtaps//2,len(signal)-numtaps//2):
    #    firh_filtered[i] = sn.hilbert(fir_filtered[i-numtaps//2:i+numtaps//2+1])[numtaps//2]
    return cfir_filtered, firh_filtered
'''







#import sys
#from PyQt5 import QtCore, QtGui, QtWidgets
import numpy as np
#from pynfb.widgets.helpers import ch_names_to_2d_pos
#try:
#    from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
#except ImportError:
#    from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas

from matplotlib.figure import Figure
from matplotlib import rcParams
rcParams['font.size'] = 8
try:
    from mne.viz import plot_topomap
except ImportError:
    pass

'''
class TopographicMapCanvas(FigureCanvas):
    def __init__(self, parent=None, width=5, height=4, dpi=100):
        self.fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = self.fig.add_subplot(111)
        self.colorbar = None
        FigureCanvas.__init__(self, self.fig)
        self.setParent(parent)
        FigureCanvas.setSizePolicy(self,
                                   QtWidgets.QSizePolicy.Expanding,
                                   QtWidgets.QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)

    def update_figure(self, data, pos=None, names=None, show_names=None, show_colorbar=True, central_text=None,
                      right_bottom_text=None, show_not_found_symbol=False, montage=None):
        
        
        
        montage = make_standard_montage("standard_1005")
        all_pos = montage.get_positions()['ch_pos']
        
        
        pos = np.zeros((len(names),3))
        
        
       
        #new_names = []
        #for name in names:
        #    if name.lower == 'C3':
                
                
        names = ['O1','O2','P3','P4']
                
        
        
        for i,name in enumerate(names):
            pos[i] = all_pos[name]
            
        pos = pos[:,:2]
            
        
        #if montage is None:
        #    if pos is None:
        #        pos = ch_names_to_2d_pos(names)
        #else:
        #pos = montage.get_pos('EEG')
        #names = montage.get_names('EEG')
        data = np.array(data)
        self.axes.clear()
        if self.colorbar:
            self.colorbar.remove()
        if show_names is None:
            show_names = ['O1', 'O2', 'CZ', 'T3', 'T4', 'T7', 'T8', 'FP1', 'FP2']
        show_names = [name.upper() for name in show_names]
        mask = np.array([name.upper() in show_names for name in names]) if names else None
        v_min, v_max = None, None
        if (data == data[0]).all():
            data[0] += 0.1
            data[1] -= 0.1
            v_min, v_max = -1, 1
        a, b = plot_topomap(data, pos, axes=self.axes, show=False, contours=0, names=names,
                            mask=mask,
                            mask_params=dict(marker='o',
                                             markerfacecolor='w',
                                             markeredgecolor='w',
                                             linewidth=0,
                                             markersize=3))
        
        plt.savefig('topomap.png', dpi=96)
        
        if central_text is not None:
            self.axes.text(0, 0, central_text, horizontalalignment='center', verticalalignment='center')

        if right_bottom_text is not None:
            self.axes.text(-0.65, 0.65, right_bottom_text, horizontalalignment='left', verticalalignment='top')

        if show_not_found_symbol:
            self.axes.text(0, 0, '/', horizontalalignment='center', verticalalignment='center')
            self.axes.text(0, 0, 'O', size=10, horizontalalignment='center', verticalalignment='center')

        if show_colorbar:
            self.colorbar = self.fig.colorbar(a, orientation='horizontal', ax=self.axes)
            self.colorbar.ax.tick_params(labelsize=6)
            self.colorbar.ax.set_xticklabels(self.colorbar.ax.get_xticklabels(), rotation=90)
        self.draw()

    def draw_central_text(self, text='', right_bottom_text='', show_not_found_symbol=False):
        data = np.random.randn(3)*0
        pos = np.array([(0, 0), (1, -1), (-1, -1)])
        self.update_figure(data, pos, central_text=text, names=[], show_colorbar=False,
                           right_bottom_text=right_bottom_text, show_not_found_symbol=show_not_found_symbol)
  '''  '''
    def test_update_figure(self):

        from pynfb.inlets.montage import Montage
        montage = Montage(names=['Fp1', 'Fp2', 'Cz', 'AUX', 'MEG 2632'])
        print(montage)
        data = np.random.randn(3)
        pos = np.array([(0, 0), (1, -1), (-1, -1)])
        self.update_figure(data=data, pos=pos, names=['c1', 'c2', 'oz'], montage=montage)

    '''

'''


class TopoFilterCavas(QtWidgets.QWidget):
    def __init__(self, parent, names, topo, filter, size = 100):
        super(TopoFilterCavas, self).__init__(parent)

        # topography layout
        topo_canvas = TopographicMapCanvas()
        topo_canvas.setMaximumWidth(size)
        topo_canvas.setMaximumHeight(size)
        topo_canvas.update_figure(topo, names=names, show_names=[], show_colorbar=False)

        # filter layout
        filter_canvas = TopographicMapCanvas()
        filter_canvas.setMaximumWidth(size)
        filter_canvas.setMaximumHeight(size)
        filter_canvas.update_figure(filter, names=names, show_names=[], show_colorbar=False)
        filter_canvas.setHidden(True)

        # layout
        layout = QtWidgets.QHBoxLayout(self)
        layout.addWidget(topo_canvas)
        layout.addWidget(filter_canvas)

        # attr
        self.show_filter = False
        self.topo = topo_canvas
        self.filter = filter_canvas
        self.names = names


    def switch(self):
        self.show_filter = not self.show_filter
        self.filter.setHidden(not self.show_filter)
        self.topo.setHidden(self.show_filter)

    def update_data(self, topo, filter):
        self.filter.update_figure(filter, names=self.names, show_names=[], show_colorbar=False)
        self.topo.update_figure(topo, names=self.names, show_names=[], show_colorbar=False)


'''


























def load_eeg_data_from_file(filename):
    file = open(filename, "rb")
    container =  pickle.load(file)
    file.close()

    exp_settings = container['exp_settings']
    srate = exp_settings['srate']
    data = container['eeg']
    stims = container['stim']
    
    channel_names = exp_settings['channel_names']
    
    for i in range(len(channel_names)):
        channel_names[i] = channel_names[i].upper()
        print(channel_names)
        n_channels = len(channel_names)

    info = mne.create_info(ch_names=channel_names, sfreq = srate, ch_types = 'eeg',)
    raw =  mne.io.RawArray(data.T, info)
    
       
    # condition1 , condition2, prepare, ready    
    
    cond_N = 0
    if 'Cond1' in exp_settings['blocks']:
        cond_N = 1
        if 'Cond2' in exp_settings['blocks']:
            cond_N = 2
        
    if cond_N > 0:
        duration1 = exp_settings['blocks']['Cond1']['duration']
        cond1_id = int(exp_settings['blocks']['Cond1']['id'])
        
    if cond_N >1:
        duration2= exp_settings['blocks']['Cond2']['duration']
        cond2_id = int(exp_settings['blocks']['Cond2']['id'])
    
    start_times = np.zeros(0, dtype = 'float')
    
    if cond_N > 0:
        where_idx = (stims == cond1_id).astype('int')
        where_idx = np.concatenate([np.zeros(1),where_idx])
        
        deriv_idx = where_idx[1:]-where_idx[:-1]
        
        start_times = np.concatenate([start_times,np.where(deriv_idx > 0)[0]/srate])
    
           
        num_cond1 = len(start_times) 
        
        if cond_N < 2:
            description = []
            for st in start_times:
                if stims[int(round((st*srate)))] == cond1_id:
                    description.append(exp_settings['blocks']['Cond1']['message'])
        
            annotations = mne.Annotations(start_times, duration1, description)
            raw.set_annotations(annotations)
        
        
        
    if cond_N > 1:
        where_idx = (stims == cond2_id).astype('int')
        where_idx = np.concatenate([np.zeros(1),where_idx])
        
        deriv_idx = where_idx[1:]-where_idx[:-1]
        
        start_times = np.concatenate([start_times,np.where(deriv_idx > 0)[0]/srate])
            
        description = []
        for st in start_times:
            if stims[int(round((st*srate)))] == cond2_id:
                description.append(exp_settings['blocks']['Cond2']['message'])
            if stims[int(round((st*srate)))] == cond1_id:
                description.append(exp_settings['blocks']['Cond1']['message'])
                
        num_cond2 = len(description)
        durations = np.concatenate([np.ones(num_cond1)*duration1,np.ones(num_cond2)*duration2])
        
        annotations = mne.Annotations(start_times, duration2, description)
        raw.set_annotations(annotations)
    
    return (raw, exp_settings)


def auto_calc_f0():
    # TODO: caluclate from spectrums
    return 10.5

def compute_ICA_spatial_filter(eeg_data, low_freq = 2, high_freq = 40):

    modified_eeg = eeg_data.copy()
    
    modified_eeg.notch_filter([50.0, 100.0, 150.0, 200.0])
    modified_eeg.filter(low_freq, high_freq)
    
    
    last_idx = (modified_eeg[0,:][0]).shape[1]
    ica = mne.preprocessing.ICA(random_state = 0)

    srate = eeg_data.info['sfreq']
    ica.fit(modified_eeg, start = int(3*srate), stop = int(last_idx-3*srate))
           
    return ica

def fit_kalman_filter(eeg_data, component_num, to_prefilter,relatio,config_file_path):

    srate = eeg_data.info['sfreq']

    # TODO: define exact cutoff freqs
    if to_prefilter:
        b_low, a_low = sn.butter(2,50.0,'low',fs = srate)
        
        b50, a50 = sn.butter(4,[48.0,52.0],'bandstop',fs = srate)
        
        b_high, a_high = sn.butter(2,4.0,'high',fs = srate)
        
        b100, a100 = sn.butter(2,[98.0,102.0],'bandstop',fs = srate)
        b150, a150 = sn.butter(2,[148.0,152.0],'bandstop',fs = srate)
        b200, a200 = sn.butter(2,[198.0,202.0],'bandstop',fs = srate)
        
        
        
        
        


    filter_type = 0
    srate = eeg_data.info['sfreq']

    # Проинициализировать ICA фильтр
    ica = compute_ICA_spatial_filter(eeg_data)

    pca_M = ica.pca_components_
    ica_M = ica.unmixing_matrix_
    unmixing_M = ica_M@pca_M

    spat_filter = unmixing_M[component_num,:]


    filtered_eeg = eeg_data.copy()
    gt_eeg = eeg_data.copy()

    gt_eeg = spat_filter @ gt_eeg[:,:][0]
    filtered_eeg = spat_filter @ filtered_eeg[:,:][0]
    
    if to_prefilter:
        filtered_eeg = sn.lfilter(b_low, a_low, filtered_eeg)
        filtered_eeg = sn.lfilter(b_high, a_high, filtered_eeg)
        filtered_eeg = sn.lfilter(b50, a50, filtered_eeg)
        filtered_eeg = sn.lfilter(b100, a100, filtered_eeg)
        filtered_eeg = sn.lfilter(b150, a150, filtered_eeg)
        filtered_eeg = sn.lfilter(b200, a200, filtered_eeg)
        
    

    central_freq = auto_calc_f0()

    #default_relatio = 1000
    
    trans_relatio = 10**(8*(relatio-0.5))#default_relatio
    A = 0.995
    q = 1.0 * trans_relatio
    r = 1.0
    kalman = WhiteKalman(central_freq, A, srate, q, r)

    cmplx_filtered_eeg = kalman.apply(filtered_eeg)

    envelope = np.abs(cmplx_filtered_eeg)
    arr = np.quantile(envelope,[0.05, 0.095])

    q0 = arr[0]
    q1 = arr[1]

    combined_array = np.concatenate((np.array([len(spat_filter)]),np.array([filter_type]),np.array([to_prefilter]),np.array([central_freq]),np.array([q0]), np.array([q1]), np.array([q]),np.array([r]),np.array([srate]),spat_filter))
    
    # Write the array to a text file with comma+space delimiters
    with open(config_file_path, 'w') as file:
        # here dump, then, in another script - load and change the phase                
        file.write(', '.join(str(x) for x in combined_array))
        
        

def fit_cfir_filter(eeg_data, component_num, to_prefilter,relatio,config_file_path):

    srate = eeg_data.info['sfreq']

    # TODO: define exact cutoff freqs
    if to_prefilter:
        b_low, a_low = sn.butter(2,50.0,'low',fs = srate)
        
        b50, a50 = sn.butter(4,[48.0,52.0],'bandstop',fs = srate)
        
        b_high, a_high = sn.butter(2,4.0,'high',fs = srate)
        
        b100, a100 = sn.butter(2,[98.0,102.0],'bandstop',fs = srate)
        b150, a150 = sn.butter(2,[148.0,152.0],'bandstop',fs = srate)
        b200, a200 = sn.butter(2,[198.0,202.0],'bandstop',fs = srate)
        
        
        
        
        


    filter_type = 1
    srate = eeg_data.info['sfreq']

    # Проинициализировать ICA фильтр
    ica = compute_ICA_spatial_filter(eeg_data)

    pca_M = ica.pca_components_
    ica_M = ica.unmixing_matrix_
    unmixing_M = ica_M@pca_M

    spat_filter = unmixing_M[component_num,:]


    filtered_eeg = eeg_data.copy()
    gt_eeg = eeg_data.copy()

    gt_eeg = spat_filter @ gt_eeg[:,:][0]
    filtered_eeg = spat_filter @ filtered_eeg[:,:][0]
    
    if to_prefilter:
        filtered_eeg = sn.lfilter(b_low, a_low, filtered_eeg)
        filtered_eeg = sn.lfilter(b_high, a_high, filtered_eeg)
        filtered_eeg = sn.lfilter(b50, a50, filtered_eeg)
        filtered_eeg = sn.lfilter(b100, a100, filtered_eeg)
        filtered_eeg = sn.lfilter(b150, a150, filtered_eeg)
        filtered_eeg = sn.lfilter(b200, a200, filtered_eeg)
        
    

    central_freq = auto_calc_f0()

    #default_relatio = 1000
    
    window_sz = ((int(relatio*400+100))//2)*2+1#default_relatio
  
    
    
    
    cfir = CFIR_firbased([central_freq-1.0,central_freq+1.0],window_sz,srate)

    cmplx_filtered_eeg = cfir.apply(filtered_eeg)

    envelope = np.abs(cmplx_filtered_eeg)
    arr = np.quantile(envelope,[0.05, 0.095])

    q0 = arr[0]
    q1 = arr[1]

    combined_array = np.concatenate((np.array([len(spat_filter)]),np.array([filter_type]),np.array([to_prefilter]),np.array([central_freq]),np.array([q0]), np.array([q1]), np.array([window_sz]),np.array([srate]),spat_filter))
    
    # Write the array to a text file with comma+space delimiters
    with open(config_file_path, 'w') as file:
        # here dump, then, in another script - load and change the phase                
        file.write(', '.join(str(x) for x in combined_array))
        
    


def fit_kalman_filter_for_file(file_name, component_num, config_file_name):
    eeg_data, exp_settings = load_eeg_data_from_file(sys.argv[1], True)
    fit_kalman_filter(eeg_data)

if __name__ == '__main__':    
    if(len(sys.argv) > 3):
        eeg_data, exp_settings = load_eeg_data_from_file(sys.argv[1])
        component_num = int(sys.argv[2])
        to_prefilter = int(sys.argv[3])
        filter_type = int(sys.argv[4])
        relatio = int(sys.argv[5])
        config_file_name = sys.argv[6]
        if filter_type == 0:
            fit_kalman_filter(eeg_data, component_num, to_prefilter, relatio,config_file_name)
        if filter_type == 1:
            fit_cfir_filter(eeg_data, component_num, to_prefilter, relatio,config_file_name)