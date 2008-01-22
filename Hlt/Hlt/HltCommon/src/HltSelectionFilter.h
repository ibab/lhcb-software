// $Id: HltSelectionFilter.h,v 1.1 2008-01-22 09:56:36 hernando Exp $
#ifndef HLTCOMMON_HLTSELECTIONFILTER_H 
#define HLTCOMMON_HLTSELECTIONFILTER_H 1

// Include files
// from Gaudi

#include "HltBase/HltAlgorithm.h"

/** @class HltSelectionFilter HltSelectionFilter.h
 *  
 *  functionality:
 *      filters a trigger with a list of selections:
 *       selections  = selection1 AND selection2
 *       selections' = selection3 AND selection4
 *       decision    = (selections) OR (selections')
 *
 *  Options:
 *      selections += {"selection1,selection2","selection3,selection4"}
 *  @author Jose Angel Hernando Morata
 *  @date   2007-07-27
 */
class HltSelectionFilter : public HltAlgorithm {
public: 
  /// Standard constructor
  HltSelectionFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSelectionFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  // Use HltAlgorithm m_extraInputSelectionsNames;
  // StringArrayProperty m_inputSelectionsNames;

  std::vector<int> m_scounters;

  Hlt::TSelection<Hlt::Selection>* m_outputSelections;

};
#endif // HLTSELECTIONENTRY_H
