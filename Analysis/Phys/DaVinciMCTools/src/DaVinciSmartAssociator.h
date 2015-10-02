#ifndef DAVINCISMARTASSOCIATOR_H 
#define DAVINCISMARTASSOCIATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IBackgroundCategory.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h"
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/Particle2MCAssociatorBase.h"
#include "Kernel/IParticle2MCWeightedAssociator.h"
#include "Kernel/MCAssociation.h"
#include "Event/MCParticle.h"
#include "CaloInterfaces/ICalo2MCTool.h"
/** @class DaVinciSmartAssociator DaVinciSmartAssociator.h
 *
 *  A ``smart'' associator for any kind of particle. Returns a vector of
 *  MCAssociation objects, each of which represents a single associated MCParticle
 *  and its association weight. The association methods used are
 *
 *  Charged stables: Protoparticle associator
 *  Neutral stables: Neutral Protoparticle associator
 *  Composite: BackgroundCategory
 *
 *  For composites, there is always at most one associated MCParticle, whose
 *  association weight is set to 1 by definition
 *
 *  Implements IParticle2MCAssociator, but does not satisfy its rules, since the
 *  retrieval of associations of stable particles interacts with the linker
 *  mechanism on the TES, which can populate the TES with linker objects
 *  when these don't already exist, and thus has direct side-effects. It also has the
 *  added constraint that the LHCb::MCParticles to be associated
 *  HAVE to be on the TES always, and the LHCb::Particles have to be on the TES if they are
 *  stable. However, the results returned to the user are self-contained and independent
 *  of the TES.
 *
 *  @author V. Gligorov
 *  @date   2009-01-13
 */
class DaVinciSmartAssociator : public Particle2MCAssociatorBase
{

public:

  /// Standard constructor
  DaVinciSmartAssociator( const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual ~DaVinciSmartAssociator( ); ///< Destructor

  virtual StatusCode initialize();

private :

  virtual Particle2MCParticle::ToVector
  relatedMCPsImpl(const LHCb::Particle* particle,
                  const LHCb::MCParticle::ConstVector& mcParticles) const ;

private:

  IParticle2MCWeightedAssociator* m_weightedAssociation; //for stables
  IBackgroundCategory* m_bkg; //for composites
  ICalo2MCTool* m_calo2MC; // for neutral calorimetric
  double m_caloWeight;
  bool m_redoNeutral;
};

#endif // DAVINCISMARTASSOCIATOR_H
