using System;
using System.Diagnostics;
using System.IO;

namespace AlphaTraining
{
    class FilterConfigurator : PipelineItem
    {
        string resultFileName;

        public FilterConfigurator(MainWindow mainWindow, string name) 
            : base(mainWindow, name)
        {
        }

        public override string GetArguments()
        {
            return resultFileName;
        }

        public override bool Run(string argument)
        {
            // В качестве аргумента должно прийти имя файла с baseline

            resultFileName = Path.GetFullPath(
                String.Format(@"./Data/users/{0}/config_{1}.txt", _mainWindow.GetUserName(), DateTime.Now.ToString("dd.MM.yyyy_H.mm")));

            // TODO: передать имя файла с записью baseline

            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = SystemVariables.Instance.PythonPath;
            startInfo.Arguments = @"./Data/scripts/main_EEG_analysis.py " + argument + " " + resultFileName;
            startInfo.UseShellExecute = false;
            startInfo.CreateNoWindow = true;
            var process = Process.Start(startInfo);
            if(null != process)
            {
                process.WaitForExit();
            }

            return true;
        }
    }
}
