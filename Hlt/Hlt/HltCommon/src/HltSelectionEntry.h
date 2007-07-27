// $Id: HltSelectionEntry.h,v 1.2 2007-07-27 22:33:17 hernando Exp $
#ifndef HLTSELECTIONENTRY_H 
#define HLTSELECTIONENTRY_H 1

// Include files
// from Gaudi

#include "HltBase/HltAlgorithm.h"

/** @class HltSelectionEntry HltSelectionEntry.h
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
class HltSelectionEntry : public HltAlgorithm {
public: 
  /// Standard constructor
  HltSelectionEntry( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSelectionEntry( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::vector< std::string > m_selectionNames;
 
  std::vector<std::vector<int> > m_selectionIDs;

  std::vector<int> m_scounters;

  StringArrayProperty m_selections;


};
#endif // HLTSELECTIONENTRY_H
