using System;
namespace AlphaTraining.Pipeline
{
    class ExperimentSettingsPipeline : PipelineItem
    {
        string _configFileName;

        public ExperimentSettingsPipeline(MainWindow mainWindow, string name) 
            : base(mainWindow, name)
        {
        }

        public override string GetArguments()
        {
            return _configFileName;
        }

        public override bool Run(string argument)
        {
            // В качестве агрумента пришло имя с файлом крнфигурации
            _configFileName = argument;

            return false;
            
        }
    }
}
