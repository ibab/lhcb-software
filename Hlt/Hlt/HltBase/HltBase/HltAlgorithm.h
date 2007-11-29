// $Id: HltAlgorithm.h,v 1.19 2007-11-29 12:59:03 graven Exp $
#ifndef HLTBASE_HLTALGORITHM_H 
#define HLTBASE_HLTALGORITHM_H 1

// Include files
#include "Event/HltSummary.h"
#include "Event/HltEnums.h"
#include "Event/HltNames.h"

#include "HltBase/HltBaseAlg.h"
#include "HltBase/HltTypes.h"
#include "HltBase/HltConfigurationHelper.h"

/** @class HltAlgorithm 
 *  
 *  functionality:
 *        - deals with input/output tracks from the summary-data 
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
 *                           by the TES
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
   *  this system will cal beginExecute() and end Execute() 
   **/
  virtual StatusCode execute   () 
  { return StatusCode::SUCCESS;};    ///< Algorithm execute

  /** finalize algorithm
   * Note: call HltAlgorithm::finalize()
   * print out info of the accepted events, etc.
   **/
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  //-----------------------
  // initializetion methods
  //-----------------------

  // initialize the containers
  bool initContainers();

  // initialize the histograms
  void initHistograms();  

  // initialize the counters
  void initCounters();

  // save configurtion of algorithm
  virtual void saveConfiguration();

protected:

  //-------------------
  // execution methods
  //-------------------

  /** begin the execution
   * call HltBaseAlgo::beginExecute() (see HltBaseAlgo)
   * set decesion to false, set histo monitor bool flag
   *    increase input counter and histograms
   *    set filter and monitoring flags and decision in selection
   **/
  bool beginExecute();

  /** end of the execution
   *    increase output counters and histograms
   *    counts the candidates from output container
   *    set decision in the selection if there are candidates
   *             set decision to true
   **/ 
  bool endExecute();

  StatusCode baseExecute() 
  {return HltAlgorithm::execute();}

protected:

  // ---------------------------------
  //  Data Summary and Selection
  // ---------------------------------

  // register a selection without output container in data-summary
  void sumregister(const std::string& selname);

  /* set decision for gaudi (filterPass) and selection
   */
  virtual void setDecision(bool ok);

  /** set the decision type to true in selection (call setDecision)
   **/
  void setDecisionType(int decisionType); 

  /** get the selection summary of this algorithm 
   **/
  LHCb::HltSelectionSummary& selectionSummary()
  {return m_datasummary->selectionSummary(m_selectionID);}
  
  
  /** get any selection summary 
   *  note: this algorithm only modifies one selection summary, the others
   *        should be const!
   **/
  const LHCb::HltSelectionSummary& selectionSummary( int id)
  {return m_datasummary->selectionSummary(id);}

protected:

  std::string m_dataSummaryLocation;

  // pointer to the summary
  LHCb::HltSummary* m_datasummary;

  // name of the selection summary
  std::string m_selectionName;

  // id of the selection
  int m_selectionID;

  // minimun number of candidates
  int m_minNCandidates;

  // Is isTrigger a positive decision will be stored in the summary
  bool m_isTrigger;

