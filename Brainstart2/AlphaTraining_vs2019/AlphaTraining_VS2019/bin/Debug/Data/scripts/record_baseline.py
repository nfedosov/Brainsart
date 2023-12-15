
import datetime
import json
import os
import pickle
import sys
import pylsl
import numpy as np

#from lsl_inlet import LSLInlet




# -*- coding: utf-8 -*-
import numpy as np
from pylsl import StreamInlet, resolve_byprop
from pylsl.pylsl import lib, StreamInfo, FOREVER, c_int, c_double, byref, handle_error
import time
#import socket
import xml.etree.ElementTree as ET
LSL_STREAM_NAMES = ['NVX52_Data']
LSL_RESOLVE_TIMEOUT = 2



class FixedStreamInfo(StreamInfo):
    def as_xml(self):
        return lib.lsl_get_xml(self.obj).decode('utf-8', 'ignore') # add ignore


class FixedStreamInlet(StreamInlet):
    def info(self, timeout=FOREVER):
        errcode = c_int()
        result = lib.lsl_get_fullinfo(self.obj, c_double(timeout),
                                      byref(errcode))
        handle_error(errcode)
        return FixedStreamInfo(handle=result) # StreamInfo(handle=result)

# TODO: make default resolving names and choosing the first stream, dive into the details

class LSLInlet:
    def __init__(self, params, dtype='float'):
        self.params = params
        if not self.params['lsl_stream_name']:
            name = self.params['lsl_stream_name']
        else:
            name = LSL_STREAM_NAMES[0]
        streams = resolve_byprop('name', name, timeout=LSL_RESOLVE_TIMEOUT)
        self.inlet = None
        self.dtype = dtype
        if len(streams) > 0:
            self.inlet = FixedStreamInlet(streams[0], max_buflen=params['max_buflen'],
                                          max_chunklen=params['max_chunklen'])  # ??? Check timing!!!
            print('Connected to {} LSL stream successfully'.format(name))
            self.n_channels = self.inlet.info().channel_count()
        else:
            raise ConnectionError('Cannot connect to "{}" LSL stream'.format(name))

    def get_next_chunk(self):
        # get next chunk
        chunk, timestamp = self.inlet.pull_chunk(max_samples=1000*120)
        # convert to numpy array
        chunk = np.array(chunk, dtype=self.dtype)
        # return first n_channels channels or None if empty chunk
        return (chunk, timestamp) if chunk.shape[0] > 0 else (None, None)

  

    def save_info(self, file):
        with open(file, 'w', encoding="utf-8") as f:
            f.write(self.info_as_xml())

    def info_as_xml(self):
        xml = self.inlet.info().as_xml()
        return xml

    def get_frequency(self):
        self.srate = self.inlet.info().nominal_srate()
        return self.srate

    def get_n_channels(self):
        return self.inlet.info().channel_count()

    def get_channel_names(self):
        xml_info = self.info_as_xml()
        tree = ET.ElementTree(ET.fromstring(xml_info))
        root = tree.getroot()
        for child in root:
            print(child.tag, child.attrib)
        return 'puf-puf'




    #  Research this code
    def get_channels_labels(self):
        for t in range(3):
            time.sleep(0.5*(t+1))
            try:
                # print('wow') TODO too many repetitions
                rt = ET.fromstring(self.info_as_xml())
                channels_tree = rt.find('desc').findall("channel") or rt.find('desc').find("channels").findall(
                    "channel")
                labels = [(ch.find('label') if ch.find('label') is not None else ch.find('name')).text
                          for ch in channels_tree]
                return labels
            except OSError:
                print('OSError during reading channels names', t+1)
        return ['channel'+str(n+1) for n in range(self.get_n_channels())]

    def disconnect(self):
        del self.inlet
        self.inlet = None















