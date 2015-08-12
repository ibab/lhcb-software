// $Id$
/** @class P2PVWithIPChi2 P2PVWithIPChi2.cpp
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-10-16
 */
// Include files 

// Local
#include "GenericParticle2PVRelator.h"
#include "DistanceCalculatorNames.h"
#include "P2PVLogic.h"

/** @class P2PVWithIP P2PVWithIP.cpp
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-10-16
 */
typedef GenericParticle2PVRelator<_p2PVWithIPChi2, OfflineDistanceCalculatorName>  P2PVWithIPChi2;
// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( P2PVWithIPChi2 )
