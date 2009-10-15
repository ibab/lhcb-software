// $Id: HltCorrelations.h,v 1.1 2009-10-15 12:32:16 pkoppenb Exp $
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

private:
  StatusCode moreAlgorithms(); ///< Fill more algorithms

  strings m_moreAlgorithms ;   ///< additional algorithms
  mutable IAlgorithmCorrelations* m_algoCorr ;  ///< Correlation tool
  mutable ICheckSelResults* m_selTool ; ///< Selection results tool
};
#endif // HLTCORRELATIONS_H
