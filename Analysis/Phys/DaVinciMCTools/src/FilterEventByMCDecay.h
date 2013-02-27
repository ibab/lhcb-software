// $Id: FilterEventByMCDecay.h,v 1.2 2009-04-30 12:52:11 jpalac Exp $
#ifndef FILTEREVENTBYMCDECAY_H 
#define FILTEREVENTBYMCDECAY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IMCDecayFinder;

/** @class FilterEventByMCDecay FilterEventByMCDecay.h
 *  
 *  Algorithm to select or reject an event based on whether an event is 
 *  "signal". It uses a private instance of an IMCDecayFinder to determine 
 *  whether the MC event is consistent with being signal. In practice this
 *  means that an MCDecayFinder (the only existing implementation of 
 *  interfaceoid IMCDecayFinder) is used, and its decay descriptor used to
 *  determine whether the event is signal or not.
 *
 *  <b>Properties:</b>
 *
 *  <b>Select</b>: bool, controls whether event is accepted or rejected
 *  if it is found to contain an MC signal.  Default <b>true</b>.
 *
 *  <b>Example</b>: Remove signal Bs->J/Psi(mumu)Phi(KK) events.
 *
 *  @code
 *  # remove events that are signal
 *  signalKiller = FilterEventByMCDecay()
 *  signalKiller.Select = False
 *  signalKiller.addTool(MCDecayFinder)
 *  signalKiller.MCDecayFinder.Decay = '[B_s0 -> (J/psi(1S) -> mu+ mu- {,gamma} {,gamma}) (phi(1020) -> K+ K-)]cc'
 *  # Assume TheSelSeq is a sequence that is carrying out a selection
 *  TheSelSeq.Members += [signalKiller] # you might want to add to fron of sequence 
 *  @endcode
 *
 *  @author Juan PALACIOS
 *  @date   2009-04-28
 */
class FilterEventByMCDecay : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterEventByMCDecay( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterEventByMCDecay( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  bool m_select;
  IMCDecayFinder* m_decayFinder;
  std::string m_inputLocation;
  
};
#endif // FILTEREVENTBYMCDECAY_H
