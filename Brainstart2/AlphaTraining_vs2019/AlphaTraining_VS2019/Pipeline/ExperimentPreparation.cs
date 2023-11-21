using System;
using System.Windows;

namespace AlphaTraining.Pipeline
{
    internal class ExperimentPreparation : PipelineItem
    {
        

        public ExperimentPreparation(MainWindow mainWindow, string name) 
            : base(mainWindow, name)
        {
        }

        public override string GetArguments()
        {
            return _mainWindow.GetSelectedProtocolName();  
        }

        public override bool CanMoveForward()
        {
            if(_mainWindow.GetSelectedProtocolName() == String.Empty)
            {
                MessageBox.Show("Выберите протокол калибровки!");
                return false;
            }
            return true;
        }

        public override void Prepare(string argument)
        {

        }

        public override bool Run(string argument)
        {
            return true;
        }
    }
}
