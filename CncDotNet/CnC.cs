using System;
using System.Windows.Forms;
using JetBrains.Annotations;

//ClientG.exe GAME_INDEX=0 TIBERIANDAWN

namespace CncDotNet
{
    public class CnC
    {
        private static bool Initialized { get; set; }

        public static INativeApiRoot Native { get; private set; }

        private static MissionScriptBase[] MissionScripts { get; set; }
        
        [PublicAPI]
        public static CnC MainInit(string[] cmdLine, INativeApiRoot root)
        {
            if (Initialized)
                throw new Exception("Api already initialized.");

            Initialized = true;

            Native = root;

            MissionScripts = new MissionScriptBase[] {new QuickSaveScript()};

            return new CnC();
        }

        internal CnC()
        {
        }

        [PublicAPI]
        public void ProcessKeyInput(int kn, int vk, bool preview)
        {
            var wfKey = (Keys) vk;

            foreach (MissionScriptBase script in MissionScripts)
            {
                if (preview)
                    script.OnKeyInputPreview(wfKey);
                else
                    script.OnKeyInput(wfKey);
            }
        }
    }
}