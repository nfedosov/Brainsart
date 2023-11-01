using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Enumeration;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AlphaTraining
{
    class FilterConfigurator : PipelineItem
    {
        string resultFileName;

        public FilterConfigurator(string name) : base(name)
        {
        }

        public override string GetArguments()
        {
            return resultFileName;
        }

        public override bool Run(string argument)
        {
            base.Run(argument);

            resultFileName = argument.Split(' ')[1];

            // TODO: передать имя файла с записью baseline

            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = @"c:\Users\elena\anaconda3\Python.exe";
            startInfo.Arguments = @"./Data/scripts/main_EEG_analysis.py " + argument;
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
