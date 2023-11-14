using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AlphaTraining
{
    internal class SystemVariables
    {
        private static SystemVariables _insatnce = new SystemVariables();

        private string _pythonPath = string.Empty;

        private SystemVariables()
        {
            GetPythonPath();
        }

        //get python path from environtment variable
        private void GetPythonPath()
        {
            IDictionary environmentVariables = Environment.GetEnvironmentVariables();
            string pathVariable = environmentVariables["Path"] as string;
            if (pathVariable != null)
            {
                string[] allPaths = pathVariable.Split(';');
                foreach (var path in allPaths)
                {
                    string pythonPathFromEnv = path + "\\python.exe";
                    if (File.Exists(pythonPathFromEnv))
                    {
                        _pythonPath = pythonPathFromEnv;
                        break;
                    }
                }
            }
        }

        public static SystemVariables Instance
        {
            get
            {
                return _insatnce;
            }
        }

        public string PythonPath
        {
            get
            {
                return _pythonPath;
            }        
        }

    }
}
