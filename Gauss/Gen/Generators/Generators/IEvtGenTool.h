// $Id: IEvtGenTool.h,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
#ifndef GENERATORS_IEVTGENTOOL_H 
#define GENERATORS_IEVTGENTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace HepMC {
  class GenEvent ;
  class GenParticle ;
}

class EvtId ;

/** @class IEvtGenTool IEvtGenTool.h Algorithms/IEvtGenTool.h
 *  
 *  Tool to contain EvtGen (Interface)
 *  @author Patrick Robbe
 *  @date   2003-10-15
 */

static const InterfaceID IID_IEvtGenTool("IEvtGenTool",1,0) ;


class IEvtGenTool : virtual public IAlgTool {
public:
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IEvtGenTool ; }
  
  virtual StatusCode generateDecay( HepMC::GenEvent * theEvent ,
                                    HepMC::GenParticle * theMother ) const 
    = 0 ;

  virtual StatusCode generateDecayWithLimit( HepMC::GenEvent * theEvent ,
                                             HepMC::GenParticle * theMother ,
                                             int targetId ) const = 0 ;

  virtual StatusCode generateDecayAtRest( HepMC::GenEvent * theEvent ,
                                          const EvtId & theId ,
                                          int & resultId , 
                                          const HepMC::GenParticle 
                                          * theParticle ) const
    = 0 ;

  virtual int getEventType() const = 0 ;
  virtual const std::vector< int >& getBHadronsList() const = 0 ;

  virtual const EvtId& getbId() const = 0 ;
  virtual const EvtId& getbbarId() const = 0 ;
  virtual const EvtId& getSignalAlias( int pdgId ) const = 0 ;
  
  virtual void enableFlip() const = 0 ;
  virtual void disableFlip() const = 0 ;

  virtual double randomFromEngine() const = 0 ;

  virtual bool isSignalDecay() const = 0;
  virtual bool doCPMixture()   const = 0;

  virtual bool isKnownToEvtGen( int ) const = 0 ;

  virtual bool forcedFragmentation( ) const = 0 ;

  virtual bool inclusiveProduction( ) const = 0 ;

  virtual bool repeatedHadronization( ) const = 0 ;

  virtual double getSignalBr( ) const = 0 ;
  
  virtual bool checkSignalPresence( ) const = 0 ;
};
#endif // GENERATORS_IEVTGENTOOL_H
