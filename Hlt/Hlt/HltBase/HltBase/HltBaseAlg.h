// $Id: HltBaseAlg.h,v 1.3 2007-02-12 09:53:19 cattanem Exp $
#ifndef HLTBASE_HLTBASEALG_H 
#define HLTBASE_HLTBASEALG_H 1

// Include files

#include "GaudiAlg/GaudiTupleAlg.h"

class Condition;

/** @class HltBaseAlg 
 *  
 *  Base class for HLT algorithms adding extra functionality from Gaudi:
 *
 *  functionality:
 *       Histogram booking via options
 *       Filling histogram in a given period
 *       Counters of input, accepted events
 *
 *  Options:
 *       PassPeriod: period to always accept/pass the algorithm
 *       HistogramUpdatePeriod: period to fill histograms
 *       ConditionsName: path to the conditions of the derived algorithm
 *       HistoDescriptor: a list of strings to book histograms
 *              i.e MyAlgo.HistoDescriptor = {"PT,100,0.,5000."};
 *
 *  @author Hugo Ruiz Perez
 *  @author Jose Angel Hernando Morata
 *  @date   2006-06-15
 */

typedef GaudiHistoAlg DefAlgo;

class HltBaseAlg : public DefAlgo {
public:

  /// Standard constructor
  HltBaseAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltBaseAlg( ); ///< Destructor

  /** initialize algorithm
   *     initialize message level, generic counters, and conditions
   **/
  virtual StatusCode initialize();

  /** execute algorithm
   *  Note: call HltBaseAlg::beginExecute(), HltBaseAlg::endExecute() 
   * at the begin and end of your algorithm
   **/
  virtual StatusCode execute   ();

  /** finalize algorithm
   *  print info of passed/accepted events
   **/
  virtual StatusCode finalize  ();

protected:

  /** begin the execution
   * always call it at the begining of your execution method
   * increase counters, set monitor and filter pass according with the
   * PassPeriod and HitogramUpdatePeriod options
   * set decision (pass algo) to false (except if the filter pass flag is on,
   *                                    in that case it is set to true)
   **/
  virtual bool beginExecute();
  
  /** end of the execution
   * always call at the end of your execution method
   * increase counter, and set decsion (pass) to true
   **/
  virtual bool endExecute();

protected:

  // check that this a required container it is not null
  template <class CON>
  inline void checkInput(CON*& cont, const std::string& comment) {
    if (cont == NULL) error() << comment << endreq;
  }

  // get objects from the TES and catch exception
  template <class T>
  bool retrieve(T*& t, const std::string& name) {
    t = NULL;
    try {t  = get<T>(name);}
    catch (GaudiException) {warning() << " No obj at " << name << endreq;}
    return (t != NULL);
  }

  // stop the execute method, print a message or send a error
  inline StatusCode stop(const std::string& message, bool err = false)
  {
    if (err) error() << message << endreq;
    debug() << message << endreq;
    return err? StatusCode::FAILURE: StatusCode::SUCCESS;
  }

protected:

  // typedef to histogram
  typedef AIDA::IHistogram1D* HltHisto;

  // Internal counter class
  // TODO: to be revisit, we should use histogram instead
  class HltCounter {
  public:
    HltCounter () {m_histo = NULL; m_counter = 0; m_name="";}
    // Return counter value if treated as int
    operator int () const { return m_counter;}
    friend MsgStream& operator << ( MsgStream& o, HltCounter& counter ) 
    {o << counter.m_histo; return o; }
  public:
    std::string m_name;
    long m_counter;
    HltHisto m_histo;
  };

  // // Histo collection class
  //   // TODO: how we use it?
  //   class HistoCol 
  //   {
  //   public:
  //     std::string m_name;
  //     std::vector<std::string> m_variables;
  //     std::vector<std::string> m_cases;
  //     std::vector<AIDA::IHistogram1D*> m_histos;
  //     bool m_initialized;
  //     unsigned m_nVariables;
  //     unsigned m_nCases;
  //     HistoCol ()  { m_initialized = false ; };
  //  };
  // initialize histograms
  //   void initializeHistoCol( HistoCol& histoCol,
  //                            const std::string& inputString,
  //                            const std::vector<std::string>& inputVariables,
  //                            const std::vector<std::string>& inputCases );
  //  // fill collection of histogram
  //   void fillHistoCol( HistoCol& histoCol, 
  //                      const std::vector<bool>& inputBools,
  //                      const std::vector<float>& inputVariables, 
  //                      float weight);


  /** initialize Histos from descritor
   *  histograms will be book using the HistoDescriptor option
   **/
  void initializeHistosFromDescriptor();
  
  /** initialize and book this histogram
   *  the booking can be changed via options using the HisDescriptor
   **/
  void initializeHisto( HltHisto& histo, const std::string& name,
                        float min = 0., float max = 100., int nBins = 100 );
  
  /** fill histogram with this wait
   *  it will be filled only when the monitor flag is on 
   *  this flag it is controlled by the HitogramUpdatePeriod option
   **/
  void fillHisto( HltHisto& histo, float x, float weight );


  /** initialize counter
   * TODO: revisit
  **/
  void initializeCounter( HltCounter& counter, const std::string& name );
  
  /// increase counter
  void increaseCounter( HltCounter& counter, int increase = 1);
  
  
  // Formated text messages
  void infoTotalEvents    ( int  nTotEvts);
  
  // Formated text messages, print also fraction of events
  void infoSubsetEvents   ( int nEventsInSubset, int  nTotEvts, 
                            const std::string& subsetName );

  // Formated text messages, print also fraction of events  
  void infoInputObjects   ( int nInputObjects, int  nTotEvts, 
                            const std::string& objectsName  );  

  // Formated text messages, print also fraction of events
  void infoAcceptedObjects( int nAcceptedObjects, int  nTotEvts, 
                            const std::string& objectsName );

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
  
  // fill histogram with the size of the container 
  template <class CON>
  bool size(CON*& con, int& n, HltHisto& h, const std::string& comment) {
    if (!con) return true;
    n = con->size(); fillHisto(h,n,1.);
    debug() << comment << n << endreq;
    if (n == 0) setDecision(false);
    return (n>0);
  }

  /** fill the histogram with the info stored in the objects of the container
   * intended to do internal monitoring, before we apply any cut
  **/
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

  // Counters of events
  HltCounter m_counterEntries;
  HltCounter m_counterOverruled;
  HltCounter m_counterPassed;

protected:

  // Minimum number of candidates needed to set filterPassed to true
  long m_nCandidates;

  // increasing the number of candidates
  void candidateFound() { m_nCandidates++; };

  // The period to always return a positive decision
  int m_passPeriod;

  // always force a positive decision of the algorithms 
  // (controlled by PassPeriod option)
  bool m_filter;

  // monitor this event (depends on HistogramUpdatePeriod option)
  bool m_monitor;

  // setting the decision, set FilterPass to inform the Gaudi sequencer
  inline void setDecision(bool ok) {
    if (m_filter) setFilterPassed(ok);
    debug() << " set decision " << ok << endreq;
  }

protected:

  // initialize the Msg service
  void initMsg();
  
  // initialize the conditions
  bool initConditions();

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

  // Property to rebook histogram from options
  StringArrayProperty m_histoDescriptor;

  // the period to fill histograms
  int m_histogramUpdatePeriod;

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
#endif // HLTBASE_HLTBASEALG_H
