using System;
using System.IO;

namespace AlphaTraining
{
    internal class PipelineItem
    {
        private string _name;

        private bool _completed;

        public string Name { get { return _name; } }

        public bool Completed { get { return _completed; } }

        protected MainWindow _mainWindow;

        public string Icon
        {
            get
            {
                if(Completed)
                {
                    return Path.GetFullPath(@".\Data\icons\done.png");
                }
                else
                {
                    return "";
                }
            }
        }

        public PipelineItem(MainWindow mainWindow, string name)
        {
            _mainWindow = mainWindow;
            _name = name;
            _completed = false;
        }

        public virtual bool CanMoveForward()
        {
            return true;
        }

        public virtual string GetArguments()
        {
            return String.Empty;
        }

        public virtual void Prepare(string argument)
        {

        }

        public virtual bool Run(string argument)
        {
            _completed = true;

            return false;
        }
    }
}
