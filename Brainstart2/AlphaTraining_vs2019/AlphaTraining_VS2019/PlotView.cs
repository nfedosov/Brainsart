using System.IO;

namespace AlphaTraining
{
    class PlotView
    {
        private string _channelName;

        private bool _isSelected;

        private string _imageTemporalPath;

        private string _imageSpectrumPath;

        private string _imageTopographyPath;

        private bool _isTemporalView;

        public PlotView(int Number)
        {
            _imageTemporalPath = Path.GetFullPath(string.Format(".\\Data\\temp\\Timeseries_{0}.png", Number));

            _imageSpectrumPath = Path.GetFullPath(string.Format(".\\Data\\temp\\Spectrum_{0}.png", Number));

            _imageTopographyPath = Path.GetFullPath(string.Format(".\\Data\\temp\\filter_{0}.png", Number));

            _isTemporalView = true;

            _channelName = (Number + 1).ToString();
        }

        public bool IsSelected
        {
            get
            {
                return _isSelected;
            }

            set
            {
                _isSelected = value;
            }
        }

        public string ImagePath
        {
            get
            {
                if(_isTemporalView)
                {
                    return _imageTemporalPath;
                }
                else
                {
                    return _imageSpectrumPath;
                }
            }
        }

        public string ChannelName
        {
            get
            {
                return _channelName;
            }
        }

        public string TopographyPath
        {
            get
            {
                return _imageTopographyPath;
            }
        }

        public void SwitchView()
        {
            _isTemporalView = !_isTemporalView;
        }

    }
}
