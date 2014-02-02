// $Id$
/** @class P2PVWithIPChi2 P2PVWithIPChi2.cpp
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-10-16
 */
// Include files 
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

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

#ifdef DECLARE_COMPONENT_WITH_ID
// backward compatibility with the old PluginService internal names
DECLARE_COMPONENT_WITH_ID(P2PVWithIPChi2, "GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_")
#endif
