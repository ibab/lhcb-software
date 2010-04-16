// $Id: HltGlobalHcalCut.h,v 1.1 2010-04-16 01:17:13 gligorov Exp $
#ifndef HLTGLOBALHCALCUT_H 
#define HLTGLOBALHCALCUT_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "Event/L0CaloCandidate.h"
#include "Event/CaloDigit.h"

/** @class HltGlobalHcalCut HltGlobalHcalCut.h 
 *  
 *
 *  @author V. Gligorov
 *  @date   2008-02-12
 */

class HltGlobalHcalCut : public HltAlgorithm {
public: 
  /// Standard constructor
  HltGlobalHcalCut( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltGlobalHcalCut(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

  StatusCode filter_Clusters();

  Hlt::SelectionContainer2<LHCb::L0CaloCandidate,LHCb::L0CaloCandidate> m_selections;

  int m_maxNumClusters; 

};

#endif // HLTGLOBALHCALCUT_H
