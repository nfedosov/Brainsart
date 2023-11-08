using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Enumeration;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
                String.Format(@"./Data/users/{0}/config_{1}.txt", _mainWindow.GetUserName(), DateTime.Now.Ticks));

            // TODO: передать имя файла с записью baseline

            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = @"c:\Users\elena\anaconda3\Python.exe";
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
