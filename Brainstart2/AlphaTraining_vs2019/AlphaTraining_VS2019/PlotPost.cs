using System.IO;

namespace AlphaTraining
{
    class PlotPost
    {
        private string _imagePath;
        private string _image2Path;

        public PlotPost()
        {
            _imagePath = Path.GetFullPath(string.Format(".\\Data\\temp\\Post_analysis_0.png"));
            _image2Path = Path.GetFullPath(string.Format(".\\Data\\temp\\Post_analysis_1.png"));


        }

        public string ImagePath
        {
            get
            {
                
                    return _imagePath;
               
                
            }
        }

        public string Image2Path
        {
            get
            {

                return _image2Path;


            }
        }

    }
}

