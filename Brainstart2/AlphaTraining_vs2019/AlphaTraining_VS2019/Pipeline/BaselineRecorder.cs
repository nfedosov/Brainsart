using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text.Json;

namespace AlphaTraining
{
    internal class BaselineRecorder : PipelineItem
    {

        string _recordingFilename;

        public BaselineRecorder(MainWindow mainWindow, string name) 
            : base(mainWindow, name) 
        { 
        
        }

        public override bool CanMoveForward()
        {
            return true;
        }

        public override string GetArguments()
        {
            return _recordingFilename;
        }

        public override void Prepare(string argument)
        {

        }

        public override bool Run(string argument)
        {
            // в качестве аргумента пришел
            string protocolFile = argument;

            // Создать каталог нового сеанса
            string sessionDirectory = Path.GetFullPath(
                String.Format(@"./Data/users/{0}/{1}",
                _mainWindow.GetUserName(),
                _mainWindow.GetSessionDate()));

            if (false == Directory.Exists(sessionDirectory))
            {
                Directory.CreateDirectory(sessionDirectory);
            }

            // сгенерировать имя выходного файла, который будет содержать baseline
            _recordingFilename = String.Format(@"{0}/baseline_{1}.pickle",
                sessionDirectory,
                DateTime.Now.ToString("dd.MM.yyyy_H.mm"));
            
            using (StreamReader sr = new StreamReader(File.OpenRead(protocolFile)))
            {
                List<ProtocolBlock> blocks = JsonSerializer.Deserialize<List<ProtocolBlock>>(sr.ReadToEnd());

                if (null != blocks)
                {
                    // Запустить Python-скрипт, который запустит запись согласно меткам в протоколе
                    ProcessStartInfo startInfo = new ProcessStartInfo();
                    startInfo.FileName = SystemVariables.Instance.PythonPath;
                    startInfo.FileName = "C:/Program Files/Spyder/Python/python.exe";

                    startInfo.Arguments = @"./Data/scripts/record_baseline.py " + protocolFile + " " + _recordingFilename;
                    startInfo.UseShellExecute = false;
                    startInfo.CreateNoWindow = true;

                    var process = Process.Start(startInfo);
                    if (null != process)
                    {
                        BaselineRecording baselineRecording = new BaselineRecording();
                        baselineRecording.SetScenario(blocks);
                        baselineRecording.ShowDialog();
                                                
                        // Ждем, когда скрипт отработает
                        process.WaitForExit();

                        // Запускаем следующий скрипт, которрый нарисует графики
                        startInfo = new ProcessStartInfo();
                        startInfo.FileName = SystemVariables.Instance.PythonPath;
                        startInfo.FileName = "C:/Program Files/Spyder/Python/python.exe";
                        startInfo.Arguments = @"./Data/scripts/CreateTimeSeriesPlot.py "
                        + _recordingFilename + " "
                        + _mainWindow.GetSpatialFilerLowerFreq() + " "
                        + _mainWindow.GetSpatialFilerHighFreq();

                        startInfo.UseShellExecute = false;
                        startInfo.CreateNoWindow = true;
                        process = Process.Start(startInfo);
                        if (null != process)
                        {
                            process.WaitForExit();
                        }

                        // Подрузим графики
                        _mainWindow.LoadPlots();
                    }
                }
            }

            return false;
        }
    }
}
