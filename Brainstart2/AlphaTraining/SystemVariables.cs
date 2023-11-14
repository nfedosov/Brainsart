using System;
using System.Collections;
using System.IO;

namespace AlphaTraining
{
    internal class SystemVariables
    {
        private static SystemVariables _instance = new SystemVariables();

        private string _pythonPath = String.Empty;

        public static  SystemVariables Instance
        { get { return _instance; } }

        public string PythonPath
        { get { return _pythonPath; } }

        private SystemVariables() 
        {
            GetPythonPath();
        }

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
                        _pythonPath = pythonPathFromEnv;
                }
            }
        }

    }
}
