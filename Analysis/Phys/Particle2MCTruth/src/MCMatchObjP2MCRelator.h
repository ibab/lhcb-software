// $Id: MCMatchObjP2MCRelator.h,v 1.1.1.1 2009-03-04 14:26:10 jpalac Exp $
#ifndef MCMATCHOBJP2MCRELATOR_H 
#define MCMATCHOBJP2MCRELATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticle2MCAssociator.h"            // Interface
#include "Kernel/Particle2MCAssociatorBase.h"

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

};
#endif // MCMATCHOBJP2MCPRELATOR_H
