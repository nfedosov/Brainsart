import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QHBoxLayout,\
    QVBoxLayout, QWidget, QLabel, QListWidget, QListWidgetItem,\
        QFileDialog, QComboBox
from PyQt5.QtCore import Qt, QMimeData, pyqtSignal, QRect, QCoreApplication
from PyQt5.QtGui import QDrag, QPixmap, QFont
from PyQt5 import QtCore, QtGui, QtWidgets
import pickle



import numpy as np
from datetime import datetime
import os
from lsl_inlet import LSLInlet
import pylsl

# new:
import json


class ProtocolBlock:
    def __init__(self, name, duration,message,code):
        self.name = name
        self.duration = duration
        self.message = message
        self.code = code


class BlockDialog(QtWidgets.QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Add Block")
        self.block = None
        self.init_ui()

    def init_ui(self):
        layout = QtWidgets.QVBoxLayout()

        name_label = QtWidgets.QLabel("Name:")
        self.name_edit = QtWidgets.QLineEdit()

        duration_label = QtWidgets.QLabel("Duration (s):")
        self.duration_spin = QtWidgets.QDoubleSpinBox()
        self.duration_spin.setDecimals(2)

        message_label = QtWidgets.QLabel("Message:")
        self.message_edit = QtWidgets.QLineEdit()

        code_label = QtWidgets.QLabel("Code:")
        self.code_edit = QtWidgets.QLineEdit()

        add_button = QtWidgets.QPushButton("Add")
        add_button.clicked.connect(self.add_block)

        layout.addWidget(name_label)
        layout.addWidget(self.name_edit)
        layout.addWidget(duration_label)
        layout.addWidget(self.duration_spin)
        layout.addWidget(message_label)
        layout.addWidget(self.message_edit)
        layout.addWidget(code_label)
        layout.addWidget(self.code_edit)
        layout.addWidget(add_button)

        self.setLayout(layout)

    def add_block(self):
        name = self.name_edit.text()
        duration = self.duration_spin.value()
        message = self.message_edit.text()
        code = self.code_edit.text()

        if name and duration and message and code:
            self.block = ProtocolBlock(name, duration, message, code)
            self.accept()
        else:
            QtWidgets.QMessageBox.warning(
                self, "Incomplete Information", "Please fill in all fields.")

    def get_block(self):
        return self.block

class ProtocolEditor(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        self.protocol_blocks = []
        self.init_ui()

    def init_ui(self):
        self.setAcceptDrops(True)
        self.layout = QtWidgets.QVBoxLayout()
        
        
        self.init_label = QLabel('DROP HERE')#QtWidgets.QListWidget()
        self.layout.addWidget(self.init_label)

        

        self.setLayout(self.layout)

        self.lsl_button = QtWidgets.QPushButton('UPD lsl streams')
        self.lsl_button.clicked.connect(self.upd_lsl_streams)
        #self.layout.addWidget(self.lsl_button)
        
        self.lsl_combobox = QComboBox(self)
        self.lsl_combobox.ReadOnly = True
        self.lsl_combobox.currentIndexChanged.connect(self.choose_lsl)
        #self.layout.addWidget(self.lsl_combobox)


        self.start_button = QtWidgets.QPushButton('Start')
        self.start_button.clicked.connect(self.onStartButtonClicked)
        #self.layout.addWidget(self.start_button)


    

    #def update_block_list(self):
    #    self.block_list_widget.clear()
    #    for block in self.protocol_blocks:
    #        item = QtWidgets.QListWidgetItem(block.name)
    #        item.setData(QtCore.Qt.UserRole, block)
    #        self.block_list_widget.addItem(item)
    
    def upd_lsl_streams(self):
        """Query available LSL streams and update stream selection combobox."""
        self.streams = pylsl.resolve_streams()
        
        for stream in self.streams:
            self.lsl_combobox.addItem(stream.name())
        
        if self.streams:
            self.inlet_info = self.streams[0]
            
        
            
            
    def choose_lsl(self, idx):
        
        self.inlet_info = self.streams[idx]
        print(self.inlet_info)
    
    def RecordBaseline(self, protocol_file_name):
    
        # Загрузить файл протокола

        # проинициализировать переменные

        self.onStartButtonClicked(self)

    def onStartButtonClicked(self):
        
        self.stim_label = QLabel('+')        
        self.stim_label.setFixedSize(1000, 700)        
        self.stim_label.setAlignment(Qt.AlignCenter)        
        font = QFont( "Gost", 50)# QFont.Bold)
        self.stim_label.setFont(font)
        self.stim_label.show()
        
        # сформировать словирь блоков из созданной последовательности блоков        
        blocks = dict()
        for i in range(len(self.protocol_blocks)):
            if self.protocol_blocks[i].name not in blocks:
                blocks[self.protocol_blocks[i].name] = {'duration': self.protocol_blocks[i].duration, 'id': self.protocol_blocks[i].code, 'message': self.protocol_blocks[i].message}
        
        # список меток?
        seq2 = list()
        for i in range(len(self.protocol_blocks)):
            seq2.append(self.protocol_blocks[i].name)
            
        # сформировать имя результирующего файла
        timestamp_str = datetime.strftime(datetime.now(), '%m-%d_%H-%M-%S')
        results_path = 'results/{}_{}/'.format('baseline_experiment', timestamp_str)
            
        # подготовить настройки приемника lsl-канала
        exp_settings = {
                    'exp_name': 'Baseline',
                    'lsl_stream_name': self.inlet_info.name(),
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
                    'results_path': results_path}
        
        inlet = LSLInlet(exp_settings)#LSLInlet(exp_settings)
        inlet.srate = inlet.get_frequency()
        xml_info = inlet.info_as_xml()
        channel_names = inlet.get_channels_labels()
        print(channel_names)
        exp_settings['channel_names'] = channel_names
        #ch_idx = np.squeeze(np.where(channel_names == np.array(exp_settings['channels_subset']))[0])
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
        
        
        #buffer = np.empty((n_seconds * self.srate + 100 * self.srate, self.n_channels))
        #buffer_stims = np.empty(n_seconds * self.srate + 100 * self.srate)
        
        block_idx = 0
        block_name = exp_settings['sequence'][0]
        current_block = exp_settings['blocks'][block_name]

        # посчитать количество отсчетов с учтом частоты
        n_samples = srate * current_block['duration']
        block_id = current_block['id']
        
        
        
        self.stim_label.setText(current_block['message'])
        QCoreApplication.processEvents()
    
        
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
                
                
        
                self.stim_label.setText(current_block['message'])
                QCoreApplication.processEvents()
            
        
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
        
        
        os.makedirs(results_path)
        
        file = open(results_path + 'data.pickle', "wb")
        pickle.dump({'eeg': data, 'stim': stims,
                'xml_info': xml_info, 'exp_settings': exp_settings}, file = file)
        file.close()
        
        print('Finished')
        
        self.stim_label.close()
    # END of onStartButtonClicked function

    def dragEnterEvent(self, event):
        # Check if the dragged data is of type DragItem
        event.accept()
            
    def dropEvent(self, e):
        pos = e.pos()
        widget = e.source()
    
        N_pos = 0#self.blayout.count()  # Initialize to the end by default
    
        for n in range(self.layout.count()):
            w = self.layout.itemAt(n).widget()
    
            #if self.blayout.orientation() == Qt.Vertical:
            drop_here = pos.y() < w.y() + w.size().height() // 2
            #else:
            #    drop_here = pos.x() < w.x() + w.size().width() // 2
    
            if drop_here:
                N_pos = n
                break
            
            
        self.layout.insertWidget(N_pos, widget)    
        self.protocol_blocks.insert(N_pos,widget.block)
        
        
        copy_object = DragItem()
        copy_object.set_data(widget.name)
        copy_object.position = widget.position
        #self.drag_objects.append(new_object)
        copy_object.parent_ref = widget.parent_ref
        copy_object.block = widget.block
        widget.parent_ref.blayout.insertWidget(copy_object.position, copy_object)#addWidget(new_object)
        
              
    
        e.accept()
        
        for block in self.protocol_blocks:
            print(block.name)
            
 
            
            
            
            
            
       

class DragItem(QLabel):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.setContentsMargins(25, 5, 25, 5)
        self.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.setStyleSheet("border: 1px solid black; background-color: white")
        # Store data separately from display label, but use label for default.
        self.data = self.text()
        
        #palette =self.palette()
        #palette.setColor(self.backgroundRole(), QtCore.Qt.darkGreen)
        
        
        self.selected = False
        
        self.position = None
        
        self.block = None

    def set_data(self, data):
        #self.data = data
        self.name = data
        self.setText(data)

    def mouseMoveEvent(self, e):
        if e.buttons() == Qt.LeftButton:
            drag = QDrag(self)
            mime = QMimeData()
            drag.setMimeData(mime)

            pixmap = QPixmap(self.size())
            self.render(pixmap)
            drag.setPixmap(pixmap)

            drag.exec_(Qt.CopyAction)#MoveAction)#Qt.MoveAction)


class DragWidget(QWidget):
    #orderChanged = pyqtSignal(list)
    
    orderChanged = pyqtSignal(list)

    def __init__(self):
        super().__init__()
        self.setAcceptDrops(True)
        
        
        self.protocol_blocks = list()
        
        self.blayout = QVBoxLayout()
        #self.block_list_widget = QtWidgets.QListWidget()
        #self.block_list_widget.setAcceptDrops(True)
        #self.blayout.addWidget(self.block_list_widget)
         
        self.add_block_button = QtWidgets.QPushButton("Add Block")
        self.add_block_button.clicked.connect(self.show_block_dialog)
        
        self.save_button = QtWidgets.QPushButton("save blocks")
        self.save_button.clicked.connect(self.onSaveButtonClicked)
        
        self.load_button = QtWidgets.QPushButton("load blocks")
        self.load_button.clicked.connect(self.onLoadButtonClicked)

        #self.blayout.addWidget(add_block_button) 
        #self.blayout.addWidget(save_button)      
        #self.blayout.addWidget(load_button)
        self.setLayout(self.blayout)     

        
    def mousePressEvent(self, event):
        self.selection_start = event.pos()
        for i in range(len(self.protocol_blocks)):
            widget = self.blayout.itemAt(i).widget()
            widget.selected = False
        self.selected_items = []
        self.update()    
    
    def mouseReleaseEvent(self, event):
        self.selection_start = None
        
    def mouseMoveEvent(self, event):
        if self.selection_start:
            self.selected_items = []
            selection_area = QRect(self.selection_start, event.pos()).normalized()
            
            for i in range(len(self.protocol_blocks)):
                widget = self.blayout.itemAt(i).widget()
                if selection_area.intersects(widget.geometry()):
                    widget.selected = True
                    self.selected_items.append(widget)
            self.update()

  
    def paintEvent(self, event):
        pass
      
    

    def dragEnterEvent(self, e):
        e.accept()

    def dropEvent(self, e):
        pos = e.pos()
        widget = e.source()
    
        N_pos = 0#self.blayout.count()  # Initialize to the end by default
    
        for n in range(self.blayout.count()-1):
            w = self.blayout.itemAt(n).widget()
    
            #if self.blayout.orientation() == Qt.Vertical:
            drop_here = pos.y() < w.y() + w.size().height() // 2
            #else:
            #    drop_here = pos.x() < w.x() + w.size().width() // 2
    
            if drop_here:
                N_pos = n
                break
            
        #if self.blayout.count()>(N_pos-2):
        self.blayout.insertWidget(N_pos, widget)
        popped = self.protocol_blocks.pop(widget.position)
        self.protocol_blocks.insert(N_pos,popped)
        self.reassign_positions(N_pos)
        #else:
        #    self.blayout.insertWidget(N_pos-1, widget)
            
        #self.orderChanged.emit(self.get_item_data())
       

        e.accept()
        
       

    def add_item(self, item):
        self.blayout.insertWidget(0, item)
        item.parent_ref = self
        self.reassign_positions(0)
        
        
        
    def reassign_positions(self,new_pos):
        for i in range(new_pos+1,len(self.protocol_blocks)):
            self.blayout.itemAt(i).widget().position += 1
    
    
    def show_block_dialog(self):
        dialog = BlockDialog(self)
        if dialog.exec_():
            block = dialog.get_block()
            #block.parent_ref = self
            self.protocol_blocks.insert(0,block)
            self.update_block_list(block)
            
         
    def update_block_list(self,block):
        #self.block_list_widget.clear()
        #for block in self.protocol_blocks:
        item = DragItem()
        item.set_data(block.name)
        item.position = 0#QtWidgets.QListWidgetItem(block.name)
            #item.setData(QtCore.Qt.UserRole, block)
            #self.blayout.addWidget(item)
            #self.block_list_widget.addItem(item)
            
        item.block = block
        self.add_item(item)
        
        
    def onSaveButtonClicked(self):
        options = QFileDialog.Options()
        options |= QFileDialog.ReadOnly  # To make sure the file is only saved
        file_dialog = QFileDialog()
        file_path, _ = file_dialog.getSaveFileName(self, "Save Data", "", "Text Files (*.txt);;All Files (*)", options=options)

        # Check if a valid file path is provided
        if file_path:
            list_to_save = self.protocol_blocks

            # Save the data to the selected file
            with open(file_path, "wb") as file:
                #file.write(data_to_save)
                pickle.dump(list_to_save, file)
                
    def onLoadButtonClicked(self):
        file_dialog = QFileDialog(self)
        self.eeg_file_path, _ = file_dialog.getOpenFileName(self, 'Load list of blocks', '')#, #'EEG Data Files (*.eeg)')

        if self.eeg_file_path:
            # Load EEG data from the selected file
            # Update status label
            file = open(self.eeg_file_path, "rb")
            container =  pickle.load(file)
            file.close()
            
            for i in range(len(self.protocol_blocks)):
                widget = self.blayout.itemAt(0).widget()
                if widget:
                    widget.deleteLater()
                    self.blayout.removeItem(self.blayout.itemAt(0))
            
            
            #self.protocol_blocks = container
            
            
            
            for i in range(len(container)):
                self.protocol_blocks.insert(0,container[len(container)-1-i])
                self.update_block_list(self.protocol_blocks[0])
                
            for i in range(len(container)):
                print(self.blayout.itemAt(i).widget().position)
        
                

        


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setFixedSize(640,800)
        self.protocol_editor = ProtocolEditor()
        self.drag = DragWidget()

        

        container = QWidget()
        glob_layout = QHBoxLayout()

        protocol_layout = QVBoxLayout()
        #protocol_layout.addStretch(1)
        protocol_layout.addWidget(self.protocol_editor)
        protocol_layout.addStretch(1)
        
        protocol_layout.addWidget(self.protocol_editor.lsl_button)
        protocol_layout.addWidget(self.protocol_editor.lsl_combobox)
        protocol_layout.addWidget(self.protocol_editor.start_button)
        
        #self.protocol_editor.acceptDrops()
     
        background = QLabel()
        background.setAutoFillBackground(True)
        palette = background.palette()
        palette.setColor(background.backgroundRole(), QtCore.Qt.lightGray)
        background.setPalette(palette)
        
        
        #protocol_layout.setAlignment(Qt.AlignTop)
        
        
        #protocol_button_layout = QVBoxLayout()
        
        
        
        
        protocol_background = QLabel()
        protocol_background.setAutoFillBackground(True)
        palette = protocol_background.palette()
        palette.setColor(background.backgroundRole(), QtCore.Qt.lightGray)
        protocol_background.setPalette(palette)
        

        drag_layout = QVBoxLayout()
        drag_layout.addWidget(self.drag)
        drag_layout.addStretch(1)
        drag_layout.addWidget(self.drag.add_block_button)
        drag_layout.addWidget(self.drag.save_button)
        drag_layout.addWidget(self.drag.load_button)

        glob_layout.addWidget(protocol_background)
        protocol_background.setLayout(protocol_layout)
        #glob_layout.addLayout(drag_layout)
        glob_layout.addWidget(background)
        background.setLayout(drag_layout)
        container.setLayout(glob_layout)
        
        
        

        self.setCentralWidget(container)

def RunPreparedProtocol(filename):
    # десериализовать список блоков
    data = open(filename,)
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
        
    # сформировать имя результирующего файла
    timestamp_str = datetime.strftime(datetime.now(), '%m-%d_%H-%M-%S')
    results_path = 'results/{}_{}/'.format('baseline_experiment', timestamp_str)
        
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
                'results_path': results_path}
    
    inlet = LSLInlet(exp_settings)#LSLInlet(exp_settings)
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
    
    
    os.makedirs(results_path)
    
    file = open(results_path + 'data.pickle', "wb")
    pickle.dump({'eeg': data, 'stim': stims,
            'xml_info': xml_info, 'exp_settings': exp_settings}, file = file)
    file.close()
    
    print('Finished')
    
    return

if __name__ == '__main__':

    # Если пришли параметры, запускаем альтернативный скрипт
    # вынести потом его в отдельный файл!!!
    # len(sys.argv) > 1
    if(1):
        RunPreparedProtocol("c:\\Users\\elena\\Documents\\HSE\\Neurofeedback\\brainstart2\\brainstart2\\Brainsart\\Brainstart2\\AlphaTraining\\bin\\Debug\\net6.0-windows\\Data\\scenarios\\test.json")
    else:
        app = QApplication([])
        w = MainWindow()
        w.show()
        app.exec_()