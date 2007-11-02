// $Id: CopyRelatedMCParticles.h,v 1.1 2007-11-02 16:28:24 jpalac Exp $
#ifndef COPYRELATEDMCPARTICLES_H 
#define COPYRELATEDMCPARTICLES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MicroDST/IDoSomethingToParticle.h"            // Interface


/** @class CopyRelatedMCParticles CopyRelatedMCParticles.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-02
 */
class CopyRelatedMCParticles : public GaudiTool, 
                               virtual public IDoSomethingToParticle {
public: 
  /// Standard constructor
  CopyRelatedMCParticles( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~CopyRelatedMCParticles( ); ///< Destructor

  virtual StatusCode operator()  (LHCb::Particle* particle);

  virtual StatusCode doSomething (LHCb::Particle* particle);

protected:

private:

};
#endif // COPYRELATEDMCPARTICLES_H
