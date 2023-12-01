using System.IO;

namespace AlphaTraining
{
    class PlotPost
    {
        private string _imagePath;

        public PlotPost()
        {
            _imagePath = Path.GetFullPath(string.Format(".\\Data\\temp\\Post_analysis_0.png"));


        }

        public string ImagePath
        {
            get
            {
                
                    return _imagePath;
               
                
            }
        }

    }
}

