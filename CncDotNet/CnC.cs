using System;
using System.Windows.Forms;
using JetBrains.Annotations;

//ClientG.exe GAME_INDEX=0 TIBERIANDAWN

namespace CncDotNet
{
    public class CnC
    {
        private static bool Initialized { get; set; }

        private static INativeApiRoot Native { get; set; }
        
        [PublicAPI]
        public static CnC MainInit(string[] cmdLine, INativeApiRoot root)
        {
            if (Initialized)
                throw new Exception("Api already initialized.");

            Initialized = true;

            Native = root;
            return new CnC();
        }

        internal CnC()
        {
        }

        [PublicAPI]
        public void ProcessKeyInput(int kn, int vk, bool preview)
        {
            var wfKey = (Keys) vk;
            
        }
    }
}