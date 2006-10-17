// $Id: PhysDict.h,v 1.2 2006-10-17 17:40:24 jpalac Exp $
#ifndef DICT_PHYSDICT_H 
#define DICT_PHYSDICT_H 1

// Include files

/** @file PhysDict PhysDict.h dict/PhysDict.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2006-10-02
 */
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IAlgorithm2ID.h"
#include "Kernel/IBestParticles.h"
#include "Kernel/IBTaggingTool.h"
#include "Kernel/ICaloParticleMaker.h"
#include "Kernel/ICheckOverlap.h"
#include "Kernel/ICheckSelResults.h"
#include "Kernel/IContextTool.h"
#include "Kernel/IDecayFinder.h"
#include "Kernel/IDecodeSimpleDecayString.h"
#include "Kernel/IDirectionFit.h"
#include "Kernel/IFilterCriteriaTESTool.h"
#include "Kernel/IFilterCriterion.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IJetMaker.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IMassVertexFit.h"
#include "Kernel/IOnOffline.h"
#include "Kernel/IParticle2State.h"
#include "Kernel/IParticle2VertexAsct.h"
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IParticleDescendants.h"
#include "Kernel/IParticleFilter.h"
#include "Kernel/IParticleMaker.h"
#include "Kernel/IParticleReFitter.h"
#include "Kernel/IParticleStuffer.h"
#include "Kernel/IPhysDesktop.h"
#include "Kernel/IPlotTool.h"
#include "Kernel/IProtoParticleFilter.h"
#include "Kernel/IPVReFitter.h"
#include "Kernel/IRelatedPV.h"
//#include "Kernel/ISecondaryVertexTool.h"
//#include "Kernel/ITagger.h"
#include "Kernel/IVertexFit.h"
// ============================================================================
// DaVinciMCKernel
// ============================================================================
#include "Kernel/IBackgroundCategory.h"
#include "Kernel/ICheatedLifetimeFitter.h"
#include "Kernel/IDebugTool.h"
#include "Kernel/IMCReconstructible.h"
#include "Kernel/IVisPrimVertTool.h"
// ============================================================================
// MCEvent
// ============================================================================
#include "Kernel/IMCDecayFinder.h"
// ============================================================================
namespace {

/// @todo remove ostream operator when included in new Gaudi
  std::ostream& operator<< ( std::ostream& s , const StatusCode& sc ) 
  {
    if ( sc.isSuccess() ) { return s << "SUCCESS" ; }
    s << "FAILURE" ;
    if ( StatusCode::FAILURE != sc.getCode() ) 
    { s << "(" << sc.getCode() << ")" ;}
    return s ;
  }

}

#endif // DICT_PHYSDICT_H
