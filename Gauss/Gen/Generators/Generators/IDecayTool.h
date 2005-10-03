// $Id: IDecayTool.h,v 1.1 2005-10-03 09:48:00 robbep Exp $
#ifndef GENERATORS_IDECAYTOOL_H 
#define GENERATORS_IDECAYTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace HepMC {
  class GenEvent ;
  class GenParticle ;
}

/** @class IDecayTool IDecayTool.h Algorithms/IDecayTool.h
 *  
 *  Tool to contain EvtGen (Interface)
 *  @author Patrick Robbe
 *  @date   2003-10-15
 */

static const InterfaceID IID_IDecayTool("IDecayTool",1,0) ;


class IDecayTool : virtual public IAlgTool {
public:
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IDecayTool ; }
  
  virtual StatusCode generateDecay( HepMC::GenEvent * theEvent ,
                                    HepMC::GenParticle * theMother ) const 
    = 0 ;

  virtual StatusCode generateSignalDecay( HepMC::GenEvent * theEvent ,
                                          HepMC::GenParticle * theMother ,
                                          bool & flip ) const = 0 ;

  virtual StatusCode generateDecayWithLimit( HepMC::GenEvent * theEvent ,
                                             HepMC::GenParticle * theMother ,
                                             int targetId ) const = 0 ;

  virtual void enableFlip() const = 0 ;
  virtual void disableFlip() const = 0 ;

  virtual bool isKnownToDecayTool( int ) const = 0 ;

  virtual double getSignalBr( ) const = 0 ;
  
  virtual bool checkSignalPresence( ) const = 0 ;

  virtual void setSignal( const int ) = 0 ;
};
#endif // GENERATORS_IDECAYTOOL_H
