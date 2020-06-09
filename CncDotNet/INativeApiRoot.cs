namespace CncDotNet
{
    public interface INativeApiRoot
    {
        void SaveGame(string filename, string description);
        void LoadGame(string filename);
        void ShowQuickMessage(string text, CnCTextColor color, int timeoutMs);
    }
}