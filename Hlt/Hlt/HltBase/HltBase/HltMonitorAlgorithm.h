// $Id: HltMonitorAlgorithm.h,v 1.3 2007-02-08 17:32:39 hernando Exp $
#ifndef HLTBASE_HLTMONITORALGORITHM_H 
#define HLTBASE_HLTMONITORALGORITHM_H 1

// Include files
#include "Event/HltSummary.h"
#include "Event/HltSummaryFunctor.h"
#include "Event/HltNames.h"
#include "Event/Track.h"

#include "HltBase/HltBaseAlg.h"
#include "HltBase/HltContainers.h"

/** @class HltMonitorAlgorithm 
 *  
 *  Base class for HLT Monitor algorithms
 *  functionality:
 *        - retrieves HltSummary
 *           allow access tracks and vertices stored in the selection summary
 *           check is a given track is saved in a selection summary
 *        - monitoring
 *           provide template methods to monitor info in tracks, vertices
 *
 *  Options:
 *
 *  Note: look at HltBaseAlg for more functionality and options
 *
 *  @author Hugo Ruiz Perez
 *  @author Jose Angel Hernando Morata
 *  @date   2006-06-15
 */
class HltMonitorAlgorithm : public HltBaseAlg {
public:

  // typedef for track and vertices container iterators
  typedef Hlt::TrackContainer::iterator track_iterator;
  typedef Hlt::VertexContainer::iterator vertex_iterator;

  typedef Hlt::TrackContainer::const_iterator track_const_iterator;
  typedef Hlt::VertexContainer::const_iterator vertex_const_iterator;

public:

  /// Standard constructor
  HltMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  /// Standard destructor
  virtual ~HltMonitorAlgorithm( ); 

  /** initialize algorithm
   *  Note: call HltMonitorAlgorithm::initialize() in your derived algorithm
   *  initialize IDs, generic counters and histograms
   **/
  virtual StatusCode initialize();    

  /** execute algorithm
   * Note: call HltMonitorAlgorithm::beginExecute() and HltMonitorAlgorithm::endExecute() 
   *       at the begin and end of the excute method of the derived algorithms
   **/
  virtual StatusCode execute   ();    ///< Algorithm execute

  /** finalize algorithm
   * Note: call HltMonitorAlgorithm::finalize()
   * print out info of the accepted events, etc.
   * 
   **/
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /** begin the execution
   *    call HltBaseAlgo::beginExecute() (see HltBaseAlgo)
   *             set decesion to false, set histo monitor bool flag
   *    retrieve selection summary if necessary
   *    check that the input tracks and vertices containers are not empty
   *    increase entry counters
   **/
  bool beginExecute();

  /** end of the execution
   *    increase histos of output tracks/vertices and couter of accepted events
   *    call HltBase::endExecute() (see HltBaseAlgo)
   *             set decision to true
   **/ 
  bool endExecute();
  

protected:

  /** get the summary 
   **/
  const LHCb::HltSummary& summary() {
    if (!m_summary)  m_summary= get<LHCb::HltSummary>(m_summaryName);
    return *m_summary;
  }

  /** returns true if selection is in summary
   **/
  bool hasSelection(int id)
  {return summary().hasSelectionSummary(id);}

  /** returns true if selection is in summary
   **/
  bool selectionDecision(int id) {
    if (!hasSelection(id)) return false;
    else return selectionSummary(id).decision();
  }

  /** returns true if the default (by options) decision types have triggered
   **/
  bool checkDecisionTypes() 
  {return checkDecisionTypes(m_decisionTypesIDs);}

  /** returns true if all decision types have triggered
   **/
  bool checkDecisionTypes(const std::vector<int>& ids) 
  {
    for (std::vector<int>::const_iterator it = ids.begin(); 
         it != ids.end(); ++it) 
      if (!summary().checkDecisionType(*it)) return false;
    return true;
  }

  /** returns true if the default (by options) selections are in summary
   **/
  bool hasSelections() 
  {return hasSelections(m_selectionsIDs);}

  /** returns true if all the selections ids are in summary
   **/
  bool hasSelections(const std::vector<int>& ids) 
  { 
    for (std::vector<int>::const_iterator it = ids.begin(); 
         it != ids.end(); ++it) if (!hasSelection(*it)) return false;
    return true; 
  }

  /** get the selection summary with a give ID (see Event/HltEnums.h)
   *  by defaul: the selection summary indicated in the option "SelectionName"
   **/
  const LHCb::HltSelectionSummary& selectionSummary(int id);
  
  /** retrieve a vector of objects saved in a summary selection info
   *  (for the IDs of teh selecion summaries see Event/HltEnums.h)
   **/
  template <class T>
  void retrieveFromSummary(int idsel, std::vector<T*>& tobjs) {
    HltSummaryFunctor::retrieve(summary(),idsel,tobjs);
  }
  
  /** returns true if this tracks is saved in a selection
   **/
  bool isInSelection(const LHCb::Track& track, int id);
  
protected:

  /** monitor info from objects according with keys
   * 
   **/
  template <class CONTAINER>
  void monitorInfo(CONTAINER& con, std::vector<std::string> keys) {
    if (con.size() <=0) return;
    for (std::vector<std::string>::iterator it = keys.begin();
         it != keys.end(); ++it) {
      const std::string& title = (*it);
      int key = HltNames::particleInfoID(title);
      if (key <0) continue;
      for (typename CONTAINER::iterator it2 = con.begin(); 
           it2 != con.end(); it2++) {
        if ((*it2)->hasInfo(key)) 
          fill( histo1D(title), (*it2)->info(key,0), 1.);
      }
    } 
  }
  
protected:

  // initialize the counters
  void initCounters();

protected:

  // Counter of Input and Accepted Events
  HltCounter m_counterInput;
  HltCounter m_counterAccepted;
  
protected:
  
  // name of the location of the summary
  std::string m_summaryName;
  
  // list of hlt selections names
  StringArrayProperty m_selectionsName;

  // list of decision types names
  StringArrayProperty m_decisionTypesName;

  // list of hlt selection ids
  std::vector<int> m_selectionsIDs;

  // list of decision types ids
  std::vector<int> m_decisionTypesIDs;

  // pointer to the summary
  LHCb::HltSummary* m_summary;

  // pointer to the selection summary
  LHCb::HltSelectionSummary* m_selectionSummary;


};
#endif // HLTBASE_HLTMONITORALGORITHM_H
