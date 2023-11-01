using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace AlphaTraining
{
    internal class PipelineItem
    {
        private string _name;

        private bool _completed;

        public string Name { get { return _name; } }

        public bool Completed { get { return _completed; } }

        public string Icon
        {
            get
            {
                if(Completed)
                {
                    return @"c:\Users\elena\Documents\HSE\Neurofeedback\brainstart2\brainstart2\Brainsart\Brainstart2\AlphaTraining\bin\Debug\net6.0-windows\Data\icons\done.png";
                }
                else
                {
                    return "";
                }
            }
        }

        public PipelineItem(string name)
        {
            _name = name;
            _completed = false;
        }

        public virtual string GetArguments()
        {
            return String.Empty;
        }

        public virtual bool Run(string argument)
        {
            _completed = true;

            return false;
        }
    }
}
