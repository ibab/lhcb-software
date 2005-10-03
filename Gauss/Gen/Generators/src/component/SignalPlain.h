// $Id: SignalPlain.h,v 1.1 2005-10-03 10:30:25 robbep Exp $
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
  unsigned int  m_nEventsBeforeCut ;
  unsigned int  m_nEventsAfterCut  ;
  unsigned int  m_nInvertedEvents  ;
};
#endif // GENERATORS_SIGNALPLAIN_H
