using System;
using System.Diagnostics;
using System.IO;
using System.Windows;

namespace AlphaTraining
{
    class FilterConfigurator : PipelineItem
    {
        string resultFileName;

        public FilterConfigurator(MainWindow mainWindow, string name) 
            : base(mainWindow, name)
        {
        }

        public override bool CanMoveForward()
        {
            if(-1 == _mainWindow.GetSelectedPlot())
            {
                MessageBox.Show("Выберите один из каналов для дальнейшей конфигурации!");
                return false;
            }

            return true;
        }

        public override string GetArguments()
        {
            return resultFileName;
        }

        public override void Prepare(string argument)
        {
           

        }

        public override bool Run(string argument)
        {
            

            // В качестве аргумента должно прийти имя файла с baseline

            resultFileName = Path.GetFullPath(
                String.Format(@"./Data/users/{0}/{1}/config_{2}.txt",
                _mainWindow.GetUserName(),
                _mainWindow.GetSessionDate(),
                DateTime.Now.ToString("dd.MM.yyyy_H.mm")));


            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = SystemVariables.Instance.PythonPath;
            startInfo.FileName = "C:/Program Files/Spyder/Python/python.exe";

            startInfo.Arguments = @"./Data/scripts/FitKalmanFilter.py "
            + argument + " "
            + _mainWindow.GetSelectedPlot().ToString() +" "+
            Convert.ToInt16(_mainWindow.cbPrefilter.IsChecked).ToString() + " "+ _mainWindow.GetTemporalFilterType().ToString()+" "
            + _mainWindow.DelayNoiseRatio.Value.ToString() + " "+resultFileName;

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
