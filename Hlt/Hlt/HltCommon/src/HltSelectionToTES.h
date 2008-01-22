// $Id: HltSelectionToTES.h,v 1.1 2008-01-22 09:56:36 hernando Exp $
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

protected:

  bool m_copyAll;

  StringArrayProperty m_AselectionNames;

  std::vector<std::string> m_selectionNames;
  std::vector<int> m_selectionIDs;

  std::string m_trackLocation;
  std::string m_vertexLocation;

  template <class TESCONT, class T>
  TESCONT* copy(int id) 
  {
    typedef typename Hlt::TSelection<T> TSelection; 
    typedef typename Hlt::TSelection<T>::iterator TSelectionIterator;
    TESCONT* tcont = new TESCONT();
    Hlt::Selection& sel = hltData().selection(id);
    TSelection& tsel = *(dynamic_cast<TSelection*>(&sel));
    for (TSelectionIterator it = tsel.begin(); it != tsel.end(); ++it) {
      const T& t = *(*it); T* tc = t.clone(); tcont->insert(tc);
    }
    return tcont;
  }


};
#endif // HLTTRACKFROMTES_H
