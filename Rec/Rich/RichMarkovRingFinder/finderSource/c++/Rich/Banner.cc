
#include "Banner.h"
#include "Utils/MessageHandler.h"

static Lester::Banner mandatory;

namespace Lester
{
  Banner::Banner()
  {
    messHandle().info("+---------------------------------------------------------------+");
    messHandle().info("| Using the 'Trackless Ring Finding Library'. Please cite:      |");
    messHandle().info("|   'Trackless ring identification and pattern recognition in   |");
    messHandle().info("|    Ring Imaging Cherenkov (RICH) detectors.'  (C.G.Lester)    |");
    messHandle().info("|    Nucl. Instr. & Meth. A560/2 (2006) pp. 621-632.            |");
    messHandle().info("| Homepage: http://www.hep.phy.cam.ac.uk/~lester/ringfinder     |");
    messHandle().info("+---------------------------------------------------------------+");
  }
}
