// $Id: HltCorrelations.h,v 1.3 2009-11-10 17:11:14 pkoppenb Exp $
#ifndef HLTCORRELATIONS_H 
#define HLTCORRELATIONS_H 1

// Include files
// from Hlt
#include "HltSelectionsBase.h"
/** @class HltCorrelations HltCorrelations.h
 *  
 *  A more advanced SelResultsCorrelations for the HLT
 *
 *  @author Patrick Koppenburg
 *  @date   2007-11-23
 */
class HltCorrelations : public HltSelectionsBase {
public: 
  /// Standard constructor
  HltCorrelations( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltCorrelations( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  /// shortcut
  StatusCode fillResult(std::string sel, const LHCb::HltDecReports* decReports){
    return (m_algoCorr->fillResult(sel,(decReports->decReport(sel))? 
                                   (decReports->decReport(sel)->decision()):0));
  }
  // bitx
  std::string bitX(unsigned int i){
    return "Bit"+std::to_string(i);
  }

private:
  StatusCode moreAlgorithms(); ///< Fill more algorithms
  StatusCode createSelections(); ///<get list of selections

  strings m_moreAlgorithms ;   ///< additional algorithms
  mutable IAlgorithmCorrelations* m_algoCorr ;  ///< Correlation tool
  mutable ICheckSelResults* m_selTool ; ///< Selection results tool
  unsigned int m_firstBit; ///< first routing bit to look at
  unsigned int m_lastBit ;///< last routing bit to look at
  bool m_first ; ///< first event?
};
#endif // HLTCORRELATIONS_H
