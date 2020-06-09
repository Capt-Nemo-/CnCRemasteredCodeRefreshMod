using System.Windows.Forms;
using JetBrains.Annotations;

namespace CncDotNet
{
    [PublicAPI]
    public abstract class MissionScriptBase
    {
        public virtual void OnKeyInputPreview(Keys key)
        {
        }

        public virtual void OnKeyInput(Keys key)
        {
        }
    }
}