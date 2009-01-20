// $Id: SignalPlain.h,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
#ifndef GENERATORS_SIGNALPLAIN_H 
#define GENERATORS_SIGNALPLAIN_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/Signal.h" 

/** @class SignalPlain SignalPlain.h 
 *  
 *  Tool for signal generation with plain method. Concrete 
 *  implementation of ISampleGenerationTool using the Signal
 *  base class.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class SignalPlain : public Signal {
 public:
  /// Standard constructor
  SignalPlain( const std::string & type , const std::string & name ,
               const IInterface * parent ) ;
  
  virtual ~SignalPlain( ); ///< Destructor
  
  /** Generates events containing a signal particle.
   *  Implements ISampleProductionTool::generate.
   *  Loop over events until it contains a signal particle
   *  without using any trick (just like Inclusive).
   */
  virtual bool generate( const unsigned int nPileUp , 
                         LHCb::HepMCEvents * theEvents ,
                         LHCb::GenCollisions * theCollisions ) ;  

 private:
};
#endif // GENERATORS_SIGNALPLAIN_H
