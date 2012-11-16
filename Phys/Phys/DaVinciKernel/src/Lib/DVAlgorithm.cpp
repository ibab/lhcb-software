
// Only for use here ....
#define DAVINCIKERNEL_DVALGORITHM_NOOBSOLETEWARNING

#include "Kernel/DVAlgorithm.h"

StatusCode DVAlgorithm::initialize ()
{
  const StatusCode sc = DaVinciTupleAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;
  warning() << "======================================================================================"
            << endmsg
            << " TAKE NOTE - '" << System::typeinfoName(typeid(*this)) << "'"
            << " is using the depreciated DVAlgorithm base class."
            << endmsg
            << "Please move to DaVinciAlgorithm, DaVinciHistoAlgorithm or DaVinciTupleAlgorithm."
            << endmsg
            << "Eventually the DVAlgorithm base class will be REMOVED, at which point this class"
            << endmsg
            << " will NO LONGER COMPILE."
            << endmsg
            << "======================================================================================"
            << endmsg;
  return sc;
}
