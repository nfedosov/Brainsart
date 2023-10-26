from pylsl.pylsl import StreamOutlet, StreamInfo
import numpy as np
import time
import matplotlib.pyplot as plt

import mne
raw= mne.io.read_raw_bdf('NeoRec_1.bdf')
raw.plot()
plt.show()




chNames = ['C3-A1','Cz-A1A2', 'F3-A1','Fz-A1A2']

raw = raw[chNames,:][0]
#chname2 = {'C3': 'C3','Cz': 'Cz','F3': 'F3','Fz': 'Fz','Fc1': 'FC1','Fc5': 'FC5','Cp5': 'CP5'}
#raw= raw[chNames][0]
#print(raw.shape)
chCount = len(chNames)

class BCIStreamOutlet(StreamOutlet):
    def __init__(self, name = 'NVX52_Data', fs = 1000):
        # create stream info
        info = StreamInfo(name=name, type='BCI', channel_count=chCount, nominal_srate=fs,
                          channel_format='float32', source_id='myuid34234')

        # set channels labels (in accordance with XDF format, see also code.google.com/p/xdf)
        chns = info.desc().append_child("channels")
        for chname in chNames:
            ch = chns.append_child("channel")
            ch.append_child_value("label", chname)

        # init stream
        super(BCIStreamOutlet, self).__init__(info,chunk_size = 1)


fs = 1000
outlet = BCIStreamOutlet(fs = fs)

start_time = time.time()
model_time = 0.0
Ns = 1
count = 0
while(1):
    cur_time = time.time()
    if cur_time-start_time > model_time+Ns/fs:

        x = raw[:,count].tolist()#(np.random.randn(4)*1e5).tolist()#raw[:,count].tolist()#np.random.randn(chCount*Ns,).tolist()
        outlet.push_chunk(x)
        model_time += Ns/fs
        count += 1

    if model_time > 200:#200.0:
        del outlet
        break