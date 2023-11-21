using System;
using System.Diagnostics;
using System.IO;

namespace AlphaTraining.Pipeline
{
    class ExperimentSettingsPipeline : PipelineItem
    {
        string _configFileName;

        public ExperimentSettingsPipeline(MainWindow mainWindow, string name) 
            : base(mainWindow, name)
        {
        }

        public override string GetArguments()
        {
            return _configFileName;
        }

        public override void Prepare(string argument)
        {

        }

        public override bool Run(string argument)
        {
            // В качестве агрумента пришло имя с файлом крнфигурации
            _configFileName = argument;

            // Определить, какой тип визуализации был выбран
            Visualization gameName = _mainWindow.GetSelectedGameName();
            string gamePath = string.Empty;

            switch(gameName)
            {
                case Visualization.Penguin:
                    gamePath = Path.GetFullPath(@".\Data\games\ModulPack\main.exe");
                    break;
            }

            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = @".\Data\utils\Brainstart2.exe";
            startInfo.Arguments = argument + " " + gamePath;
            startInfo.UseShellExecute = false;
            startInfo.CreateNoWindow = true;
            var process = Process.Start(startInfo);
            if (null != process)
            {
                process.WaitForExit();
            }

            return false;
            
        }
    }
}
