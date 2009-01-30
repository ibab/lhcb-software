#ifndef DAVINCISMARTASSOCIATOR_H 
#define DAVINCISMARTASSOCIATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IBackgroundCategory.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h"
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "Kernel/Particle2MCAssociatorBase.h"
#include "Kernel/MCAssociation.h"
/** @class DaVinciSmartAssociator DaVinciSmartAssociator.h
 *  
 *  A ``smart'' associator for any kind of particle. Returns a vector of
 *  MCAssociation classes, each of which represents a single associated MCParticle
 *  and its association weight. The association methods used are
 *
 *  Charged stables: Protoparticle associator
 *  Neutral stables: Neutral Protoparticle associator
 *  Composite: BackgroundCategory
 *
 *  For composites, there is always at most one associated MCParticle, whose
 *  association weight is set to 1 by definition
 *  
 *  @author V. Gligorov
 *  @date   2009-01-13
 */
class DaVinciSmartAssociator : public Particle2MCAssociatorBase, 
                               virtual public IParticle2MCAssociator {
public: 
  /// Standard constructor
  DaVinciSmartAssociator( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~DaVinciSmartAssociator( ); ///< Destructor

  virtual StatusCode initialize() ;

  virtual StatusCode finalize() ;

  virtual Particle2MCParticle::ToVector 
  associate(const LHCb::Particle* particle,
            const std::string& mcParticleLocation) const;


private:

  IDaVinciAssociatorsWrapper* m_linkerTool_cPP;
  IDaVinciAssociatorsWrapper* m_linkerTool_nPP;
  IBackgroundCategory* m_bkg; //for composites

};
#endif // DAVINCISMARTASSOCIATOR_H
