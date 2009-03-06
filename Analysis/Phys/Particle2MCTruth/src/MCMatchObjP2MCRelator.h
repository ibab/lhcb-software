// $Id: MCMatchObjP2MCRelator.h,v 1.3 2009-03-06 14:24:01 jpalac Exp $
#ifndef MCMATCHOBJP2MCRELATOR_H 
#define MCMATCHOBJP2MCRELATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticle2MCAssociator.h"            // Interface
#include "Kernel/Particle2MCAssociatorBase.h"
#include "LoKi/MCMatch.h"
#include "LoKi/MCMatchObj.h"
// forward declarations
namespace LoKi 
{
  class IReporter;
}
/** @class MCMatchObjP2MCRelator MCMatchObjP2MCRelator.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2009-03-04
 */
class MCMatchObjP2MCRelator : public Particle2MCAssociatorBase,
                              virtual public IParticle2MCAssociator {
public: 
  /// Standard constructor
  MCMatchObjP2MCRelator( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~MCMatchObjP2MCRelator( ); ///< Destructor

  virtual StatusCode initialize() ;

  virtual StatusCode finalize() ;

  virtual bool isMatched(const LHCb::Particle* particle, 
                         const LHCb::MCParticle* mcParticle) const ;

private:

  virtual LHCb::MCParticle::ConstVector sort(const LHCb::MCParticle::ConstVector& mcParticles) const;
  
  virtual LHCb::MCParticle::ConstVector sort(const LHCb::MCParticle::Container* mcParticles) const;

  LoKi::MCMatch matcher() const;

private:

  LoKi::IReporter* m_reporter;
  mutable LoKi::MCMatchObj* m_matcher;
  typedef std::vector<std::string> Addresses ;
  Addresses m_PP2MC;

};
#endif // MCMATCHOBJP2MCPRELATOR_H
