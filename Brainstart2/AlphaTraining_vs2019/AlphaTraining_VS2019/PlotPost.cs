using System.IO;

namespace AlphaTraining
{
    class PlotPost
    {
        private string _imagePath;

        public PlotPost(int imageIndex)
        {
            _imagePath = Path.GetFullPath(string.Format(".\\Data\\temp\\Post_analysis_{0}.png", imageIndex));
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

