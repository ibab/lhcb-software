// $Id: CopyParticle2BackgroundCategory.h,v 1.2 2010-02-24 17:02:09 jpalac Exp $
#ifndef COPYPARTICLE2BACKGROUNDCATEGORY_H 
#define COPYPARTICLE2BACKGROUNDCATEGORY_H 1

// Include files
// from LHCb
#include <Relations/Relation1D.h>
// from MicroDST
#include <MicroDST/ICloneParticle.h>
#include "MicroDST/RelationsFromParticleLocationsFromClonerAlg.h"
#include "MicroDST/BindType2ClonerDef.h"
// from DaVinci
#include "Event/Particle.h"
/** @class CopyParticle2BackgroundCategory CopyParticle2BackgroundCategory.h
 *  
 *  MicroDSTAlgorithm to clone the associations between an LHCb::Particle
 *  and a BackgroundCategory. 
 *  It inherits the std::string properties InputLocation and OutputPrefix from
 *  MicroDSTCommon. The associations are taken from the TES location 
 *  defined by InputLocation, and are cloned and put in TES location 
 *  "/Event" + OutputPrefix + InputLocation. If InputLocation already contains
 *  a leading "/Event" it is removed.
 *
 *  <b>Important conditions</b>: This algorithm relies on the LHCb::Particle
 *  set in the associations to have been previously cloned by CopyParticles,
 *  and the existence of the LHCb::Relation1D<const LHCb::Particle*,int> associating the origin
 *  LHCb::Particles to BackgroundCategories (ints)
 *
 *  <b>Example</b>: Clone LHCb::Particle -> BackgroundCategory associations
 *  from "/Event/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2BCRelations"
 *  to "/Event/MyLocation/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particle2VertexRelations"
 *  @code
 *  mySeq = GaudiSequencer('SeqDC06selBd2Jpsi2MuMu_Kst2KPi')
 *  copyP2BC = CopyParticle2BackgroundCategory()
 *  mySeq.Members += [copyP2BC]
 *  copyP2BC.InputLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2BCRelations";
 *  copyP2BC.OutputPrefix = "MyLocation";
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2010-02-24
 */
//=============================================================================
namespace Particle2BackCat 
{  
  typedef LHCb::Relation1D<LHCb::Particle, int> Table;
}
//=============================================================================
template<> struct Defaults<Particle2BackCat::Table>
{
  const static std::string relationsName;
};
const std::string Defaults<Particle2BackCat::Table>::relationsName = "/P2BCRelations";
//=============================================================================
template<> struct Location<Particle2BackCat::Table>
{
  const static std::string Default;
};
const std::string Location<Particle2BackCat::Table>::Default = "NO DEFAULT LOCATION";
//=============================================================================
typedef MicroDST::RelationsFromParticleLocationsFromClonerAlg<Particle2BackCat::Table> CopyParticle2BackgroundCategory;
DECLARE_NAMED_ALGORITHM_FACTORY( CopyParticle2BackgroundCategory, CopyParticle2BackgroundCategory )
#endif // COPYPARTICLE2BACKGROUNDCATEGORY_H
