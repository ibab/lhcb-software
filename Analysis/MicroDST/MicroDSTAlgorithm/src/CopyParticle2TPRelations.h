// $Id: CopyParticle2TPRelations.h,v 1.4 2010-08-11 13:50:57 jpalac Exp $
#ifndef COPYPARTICLE2TPRELATIONS_H 
#define COPYPARTICLE2TPRELATIONS_H 1

// Include files
// from LHCb
#include <Relations/Relation2D.h>

// from MicroDST
#include <MicroDST/RelationsClonerAlg.h>
#include <MicroDST/ICloneSwimmingReport.h>
#include <MicroDST/BindType2ClonerDef.h>

// from DaVinci
#include "Event/Particle.h"
#include "Event/SwimmingReport.h"

/** @class CopyParticle2TPRelations CopyParticle2TPRelations.h
 *  
 *  MicroDSTAlgorithm to clone the associations between an LHCb::Particle
 *  and LHCb::SwimmingReport. 
 *  It inherits the std::string properties InputLocation and OutputPrefix from
 *  MicroDSTCommon. The associations are taken from the TES location 
 *  defined by InputLocation, and are cloned and put in TES location 
 *  "/Event" + OutputPrefix + InputLocation. If InputLocation already contains
 *  a leading "/Event" it is removed.
 *  The associations are typically in a LHCb::Relation in a TES 
 *  location like "/Event/Phys/SomeParticleMakingAlg/P2TPRelations".
 *  The algorithm picks up this relations table, which relates LHCb::Particles
 *  from "/Event/Phys/SomeParticleMakingAlg/Particles" to LHCb::SwimmingReport.
 *  The algorithm constructs a new Relation2D table relating the clones and 
 *  stores it in the TES location described above.
 *
 *  <b>Important conditions</b>: This algorithm relies on the LHCb::Particle
 *  set in the associations to have been previously cloned by CopyParticles,
 *  and the existence of the Relation2D table associating the origin
 *  LHCb::Particles to LHCb::SwimmingReport.
 *
 *  @author Roel Aaij roel.aaij@cern.ch
 *  @date   2011-10-09
 */

namespace {
   typedef LHCb::Relation2D<LHCb::Particle, LHCb::SwimmingReport> P2TPTable;
}
//=============================================================================
template <> struct BindType2Cloner<P2TPTable> 
{
  typedef LHCb::SwimmingReport ToType;
  typedef ICloneSwimmingReport ToCloner;
};
//=============================================================================
template<> struct Defaults<P2TPTable>
{
  const static std::string clonerType;
};
const std::string Defaults<P2TPTable>::clonerType = "SwimmingReportCloner";
//=============================================================================
template<> struct Location<P2TPTable>
{
  const static std::string Default;
};
const std::string Location<P2TPTable>::Default = "NO DEFAULT LOCATION";
//=============================================================================
typedef MicroDST::RelationsClonerAlg<P2TPTable> CopyParticle2TPRelations;
DECLARE_NAMED_ALGORITHM_FACTORY( CopyParticle2TPRelations, CopyParticle2TPRelations )
#endif // COPYPARTICLE2TPRELATIONS_H
