using System.IO;
using System.Windows.Forms;

namespace CncDotNet
{
    internal class QuickSaveScript : MissionScriptBase
    {
        public override void OnKeyInput(Keys key)
        {
            switch (key)
            {
                case Keys.F5:
                    Cnc.Native.SaveGame(GetFilename(), "Quick save slot");
                    Cnc.Native.ShowQuickMessage("Quick save successful...", 1000);
                    break;
                case Keys.F9:
                    try
                    {
                        Cnc.Native.LoadGame(GetFilename());
                        Cnc.Native.ShowQuickMessage("Quick load successful...", 1000);
                    }
                    catch (FileNotFoundException)
                    {
                        Cnc.Native.ShowQuickMessage("Error: save file not found...", 2000);
                    }
                    catch
                    {
                        Cnc.Native.ShowQuickMessage("Error: quick load failed...", 2000);
                    }

                    break;
            }

            string GetFilename() => $"QUICKSAVE_TD_{Cnc.Native.CurrentScenarioName}";
        }
    }
}