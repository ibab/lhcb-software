// $Id$
/** @class OnlineP2PVWithIPChi2 OnlineP2PVWithIPChi2.cpp
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-10-20
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
typedef GenericParticle2PVRelator<_p2PVWithIPChi2, OnlineDistanceCalculatorName>  OnlineP2PVWithIPChi2;
// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OnlineP2PVWithIPChi2 )
