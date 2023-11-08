using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

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

        public override bool Run(string argument)
        {
            // в качестве аргумента пришел
            string scenario = argument;

            // сгенерировать имя выходного файла, который будет содержать baseline
            _recordingFilename = Path.GetFullPath(
                String.Format(@"./Data/users/{0}/baseline_{1}.pickle", _mainWindow.GetUserName(), DateTime.Now.Ticks));
            
            using (StreamReader sr = new StreamReader(File.OpenRead(scenario)))
            {
                List<ProtocolBlock> blocks = JsonSerializer.Deserialize<List<ProtocolBlock>>(sr.ReadToEnd());

                if (null != blocks)
                {
                    // Запустить Python-скрипт, который запустит запись согласно меткам в протоколе
                    ProcessStartInfo startInfo = new ProcessStartInfo();
                    startInfo.FileName = @"c:\Users\elena\anaconda3\Python.exe";
                    startInfo.Arguments = @"./Data/scripts/record_baseline.py " + scenario + " " + _recordingFilename;
                    startInfo.UseShellExecute = false;
                    startInfo.CreateNoWindow = true;
                    var process = Process.Start(startInfo);
                    if (null != process)
                    {
                        BaselineRecording baselineRecording = new BaselineRecording();
                        baselineRecording.PlayScenario(blocks);
                        baselineRecording.ShowDialog();
                                                
                        // Ждем, когда скрипт отработает
                        process.WaitForExit();
                    }
                }
            }

            return true;
        }
    }
}
