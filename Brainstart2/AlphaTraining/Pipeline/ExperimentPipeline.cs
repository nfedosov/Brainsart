using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AlphaTraining.Pipeline
{
    class ExperimentPipeline : PipelineItem
    {
        public ExperimentPipeline(string name) : base(name)
        {
        }

        public override string GetArguments()
        {
            return base.GetArguments();
        }

        public override bool Run(string argument)
        {
            base.Run(argument);

            // TODO: передать имя файла с записью baseline

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
