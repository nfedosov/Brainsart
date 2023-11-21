using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AlphaTraining.Pipeline
{
    internal class TerminalStep : PipelineItem
    {
        public TerminalStep(MainWindow mainWindow, string name) : base(mainWindow, name)
        {
        }

        public override bool CanMoveForward()
        {
            return true;
        }

        public override string GetArguments()
        {
            return String.Empty;
        }

        public override void Prepare(string argument)
        {

        }

        public override bool Run(string argument)
        {
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = @".\Data\utils\Brainstart2.exe";
            startInfo.Arguments = argument;
            startInfo.UseShellExecute = false;
            startInfo.CreateNoWindow = true;
            var process = Process.Start(startInfo);
            if (null != process)
            {
                process.WaitForExit();
            }

            return true;
        }
    }
}
