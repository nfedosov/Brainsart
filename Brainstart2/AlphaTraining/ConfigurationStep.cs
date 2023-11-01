using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AlphaTraining
{
    internal class ConfigurationStep
    {
        private string _name;

        private bool _completed;

        public string Name { get { return _name; } }    

        public bool Completed { get { return _completed; } }

        public ConfigurationStep(string name, bool completed = false)
        {
            _name = name;
            _completed = completed;
        }
    }
}
