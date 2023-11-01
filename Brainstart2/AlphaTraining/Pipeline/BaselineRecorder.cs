using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AlphaTraining
{
    internal class BaselineRecorder : PipelineItem
    {
        string _scenarioFilename;

        string _recordingFilename;

        public BaselineRecorder(string name) 
            : base(name) 
        { 
        
        }

        public override string GetArguments()
        {
            return @"c:\Users\elena\Documents\HSE\Neurofeedback\brainstart2\brainstart2\Brainsart\Brainstart2\AlphaTraining\bin\Debug\net6.0-windows\Data\users\1\baseline_experiment_10-18_21-32-39\data.pickle"
                + @" c:\Users\elena\Documents\HSE\Neurofeedback\brainstart2\brainstart2\Brainsart\Brainstart2\AlphaTraining\bin\Debug\net6.0-windows\Data\users\1\config.txt";
        }

        public override bool Run(string argument)
        {
            base.Run(argument);
            return false;
        }
    }
}
