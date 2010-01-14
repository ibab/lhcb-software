// $Id: DaVinciInterfaces.cpp,v 1.2 2010-01-14 07:49:27 jpalac Exp $
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
#include "Kernel/IDirectionFit.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/IMassFit.h"
#include "Kernel/IMassVertexFit.h"
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
IMassVertexFit           :: ~IMassVertexFit           () {}
IJetMaker                :: ~IJetMaker                () {}
IDistanceCalculator      :: ~IDistanceCalculator      () {}
ISetInputParticles       :: ~ISetInputParticles       () {}
IPrintDecay              :: ~IPrintDecay              () {}
ICheckSelResults         :: ~ICheckSelResults         () {}

// ============================================================================
// The END 
// ============================================================================