protected:

  // ------------------------
  // Input/Output Containers
  // ------------------------

  // retrieve a container from the data summary
  template <class CONT>
  void sumretrieve(CONT*& con, const std::string& name) {
    con = 0; if (name.empty()) return;
    std::string::size_type s = name.rfind('/');
    std::string selname = (s != std::string::npos ? name.substr(s+1) : name );
    int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",selname);
    info() << " input selection " << selname << " id " << id << endreq;
    const LHCb::HltSelectionSummary& sum = selectionSummary(id);
    const std::vector<ContainedObject*>& sumdata = sum.data();
    if (sumdata.size() <=0) 
      fatal() <<" no data in selection " << name<< endreq;
    ContainedObject* pholder = (ContainedObject*) sumdata.front();
    Hlt::DataSizeHolder<CONT>* holder= 
      dynamic_cast< Hlt::DataSizeHolder<CONT>* >(pholder);
    if (!holder) fatal() << " no holder at selection " << selname << endreq;
    con = & (holder->object()); m_inputSelections.push_back(selname);
    info() << " retrieved data from data-summary " << selname << endreq;
  }

  // register a container to the data summary
  template <class CONT>
  void sumregister(CONT*& con, const std::string& name) {
    con = 0; if (name.empty()) return;
    if (m_outputHolder != 0) 
      fatal() << " A selection output already registed! " << endreq;
    std::string::size_type s = name.rfind('/');
    std::string selname = (s != std::string::npos ? name.substr(s+1) : name );
    sumregister(selname);
    if (!con) con = new CONT();
    Hlt::DataSizeHolder<CONT>* holder = new Hlt::DataSizeHolder<CONT>(*con);
    m_outputHolder = holder; selectionSummary().addData(*holder);
    info() << " registered data in data-summary " << m_selectionName << endreq;
  }

  // to register a container in the HltDataProvider (at initialization level)
  template <class CONT>
  void hltregister(CONT*& con, const std::string& name) {
    con = 0; if (name.empty()) return;
    std::string loca = "/Event/"+name;
    if (exist< Hlt::DataHolder<CONT> > (m_hltSvc,loca)) {
      fatal() << " output location already exist " << loca << endreq;
    } else {
      if (!con) con = new CONT();
      put(m_hltSvc, new Hlt::DataHolder<CONT>(*con),loca);
      info() << " located data at hlt " << loca << endreq;
    }
  }

  // retrieve a container from the HltDataProvider (at initialization level)
  template <class CONT>
  void hltretrieve(CONT*& con, const std::string& name) {
    con = 0; if (name.empty()) return;
    std::string loca = "/Event/"+name;
    if (! exist<Hlt::DataHolder<CONT> > (m_hltSvc,loca)) 
      fatal() << " input location does not exist " << loca << endreq;
    Hlt::DataHolder<CONT>* holder = get< Hlt::DataHolder<CONT> >(m_hltSvc,loca);
    if (!holder) error() << " not data holder at " << loca << endreq;
    con = &(holder->object());  
    info() << " retrieved data from hlt " << loca << endreq;
  }
  

protected:

  // hlt data provided service
  IDataProviderSvc* m_hltSvc;
  
  // names of the selection containers
  std::string m_inputTracksName;
  std::string m_inputTracks2Name;
  std::string m_inputVerticesName;
  std::string m_primaryVerticesName;

  std::string m_outputTracksName;
  std::string m_outputVerticesName;

  // pointers to the selection containers
  Hlt::TrackContainer* m_inputTracks;
  Hlt::TrackContainer* m_inputTracks2;
  Hlt::VertexContainer* m_inputVertices;
  Hlt::VertexContainer* m_primaryVertices;

  Hlt::TrackContainer* m_outputTracks;
  Hlt::VertexContainer* m_outputVertices;


protected:

  //-------------
  // Configuration
  //-------------

  // register in a value in the configuration with a key
  template <class T>
  void confregister(const std::string& key, const T& value) {
    std::string mykey = m_selectionName+"/"+key; 
    m_conf->add(mykey,value);
    info() << " HLT [" << mykey << "] = " << m_conf->template retrieve<T>(mykey) << endreq;    
  }

  template <class T>
  void printInfo(const std::string& title, const T& con) {
    typedef typename T::const_iterator iter;
    for (iter it = con.begin(); it != con.end(); ++it)
      printInfo(title,*(*it));  
  }
  
  void printInfo(const std::string& title, const LHCb::Track& track);

  void printInfo(const std::string& title, const LHCb::RecVertex& ver);

protected:

  // hlt configuration
  Hlt::Configuration* m_conf;

  std::vector<std::string> m_inputSelections;


  // Algorithms with 2 input track sources
  // m_consider1 m_consider2
  //    false       false   -> neither one can be empty
  //    false       true    -> 2nd source may be empty, but 1st cannot
  //    true                -> either one may be empty, but not at the same time 
  bool m_consider1;
  bool m_consider2;

  //------------------------
  // Histograms and Counters
  //------------------------

protected:


  // Counter of Input and Accepted Events
  HltCounter m_counterInput;
  HltCounter m_counterAccepted;

  // Input histograms for tracks and vertices
  HltHisto m_histoInputVertices;
  HltHisto m_histoInputPVs;
  HltHisto m_histoInputTracks;
  HltHisto m_histoInputTracks2;


  // Output histograms for track and vertices
  HltHisto m_histoCandidates;
  HltHisto m_histoOutputVertices;
  HltHisto m_histoOutputTracks;
  

  // size of the input containers
  int m_nInputTracks;
  int m_nInputTracks2;
  int m_nInputVertices;
  int m_nPrimaryVertices;  
  
  // size of the output containers
  int m_nOutputTracks;
  int m_nOutputVertices;

private:  

  // internal interface to compute size of the output selection objects
  Hlt::ISizeHolder* m_outputHolder;

};
#endif // HLTBASE_HLTALGORITHM_H
