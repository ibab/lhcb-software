// $Id: DaVinciInterfaces.cpp,v 1.6 2010-03-17 08:37:30 giampi Exp $
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

// ============================================================================
/** @file
 *  Implementation file for class doem interfaces from 
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

// ============================================================================
// The END 
// ============================================================================

