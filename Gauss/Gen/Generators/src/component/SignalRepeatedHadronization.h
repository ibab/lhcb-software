// $Id: SignalRepeatedHadronization.h,v 1.2 2005-11-17 15:57:31 robbep Exp $
#ifndef GENERATORS_SIGNALREPEATEDHADRONIZATION_H 
#define GENERATORS_SIGNALREPEATEDHADRONIZATION_H 1

// Include files
#include "Generators/Signal.h" 

/** @class SignalRepeatedHadronization SignalRepeatedHadronization.h 
 *  
 *  Tool for signal with repeated hadronization method samples generation
 *
 *  @author Patrick Robbe
 *  @date   2005-08-18
 */
class SignalRepeatedHadronization : public Signal {
public:
  /// Standard constructor
  SignalRepeatedHadronization( const std::string& type, 
                               const std::string& name,
                               const IInterface* parent);
  
  virtual ~SignalRepeatedHadronization( ); ///< Destructor

  virtual StatusCode initialize( ) ; ///< Initialize
  
  virtual bool generate( const unsigned int nPileUp , 
                         EventVector & theEventVector ,
                         HardVector  & theHardVector ) ;

protected:

private:
  unsigned int  m_nEventsBeforeCut ;
  unsigned int  m_nEventsAfterCut  ;
  unsigned int  m_nInvertedEvents  ;
  unsigned int  m_maxNumberOfRepetitions ;
  PIDs          m_pidQuarks        ;
};
#endif // GENERATORS_SIGNALREPEATEDHADRONIZATION_H
