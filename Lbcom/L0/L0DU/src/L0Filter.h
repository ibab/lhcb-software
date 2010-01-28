// $Id: L0Filter.h,v 1.4 2010-01-28 16:55:23 odescham Exp $
#ifndef L0FILTER_H 
#define L0FILTER_H 1

// Include files
#include "L0Base/L0AlgBase.h"


/** @class L0Filter L0Filter.h
 *  
 *  Algorithm that checks if L0Filter passed. Useful for conditional sequencing.
 *  
 *  To require any L0-yes, just run the algorithm unconfigured.
 *  
 *  To require a specific L0 channel do for instance
 *   
 *  @code
 *  L0Filter.OrChannels = { "L0electron_High", "L0globalPi0_High" };
 *  @endcode
 *  
 *  This will accept only events which passed L0electron_High or L0globalPi0_High.
 *  Only the or-mode is supported. To accept events that passed two or more given 
 *  channels use two instances of L0Filter.
 *  
 *  Valid channel names are given in $L0DUROOT/options/L0DU_Lumi2Setting.opts 
 *  be very careful about spelling. Misspelled L0 channels will return false
 *  with no warning.
 *
 *  @author Patrick Koppenburg
 *  @date   2007-07-11
 */
class L0Filter : public L0AlgBase {
public: 
  /// Standard constructor
  L0Filter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0Filter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_l0Location ;              ///< Location of L0 decision
  std::vector<std::string> m_l0channels ; ///< required channels (Or-ed)
  std::string m_trig;
  unsigned long m_count;
  unsigned long m_sel;
  int m_mask;
  std::vector<std::string> m_l0triggers;
  bool m_revert;
};
#endif // L0FILTER_H
