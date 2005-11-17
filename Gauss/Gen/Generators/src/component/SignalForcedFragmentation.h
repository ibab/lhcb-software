// $Id: SignalForcedFragmentation.h,v 1.1 2005-11-17 15:57:56 robbep Exp $
#ifndef GENERATORS_SIGNALFORCEDFRAGMENTATION_H 
#define GENERATORS_SIGNALFORCEDFRAGMENTATION_H 1

// Include files
#include "Generators/Signal.h" 

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

/** @class SignalForcedFragmentation SignalForcedFragmentation.h 
 *  
 *  Tool for signal with forced fragmentation method samples generation
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class SignalForcedFragmentation : public Signal {
public:
  /// Standard constructor
  SignalForcedFragmentation( const std::string& type, 
                             const std::string& name,
                             const IInterface* parent);
  
  virtual ~SignalForcedFragmentation( ); ///< Destructor

  virtual StatusCode initialize( ) ; ///< Initialize
  
  virtual bool generate( const unsigned int nPileUp , 
                         EventVector & theEventVector ,
                         HardVector  & theHardVector ) ;
protected:

private:
  StatusCode boostTree( HepMC::GenParticle * theSignal , 
                        const HepMC::GenParticle * theSignalAtRest ,
                        HepMC::GenEvent * theEvent ,
                        const Hep3Vector & theVector ) const ;
};


#endif // GENERATORS_SIGNALFORCEDFRAGMENTATION_H
