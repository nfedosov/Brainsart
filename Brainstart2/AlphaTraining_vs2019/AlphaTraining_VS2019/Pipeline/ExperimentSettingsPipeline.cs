using System;
using System.Diagnostics;
using System.IO;

namespace AlphaTraining.Pipeline
{
    class ExperimentSettingsPipeline : PipelineItem
    {
        string _configFileName;
        string _toSaveFileName;

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


            //Костыль, сделать, чтобы можно было много раз запускать и записывать в новые файлы по номерам
            _toSaveFileName = argument.Replace("config_","probe_").Replace(".txt", ".fif");


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
            startInfo.Arguments = String.Format("-c \"{0}\" -o \"{1}\" -u \"{2}\" -l \"{3}\"",
                _configFileName,
                _toSaveFileName,
                _mainWindow.GetUserName(),
                _mainWindow.GetLslStreamName()
                ) ;

            startInfo.UseShellExecute = false;
            startInfo.CreateNoWindow = true;
            var process = Process.Start(startInfo);
            
            // запустить приложение с визуализацией
            startInfo = new ProcessStartInfo();
            startInfo.FileName = gamePath;
            startInfo.Arguments = String.Empty;
            startInfo.UseShellExecute = false;
            startInfo.CreateNoWindow = true;
            var PenguinProcess = Process.Start(startInfo);
            if (null != process)
            {
                process.WaitForExit();
            }


            return false;
            
        }
    }
}
