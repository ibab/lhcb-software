// $Id: HltSelectionFilter.h,v 1.5 2008-12-20 17:59:36 graven Exp $
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

private:

  StringArrayProperty m_inputSelectionNames;
  std::string m_outputSelectionName;

  std::vector<Hlt::Selection*> m_input;
  Hlt::TSelection<Hlt::Selection>* m_output;

};
#endif // HLTSELECTIONENTRY_H
