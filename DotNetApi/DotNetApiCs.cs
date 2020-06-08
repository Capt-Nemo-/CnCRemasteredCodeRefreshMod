using System;
using System.Windows.Forms;
using JetBrains.Annotations;

//ClientG.exe GAME_INDEX=0 TIBERIANDAWN

namespace DotNetApi
{
    [PublicAPI]
    public static class DotNetApiCs
    {
        public static Func<string, string> SaveGameHandler { private get; set; }

        public static void MainInit(string[] cmdLine)
        {
        }

        public static void OnKeyInput(int kn, int vk, bool preview)
        {
            var wfKey = (Keys) vk;

        }
    }
}