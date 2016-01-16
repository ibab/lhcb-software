// $Id$ 
// ============================================================================
#ifndef LOKI_LOKICORE_DCT_H 
#define LOKI_LOKICORE_DCT_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/HistoDef.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functors.h"
#include "LoKi/Services.h"
#include "LoKi/Dicts.h"
#include "LoKi/Print.h"
#include "LoKi/HistoBook.h"
// ============================================================================
#include "LoKi/ICoreFactory.h"
#include "LoKi/ICoreAntiFactory.h"
#include "LoKi/CoreEngine.h"
#include "LoKi/iTree.h"
#include "LoKi/IDecayNode.h"
#include "LoKi/Listener.h"
#include "LoKi/Dump.h"
#include "LoKi/DecayChainBase.h"
#include "LoKi/Tensors.h"
#include "LoKi/Param.h"
#include "LoKi/Monitor.h"
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
#include "LoKi/KeeperBase.h"
#include "LoKi/Welcome.h"
#include "LoKi/Report.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/Kinematics.h"
#include "LoKi/Geometry.h"
#include "LoKi/Constants.h"
#include "LoKi/Assert.h"
#include "LoKi/Print.h"
#include "LoKi/Tensors.h"
#include "LoKi/Trees.h"
#include "LoKi/Base.h"
#include "LoKi/Monitoring.h"
#include "LoKi/PidFunctions.h"
#include "LoKi/TES.h"
#include "LoKi/RecStat.h"
#include "LoKi/MissingParticle.h"
// ============================================================================
namespace
{
  // ==========================================================================
  struct _Interfaces
  {
    // ========================================================================
    LoKi::Interface<LoKi::ILoKiSvc>   m_1 ;
    LoKi::Interface<LoKi::IReporter>  m_2 ;
    LoKi::Interface<GaudiTool>        m_3 ;
    LoKi::Interface<GaudiAlgorithm>   m_4 ;
    LoKi::Interface<IDataProviderSvc> m_5 ;
    // fictive constructor
    _Interfaces() ;
    // ========================================================================
  } ;
  // ==========================================================================
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOKICORE_DCT_H
// ============================================================================
