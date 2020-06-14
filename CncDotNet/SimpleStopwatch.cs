using System;
using JetBrains.Annotations;

namespace CncDotNet
{
    internal sealed class SimpleStopWatch
    {
        private int _prevTickCount;

        [UsedImplicitly]
        public TimeSpan Elapsed => TimeSpan.FromMilliseconds(ElapsedMilliseconds);

        [UsedImplicitly]
        public int ElapsedMilliseconds
        {
            get
            {
                int prevTickCount = _prevTickCount, currentTickCount = Environment.TickCount;

                if (currentTickCount < prevTickCount)
                {
                    Lap();
                    return 0;
                }

                return currentTickCount - prevTickCount;
            }
        }

        public SimpleStopWatch()
        {
            Lap();
        }

        [UsedImplicitly]
        public void Lap()
        {
            _prevTickCount = Environment.TickCount;
        }
    }

    /// <summary>
    /// Call to Lap() after initialization!
    /// </summary>
    internal struct SimpleStopWatchFast
    {
        private int _prevTickCount;

        [UsedImplicitly]
        public TimeSpan Elapsed => TimeSpan.FromMilliseconds(ElapsedMilliseconds);

        [UsedImplicitly]
        public int ElapsedMilliseconds
        {
            get
            {
                int prevTickCount = _prevTickCount, currentTickCount = Environment.TickCount;

                if (currentTickCount < prevTickCount)
                {
                    Lap();
                    return 0;
                }

                return currentTickCount - prevTickCount;
            }
        }
        
        [UsedImplicitly]
        public void Lap()
        {
            _prevTickCount = Environment.TickCount;
        }
    }
}