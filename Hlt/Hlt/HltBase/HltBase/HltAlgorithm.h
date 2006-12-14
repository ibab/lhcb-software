// $Id: HltAlgorithm.h,v 1.4 2006-12-14 11:21:35 hernando Exp $
#ifndef HLTBASE_HLTALGORITHM_H 
#define HLTBASE_HLTALGORITHM_H 1

// Include files

#include "GaudiAlg/GaudiHistoAlg.h"
#include "DetDesc/Condition.h"
#include "Event/HltSummary.h"
#include "Event/HltSummaryFunctor.h"
#include "Event/HltNames.h"

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
class HltAlgorithm : public GaudiHistoAlg {
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

  //-----------------------
  // stop and error methods
  //-----------------------

  // check that this a required container it is not null
  // i.e if the inputTracks needs to be there!
  template <class CON>
  inline void checkInput(CON*& cont, const std::string& comment) {
    if (cont == NULL) error() << comment << endreq;
  }

  template <class T>
  bool retrieve(T*& t, const std::string& name) {
    t = NULL;
    try {t  = get<T>(name);}
    catch (GaudiException) {warning() << " No obj at " << name << endreq;}
    return (t != NULL);
  }

  // getting out of the execute method, print a message or send a error
  inline StatusCode stop(const std::string& message, bool err = false)
  {
    if (err) error() << message << endreq;
    debug() << message << endreq;
    return err? StatusCode::FAILURE: StatusCode::SUCCESS;
  }

protected:

  //---------------------------------------------
  // methods dealing with histograms and counters 
  //---------------------------------------------

  // Counter class
  // TODO: to be revisit, we should use histogram instead
  class HltCounter {
  public:
    std::string m_name;
    long m_counter;
    bool m_initialized;
    HltCounter () {  m_initialized = false; };
    // Return counter value if treated as int
    operator int () const { return m_counter;};
    friend MsgStream& operator<< ( MsgStream& o, HltCounter& counter ) {
      o << counter.m_counter;
      return o;
    }
  };
  
  // Histo class
  // TODO: to be revisit, update for algorithms instead?
  unsigned m_histogramUpdatePeriod;
  class HltHisto {
  public:
    std::string m_name;
    bool m_initialized;
    int m_updatePeriod;
    AIDA::IHistogram1D* m_histo;
    HltHisto () { 
      m_initialized = false ; 
      m_updatePeriod = -1;
    }
  };
  
  // Histo collection class
  // TODO: how we use it?
  class HistoCol 
  {
  public:
    std::string m_name;
    std::vector<std::string> m_variables;
    std::vector<std::string> m_cases;
    std::vector<AIDA::IHistogram1D*> m_histos;
    bool m_initialized;
    unsigned m_nVariables;
    unsigned m_nCases;
    HistoCol ()  { m_initialized = false ; };
 };

  // initialize this histogram with a given titleID
  void initializeHisto( HltHisto& histo, const std::string& name );

  // initialize histograms
  void initializeHistoCol( HistoCol& histoCol,
                           const std::string& inputString,
                           const std::vector<std::string>& inputVariables,
                           const std::vector<std::string>& inputCases );
  
  // initialize and book this histogram
  // the booking can be changed via options using the HisDescriptor
  void initializeHisto( HltHisto& histo, const std::string& name,
                        float min, float max, int nBins );
  
  // fill histogram with this wait
  // it will be filled with the period set by options
  void fillHisto( HltHisto& histo, float x, float weight );

  // fill collection of histogram
  void fillHistoCol( HistoCol& histoCol, 
                     const std::vector<bool>& inputBools,
                     const std::vector<float>& inputVariables, 
                     float weight);

  // initialize counter
  // TODO: revisit
  void initializeCounter( HltCounter& Counter, const std::string& name );
  
  // increase counter
  // TODO: revisit
  void increaseCounter( HltCounter& Counter, int increase = 1);
  
  // Formated text messages
  void infoTotalEvents    ( int  nTotEvts );

  // Formated text messages, print also fraction of events
  void infoSubsetEvents   ( int nEventsInSubset, int  nTotEvts, 
                            const std::string& subsetName );

  void infoInputObjects   ( int nInputObjects, int  nTotEvts, 
                            const std::string& objectsName  );

  void infoAcceptedObjects( int nAcceptedObjects, int nInputObjects,
                            int  nTotEvts, const std::string& objectsName );

protected:
  
