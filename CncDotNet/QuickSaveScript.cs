using System.Windows.Forms;

namespace CncDotNet
{
    internal class QuickSaveScript : MissionScriptBase
    {
        private const string Filename = "QUICKSAVE_TD", Desc = "Quick save slot", Text = "Quick save successful...";

        public override void OnKeyInput(Keys key)
        {
            switch (key)
            {
                case Keys.F5:
                    CnC.Native.SaveGame(Filename, Desc);
                    CnC.Native.ShowQuickMessage(Text, CnCTextColor.Grey, 900);
                    break;
                case Keys.F9:
                    try
                    {
                        CnC.Native.LoadGame(Filename);
                    }
                    catch
                    {
                        //slot prolly doesn't exist
                    }
                    break;
            }
        }
    }
}