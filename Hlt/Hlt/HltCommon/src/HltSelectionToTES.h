// $Id: HltSelectionToTES.h,v 1.2 2008-05-15 08:56:55 graven Exp $
#ifndef HLTCOMMON_HLTSELECTIONTOTES_H 
#define HLTCOMMON_HLTSELECTIONTOTES_H 1

// Include files
// from Gaudi
#include "HltBase/HltBaseAlg.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"

/** @class HltSelectionToTES HltSelectionToTES.h
 *  
 *  @author Jose Angel Hernando Morata
 *  @date   2006-09-01
 */
class HltSelectionToTES : public HltBaseAlg {
public: 
  /// Standard constructor
  HltSelectionToTES( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSelectionToTES( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  bool m_copyAll;

  StringArrayProperty m_AselectionNames;

  std::vector<stringKey> m_selectionIDs;

  std::string m_trackLocation;
  std::string m_vertexLocation;
};
#endif // HLTTRACKFROMTES_H