  // Input counters
  HltCounter m_countInput;
  HltCounter m_countNonEmpty;
  HltCounter m_countEmptyTracks;
  HltCounter m_countEmptyTracks2;
  HltCounter m_countInputVertices;
  HltCounter m_countEmptyVertices;
  HltCounter m_countEmptyPVs;
  HltCounter m_countInputPVs;
  HltCounter m_countInputTracks;
  HltCounter m_countInputTracks2;

  // Output coutners
  HltCounter m_countCandidates;
  HltCounter m_countAccepted;
  HltCounter m_countOverruled;
  HltCounter m_countOutputTracks;
  HltCounter m_countOutputTracks2;
  HltCounter m_countOutputVertices;

  HltCounter m_counterEntries;
  HltCounter m_counterInput;
  HltCounter m_counterPasses;

  // counter histograms
  HltHisto m_histoEntries;
  HltHisto m_histoPasses;

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

  // declare an integer condition to be read from the condDB
  inline void declareCondition(const std::string& name, int& val)
  {m_iconditions[name] = &val;}

  // declare a double condition to be read from the condDB
  inline void declareCondition(const std::string& name, double& val)
  {m_dconditions[name] = &val;}

  // declare a double condition to be read from the condDB
  inline void declareCondition(const std::string& name, 
                               std::vector<double>& val)
  {m_dvconditions[name] = &val;}

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

  
  // fill histogram with the size of the container 
  template <class CON>
  bool size(CON*& con, int& n, HltHisto& h, const std::string& comment) {
    if (!con) return true;
    n = con->size(); fillHisto(h,n,1.);
    debug() << comment << n << endreq;
    if (n == 0) setDecision(false);
    return (n>0);
  }


  // fill the histogram with the info stored in the objects of the container
  // intended to do internal monitoring, before we apply any cut
  template <class INPUT >
  void monitor(INPUT& con, int key, HltHisto& histo){
    if (!m_monitor) return;
    for (typename INPUT::iterator it = con.begin(); it != con.end(); ++it) {
      double d = (*it)->info(key,-1.);
      verbose() << " monitor " << key << " " << d << endreq;
      fillHisto( histo, d, 1.);
    }
  }
  
protected:

  // Minimum number of candidates needed to set filterPassed to true
  long m_nCandidates;

  void candidateFound() { m_nCandidates++; };

  // always force a positive decision of the algorithms 
  // (controlled by options)
  bool m_filter;

  // monitor this event (depends on a period set by options)
  bool m_monitor;

  // setting the decision, inform Gaudi about the decision
  void setDecision(bool ok) {
    if (m_filter) setFilterPassed(ok);
    debug() << " set decision " << ok << endreq;
  }

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

  // initialize the Msg service
  void initMsg();
  
  // initialize the containers
  bool initContainers();

  // initialize the histograms
  void initHistograms();  

  // initialize the counters
  void initCounters();
  
  // initialize the conditions
  bool initConditions();

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
      info() << " retrieving selection summary id " << id << endreq;
    m_selectionSummary = &(m_summary->selectionSummary(id));
    return *m_selectionSummary;
  }

protected:

  // bool about the msg level
  bool m_verbose;
  bool m_debug;
  bool m_info;
  bool m_warning;
  bool m_error;
  bool m_fatal;

  // print information that we use this container
  template <class CON>
  void infoContainer(CON*& con, const std::string& name, 
                     const std::string& comment) 
  {if (con) info() << " using container " << comment << " " << name << endreq;}
  
protected:

  // Is isTrigger a positive decision will be stored in the summary
  bool m_isTrigger;

  // TODO: do we need it?
  bool m_decision;

  // The period to always return a positive decision
  int m_noFilterPeriod;
  
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

protected:

  // Property to rebook histogram from options
  StringArrayProperty m_histoDescriptor;

protected:

  // location of the conditions
  std::string m_conditionsName;

  // internal method to deal with the conditions
  StatusCode i_cacheConditions();

    
  // pointer to conditions
  Condition* m_conditions;

  // maps of the conditions
  std::map<std::string, double*> m_dconditions;
  std::map<std::string, int*> m_iconditions;
  std::map<std::string, std::vector<double>*> m_dvconditions;

};
#endif // HLTBASE_HLTALGORITHM_H