def RunPreparedProtocol(protocol_file, result_file):
    # десериализовать список блоков
    data = open(protocol_file,)
    data = json.load(data) 

    # сформировать словирь блоков из созданной последовательности блоков        
    blocks = dict()
    for i in range(len(data)):
        if data[i]["name"] not in blocks:
            blocks[data[i]["name"]] = {'duration': data[i]["duration"], 'id': data[i]["code"], 'message': data[i]["message"]}
    
    # список меток?
    seq2 = list()
    for i in range(len(data)):
        seq2.append(data[i]["name"])
        
    
    streams = pylsl.resolve_streams()

    # подготовить настройки приемника lsl-канала
    exp_settings = {
                'exp_name': 'Baseline',
                'lsl_stream_name': streams[0].name(),
                'blocks': blocks,
                'sequence': seq2,
    
                #максимальная буферизация принятых данных через lsl
                'max_buflen': 5,  # in seconds!!!!
    
                #максимальное число принятых семплов в чанке, после которых появляется возможность
                #считать данные
                'max_chunklen': 1,  # in number of samples!!!!
    
    
                ##какой канал использовать
                #'channels_subset': 'P3-A1',
                #'bands': [[9.0, 13.0],[18.0,25.0]],
                #'results_path': results_path
                }
    
    inlet = LSLInlet(exp_settings)
    inlet.srate = inlet.get_frequency()
    xml_info = inlet.info_as_xml()
    channel_names = inlet.get_channels_labels()
    print(channel_names)
    exp_settings['channel_names'] = channel_names
    
    n_channels = len(channel_names)
    
    
    srate = int(round(inlet.srate)) #Hz
    exp_settings['srate'] = srate
    
    #max buffer length
    total_samples = 0
    
    for block_name in exp_settings['sequence']:
        current_block = exp_settings['blocks'][block_name]
        total_samples += round(srate * current_block['duration'])
    
    data = np.zeros((total_samples+round(exp_settings['max_buflen']*srate),n_channels))
    print(data.shape)
    stims =  np.zeros((total_samples+round(exp_settings['max_buflen']*srate),1)).astype(int)
    
    
    n_samples_received = 0
    n_samples_received_in_block = 0
    
        
    block_idx = 0
    block_name = exp_settings['sequence'][0]
    current_block = exp_settings['blocks'][block_name]

    # посчитать количество отсчетов с учтом частоты
    n_samples = srate * current_block['duration']
    block_id = current_block['id']      
        
    
    while (1):

        # как только получли нужно количество отсчетов, переходим к следующему шагу
        if n_samples_received_in_block >= n_samples:
            dif =  n_samples_received_in_block - n_samples
            
            
            block_idx += 1
            
            
            if block_idx >= len(exp_settings['sequence']):
                #save_and_finish()
                inlet.disconnect()
    
                break
    
            block_name = exp_settings['sequence'][block_idx]
            current_block = exp_settings['blocks'][block_name]
            n_samples = srate * current_block['duration']
            n_samples_received_in_block = dif
            block_id = current_block['id']
            if dif>0:
                stims[n_samples_received-dif:n_samples_received] = block_id
        
    
        # получить очередной чанк из lsl-канала
        chunk, t_stamp = inlet.get_next_chunk()
        # print(f"{chunk=}")
        if chunk is not None:
            n_samples_in_chunk = len(chunk)

            # сохранить полученные отсчеты в буфер
            data[n_samples_received:n_samples_received + n_samples_in_chunk, :] = chunk

            # пометить полученные отсчеты идентификатором текущего блока
            stims[n_samples_received:n_samples_received + n_samples_in_chunk] = block_id

            # просуммировать количество полученных отсчетов в рамках блока
            n_samples_received_in_block += n_samples_in_chunk

            # просуммировать общее количество полученных отсчетов
            n_samples_received += n_samples_in_chunk
    
          
    # отсекаем только нужное количество отсчетов
    data = data[:total_samples]
    stims = stims[:total_samples,0]
    
    
    #os.makedirs(results_path)
    
    file = open(result_file, "wb")
    pickle.dump({'eeg': data, 'stim': stims,
            'xml_info': xml_info, 'exp_settings': exp_settings}, file = file)
    file.close()
    
    return

if __name__ == '__main__':
    # проверяем входные параметры
    print("function enter")
    if(len(sys.argv) > 2):
        print("call RunPreparedProtocol")
        RunPreparedProtocol(sys.argv[1], sys.argv[2])
    else:
        print("Invalid number of parameters")
