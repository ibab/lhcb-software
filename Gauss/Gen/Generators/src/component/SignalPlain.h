// $Id: SignalPlain.h,v 1.2 2005-11-21 16:18:05 robbep Exp $
#ifndef GENERATORS_SIGNALPLAIN_H 
#define GENERATORS_SIGNALPLAIN_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/Signal.h" 

/** @class SignalPlain SignalPlain.h 
 *  
 *  Tool for signal with plain method samples generation
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class SignalPlain : public Signal {
public:
  /// Standard constructor
  SignalPlain( const std::string& type, 
               const std::string& name,
               const IInterface* parent);
  
  virtual ~SignalPlain( ); ///< Destructor
  
  virtual bool generate( const unsigned int nPileUp , 
                         EventVector & theEventVector ,
                         HardVector  & theHardVector ) ;

protected:

private:
};
#endif // GENERATORS_SIGNALPLAIN_H
