// $Id: DaVinciInterfaces.cpp,v 1.11 2010-06-20 15:49:19 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// local
// ============================================================================
#include "Kernel/IParticleReFitter.h"
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IDecodeSimpleDecayString.h"
#include "Kernel/IDirectionFit.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/IMassFit.h"
#include "Kernel/IJetMaker.h"
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/ISetInputParticles.h"
#include "Kernel/IPrintDecay.h"
#include "Kernel/ICheckSelResults.h"
#include "Kernel/ITrackIsolation.h"
#include "Kernel/IExtraInfoTool.h"
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/IParticleFilter.h"
#include "Kernel/IDecayTreeFit.h"
#include "Kernel/ICheckOverlap.h"
#include "Kernel/IParticleIsolation.h"
#include "Kernel/IParticleVeto.h"
#include "Kernel/IBremAdder.h"
// ============================================================================
/** @file  
 *  Implementation file for class some interfaces from 
 *  Phys/DaVinciInterfaces package 
 *  @date 2009-08-15 
 *  @author Vanya  BELYAEV Ivan.Belyaev
 */ 
// ============================================================================
// virtual & protected desctructor 
// ============================================================================
IParticleReFitter        :: ~IParticleReFitter        () {}
IParticleCombiner        :: ~IParticleCombiner        () {}
IDirectionFit            :: ~IDirectionFit            () {}
IVertexFit               :: ~IVertexFit               () {}
IMassFit                 :: ~IMassFit                 () {}
IJetMaker                :: ~IJetMaker                () {}
IDistanceCalculator      :: ~IDistanceCalculator      () {}
ISetInputParticles       :: ~ISetInputParticles       () {}
IPrintDecay              :: ~IPrintDecay              () {}
ICheckSelResults         :: ~ICheckSelResults         () {}
IDecodeSimpleDecayString :: ~IDecodeSimpleDecayString () {}
ITrackIsolation          :: ~ITrackIsolation          () {}
IExtraInfoTool           :: ~IExtraInfoTool           () {}
IDVAlgorithm             :: ~IDVAlgorithm             () {}
IParticleFilter          :: ~IParticleFilter          () {}
IDecayTreeFit            :: ~IDecayTreeFit            () {}
ICheckOverlap            :: ~ICheckOverlap            () {}
IParticleIsolation       :: ~IParticleIsolation       () {}
IParticleVeto            :: ~IParticleVeto            () {}
IBremAdder               :: ~IBremAdder               () {}

// ============================================================================
// The END 
// ============================================================================
