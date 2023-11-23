# -*- coding: utf-8 -*-
"""
Created on Sun Sep 24 14:01:12 2023

@author: Fedosov
"""


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
    def __init__(self, band,lensec,srate,window = 'hamming'):
        self.band = band
        self.srate = srate
        self.window = window
        self.lensec = lensec
        
        self.numtaps = self.lensec()
        
        self.b = sn.firwin(self.numtaps, cutoff = self.band, window=self.window, pass_zero=False, fs=self.srate)  
        self.b_cfir = sn.hilbert(self.b)
        
        
        
    def step(self):
        pass
        
    def apply(self,y):
        filtered = sn.lfilter(self.b_cfir, 1.0,y)
        
        return filtered
        
        
        
class CFIR_leastsquares:
    pass
        
    
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







import sys
from PyQt5 import QtCore, QtGui, QtWidgets
import numpy as np
#from pynfb.widgets.helpers import ch_names_to_2d_pos
try:
    from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
except ImportError:
    from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas

from matplotlib.figure import Figure
from matplotlib import rcParams
rcParams['font.size'] = 8
try:
    from mne.viz import plot_topomap
except ImportError:
    pass


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
    '''
    def test_update_figure(self):

        from pynfb.inlets.montage import Montage
        montage = Montage(names=['Fp1', 'Fp2', 'Cz', 'AUX', 'MEG 2632'])
        print(montage)
        data = np.random.randn(3)
        pos = np.array([(0, 0), (1, -1), (-1, -1)])
        self.update_figure(data=data, pos=pos, names=['c1', 'c2', 'oz'], montage=montage)

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











        
        
        
