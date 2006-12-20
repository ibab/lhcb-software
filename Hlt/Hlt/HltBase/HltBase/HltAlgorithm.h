// $Id: HltAlgorithm.h,v 1.5 2006-12-20 09:32:44 hernando Exp $
#ifndef HLTBASE_HLTALGORITHM_H 
#define HLTBASE_HLTALGORITHM_H 1

// Include files
#include "Event/HltSummary.h"
#include "Event/HltSummaryFunctor.h"
#include "Event/HltNames.h"

#include "HltBase/HltBaseAlg.h"
#include "HltBase/IHltDataStore.h"
#include "PatTools/PatDataStore.h"

/** @class HltAlgorithm 
 *  
 *  Base class for HLT algorithms
 *  functionality:
 *        - deals with input/output tracks from Pat and Hlt stores
 *        - book and fill default histograms: i.e: number of input tracks
 *        - set a period to fill histograms
 *        - set a period to overrule the decision of the algorithm
 *        - deals with the HLT summary
 *
 *  @author Hugo Ruiz Perez
 *  @author Jose Angel Hernando Morata
 *  @date   2006-06-15
 */
class HltAlgorithm : public HltBaseAlg {
public:

  typedef Hlt::TrackContainer::iterator track_iterator;
  typedef Hlt::VertexContainer::iterator vertex_iterator;

  typedef Hlt::TrackContainer::const_iterator track_const_iterator;
  typedef Hlt::VertexContainer::const_iterator vertex_const_iterator;

public:

  /// Standard constructor
  HltAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  // begin the execution
  //    check that the input tracks and vertices are ready
  //    fill histograms and counters
  //    set the decision to false, and deals with the report to the summary
  bool beginExecute();

  // end of the execution
  //    fill histogram and counters of the output tracks and vertices
  //    set the decision to true
  bool endExecute();

protected:

  // Input counters
  HltCounter m_counterInput;
  HltCounter m_counterAccepted;


//   HltCounter m_countEmptyTracks;
//   HltCounter m_countEmptyTracks2;
//   HltCounter m_countInputVertices;
//   HltCounter m_countEmptyVertices;
//   HltCounter m_countEmptyPVs;
//   HltCounter m_countInputPVs;
//   HltCounter m_countInputTracks;
//   HltCounter m_countInputTracks2;

  // Output coutners


  // Input histograms
  HltHisto m_histoInputVertices;
  HltHisto m_histoInputPVs;
  HltHisto m_histoInputTracks;
  HltHisto m_histoInputTracks2;

  HltHisto m_histoPatInputTracks;
  HltHisto m_histoPatInputTracks2;
  HltHisto m_histoPatInputVertices;

  // Output histograms
  HltHisto m_histoCandidates;
  HltHisto m_histoOutputVertices;
  HltHisto m_histoOutputTracks;
  HltHisto m_histoOutputTracks2;


protected:

  // set the decision type on the summary
  inline void setDecisionType(int decisionType, int idsel= -1) {
    LHCb::HltSelectionSummary& sel = selectionSummary(idsel);    
    sel.setDecisionType(decisionType,true);
    if (m_isTrigger) m_summary->setDecisionType(decisionType,true);
    setDecision(true);
  }
  

  // save in sammry this track
  template <class T>
  void saveObjectInSummary(const T& t, int idsel = -1) {
    LHCb::HltSelectionSummary& sel = selectionSummary(idsel);
    ContainedObject* obj = (ContainedObject*) &t;
    sel.addData(*obj);
  }
  
  
  // save in summary these tracks
  template <class CONT>
  inline void saveInSummary(const CONT& cont, int idsel = -1) {
    for (typename CONT::const_iterator it = cont.begin(); 
         it != cont.end(); ++it) saveObjectInSummary(**it,idsel);
  }
  
  template <class T>
  void retrieveFromSummary(int idsel, std::vector<T*>& tobjs) {
    HltSummaryFunctor::retrieve(*m_summary,idsel,tobjs);
  }
  

protected:

  
  // initialize the containers
  bool initContainers();

  // initialize the histograms
  void initHistograms();  

  // initialize the counters
  void initCounters();

  // retrive a track container from a store with a given name
  void init(Hlt::TrackContainer*& con, IHltDataStore*& store,
            const std::string& name);
  
  // retrieve a vertex container from a store with a given name
  void init(Hlt::VertexContainer*& con, IHltDataStore*& store,
            const std::string& name);
  
  // retrieve a pat track container from the pat store with a given name
  void init(PatTrackContainer*& con, PatDataStore*& store,
            const std::string& name);

  // retrieve a pat vertex container from the pat store with a given name
  void init(PatVertexContainer*& con, PatDataStore*& store,
            const std::string& name);
protected:

  // get the summary
  LHCb::HltSelectionSummary& selectionSummary(int id = -1) {
    if (!m_summary) m_summary = get<LHCb::HltSummary>(m_summaryName);
    if (id>0) return m_summary->selectionSummary(id);
    id = m_selectionSummaryID;
    if (!m_selectionSummary)
      debug() << " retrieving selection summary id " << id << endreq;
    m_selectionSummary = &(m_summary->selectionSummary(id));
    return *m_selectionSummary;
  }
  
protected:

  // Is isTrigger a positive decision will be stored in the summary
  bool m_isTrigger;

  // TODO: do we need it?
  bool m_decision;
  
  // name of the location of the summary
  std::string m_summaryName;

  // name of the algorithm name to report
  std::string m_selectionSummaryName;

  // ID of the algorithm name to report
  int m_selectionSummaryID;
  
  // pointer to the summary
  LHCb::HltSummary* m_summary;

  // pointer to the algorithm report
  LHCb::HltSelectionSummary* m_selectionSummary;

protected:

  // pointer to the pat data Store
  PatDataStore* m_patDataStore;
  // pointer to the hlt data store
  IHltDataStore* m_hltDataStore;

  // names of the pat containers
  std::string m_patInputTracksName;
  std::string m_patInputTracks2Name;
  std::string m_patInputVerticesName;
  
  // pointers to the pat containers
  PatTrackContainer* m_patInputTracks;
  PatTrackContainer* m_patInputTracks2;
  PatVertexContainer* m_patInputVertices;

  // names of the hlt containers
  std::string m_inputTracksName;
  std::string m_inputTracks2Name;
  std::string m_inputVerticesName;
  std::string m_primaryVerticesName;

  std::string m_outputTracksName;
  std::string m_outputVerticesName;

  // pointer to the hlt containers
  Hlt::TrackContainer* m_inputTracks;
  Hlt::TrackContainer* m_inputTracks2;
  Hlt::VertexContainer* m_inputVertices;
  Hlt::VertexContainer* m_primaryVertices;

  Hlt::TrackContainer* m_outputTracks;
  Hlt::VertexContainer* m_outputVertices;

  // size of the input containers
  int m_nInputTracks;
  int m_nInputTracks2;
  int m_nInputVertices;
  int m_nPrimaryVertices;
  
  int m_nPatInputTracks;
  int m_nPatInputTracks2;
  int m_nPatInputVertices;
  
  // size of the output containers
  int m_nOutputTracks;
  int m_nOutputVertices;
};
#endif // HLTBASE_HLTALGORITHM_H
