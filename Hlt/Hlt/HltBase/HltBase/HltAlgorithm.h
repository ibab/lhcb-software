// $Id: HltAlgorithm.h,v 1.7 2007-02-08 17:32:39 hernando Exp $
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
 *          options to set the pat to input/output tracks and vertices
 *        - deals with the HltSummary
            save an retrieve from summary, set decision and decision type.
 *        - beginExecute, and endExecute methods to do common task 
 *          increase counters, check input counters, fill general histos
 *      at the begin and end of the execute method of Hlt algorithms,
 *         (i.e increase counters, etc) 
 *
 *  options:
 *      InputTracksName, InputTracks2Name,InputVerticesName,PrimaryVerticesName,
 *      OuptutTracksName,OutputVertices:
 *          input tracks and vertices from the HltDataStore (logical objects)
 *          logical objects: the containers are pointers to objects owned
 *                           by an external container (in this case PatDataStore)
 *      PatInputTracksName,PatInputTrack2Name,PatInputVerticesName:
 *          input tracks and vertices from the PatDataStore (own objectes)
 *      SummaryLocation: location of the HltSummary 
 *      SelectionName: name of the SelectionSummary of this algorithm
 *                     (see Event/HltEnums.h)  
 *      IsTrigger: if the decision is possitive a valid trigger will be issued 
 *                 (normally decision indicates continuation of the trigger flow,
 *                  isTrigger = true will indicate a trigger yes event)
 *
 *  Note: look at HltBaseAlg for more functionality and options
 *
 *  @author Hugo Ruiz Perez
 *  @author Jose Angel Hernando Morata
 *  @date   2006-06-15
 */
class HltAlgorithm : public HltBaseAlg {
public:

  // typedef for track and vertices container iterators
  typedef Hlt::TrackContainer::iterator track_iterator;
  typedef Hlt::VertexContainer::iterator vertex_iterator;

  typedef Hlt::TrackContainer::const_iterator track_const_iterator;
  typedef Hlt::VertexContainer::const_iterator vertex_const_iterator;

public:

  /// Standard constructor
  HltAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  /// Standard destructor
  virtual ~HltAlgorithm( ); 

  /** initialize algorithm
   *  Note: call HltAlgorithm::initialize() in your derived algorithm
   *  initialize IDs, generic counters and histograms
   **/
  virtual StatusCode initialize();    

  /** execute algorithm
   * Note: call HltAlgorithm::beginExecute() and HltAlgorithm::endExecute() 
   *       at the begin and end of the excute method of the derived algorithms
   **/
  virtual StatusCode execute   ();    ///< Algorithm execute

  /** finalize algorithm
   * Note: call HltAlgorithm::finalize()
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

  // Counter of Input and Accepted Events
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


  // Input histograms for tracks and vertices
  HltHisto m_histoInputVertices;
  HltHisto m_histoInputPVs;
  HltHisto m_histoInputTracks;
  HltHisto m_histoInputTracks2;

  // Input histograms for tracks and vertices from Pat container
  HltHisto m_histoPatInputTracks;
  HltHisto m_histoPatInputTracks2;
  HltHisto m_histoPatInputVertices;

  // Output histograms for track and vertices
  HltHisto m_histoCandidates;
  HltHisto m_histoOutputVertices;
  HltHisto m_histoOutputTracks;
  HltHisto m_histoOutputTracks2;


protected:

  /** set the decision type to true in selection summary info
   *        (see Event/HltEnums.h for types)
   *        automatically set decision/pass to true
   **/
  inline void setDecisionType(int decisionType, int idsel= -1) {
    LHCb::HltSelectionSummary& sel = selectionSummary(idsel);    
    sel.setDecisionType(decisionType,true);
    if (m_isTrigger) m_summary->setDecisionType(decisionType,true);
    setDecision(true);
  }
  

  /** save a data object (derived from ContainedObject) as Track or RecVertex
   * into the selecion summary info.
   * by the default the selection summary is taked from the option SelectionName
   * user can save an object into other selection summary, indicating the ID
   * of the selection summary 
   * (for IDs of selection summaries see Event/HltEnums.h) 
   **/
  template <class T>
  void saveObjectInSummary(const T& t, int idsel = -1) {
    LHCb::HltSelectionSummary& sel = selectionSummary(idsel);
    ContainedObject* obj = (ContainedObject*) &t;
    sel.addData(*obj);
  }
  
  
  /** save a container of data objeces (i.e Tracks and Vertices) into the
   *  selection summary info
   * by the default the selection summary is taked from the option SelectionName
   * user can save an object into other selection summary, indicating the ID
   * of the selection summary 
   * (for IDs of selection summaries see Event/HltEnums.h) 
   **/
  template <class CONT>
  inline void saveInSummary(const CONT& cont, int idsel = -1) {
    for (typename CONT::const_iterator it = cont.begin(); 
         it != cont.end(); ++it) saveObjectInSummary(**it,idsel);
  }


  /** retrieve a vector of objects saved in a summary selection info
   *  (for the IDs of teh selecion summaries see Event/HltEnums.h)
   **/
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

  /** get the summary 
   **/
  LHCb::HltSummary& summary() {
    if (!m_summary) 
      m_summary = getOrCreate<LHCb::HltSummary,LHCb::HltSummary>(m_summaryLocation);
    return *m_summary;
  }

  /** get the selection summary with a give ID (see Event/HltEnums.h)
   *  by defaul: the selection summary indicated in the option "SelectionName"
   **/
  LHCb::HltSelectionSummary& selectionSummary(int id = -1);
  
protected:

  // Is isTrigger a positive decision will be stored in the summary
  bool m_isTrigger;

  // TODO: do we need it?
  bool m_decision;
  
  // name of the location of the summary
  std::string m_summaryLocation;

  // name of the selection summary
  std::string m_selectionName;

  // ID of the selection summary
  int m_selectionSummaryID;
  
 protected:

  // pointer to the summary
  LHCb::HltSummary* m_summary;

  // pointer to the selection summary
  LHCb::HltSelectionSummary* m_selectionSummary;

protected:

  // pointer to the pat data Store
  PatDataStore* m_patDataStore;

  // pointer to the hlt data store
  IHltDataStore* m_hltDataStore;

  // names of the pat input containers
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

  // pointers to the hlt containers
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
