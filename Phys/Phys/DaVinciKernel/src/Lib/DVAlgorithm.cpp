
#include "Kernel/DVAlgorithm.h"

StatusCode DVAlgorithm::initialize ()
{
  const StatusCode sc = DaVinciTupleAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;
  warning() << "'" << System::typeinfoName(typeid(*this)) << "'"
            << " is using the depreciated DVAlgorithm base class. "
            << "Please move to DaVinciAlgorithm, DaVinciHistoAlgorithm or DaVinciTupleAlgorithm."
            << endmsg;
  return sc;
}
