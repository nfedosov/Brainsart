﻿using System;

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
            return _mainWindow.GetSelectedProtocolName() != String.Empty;
        }

        public override bool Run(string argument)
        {
            return false;
        }
    }
}
