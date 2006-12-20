// $Id: HltBaseAlg.h,v 1.1 2006-12-20 09:32:44 hernando Exp $
#ifndef HLTBASE_HLTBASEALG_H 
#define HLTBASE_HLTBASEALG_H 1

// Include files

#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "DetDesc/Condition.h"


/** @class HltBaseAlg 
 *  
 *  Base class for HLT algorithms
 *  extra functionality from Gaudi:
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

  virtual StatusCode initialize();
  virtual StatusCode execute   ();
  virtual StatusCode finalize  ();

protected:

  virtual bool beginExecute();
  
  virtual bool endExecute();

protected:

  //-----------------------
  // stop and error methods
  //-----------------------

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

  typedef AIDA::IHistogram1D* HltHisto;

  // Counter class
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


  // initialize Histos from descritor
  void initializeHistosFromDescriptor();
  
  // initialize and book this histogram
  // the booking can be changed via options using the HisDescriptor
  void initializeHisto( HltHisto& histo, const std::string& name,
                        float min = 0., float max = 100., int nBins = 100 );
  
  // fill histogram with this wait
  // it will be filled with the period set by options
  void fillHisto( HltHisto& histo, float x, float weight );


  // initialize counter
  // TODO: revisit
  void initializeCounter( HltCounter& counter, const std::string& name );
  
  // increase counter
  void increaseCounter( HltCounter& counter, int increase = 1);
  
  
  // Formated text messages
  void infoTotalEvents    ( int  nTotEvts);
  
  // Formated text messages, print also fraction of events
  void infoSubsetEvents   ( int nEventsInSubset, int  nTotEvts, 
                            const std::string& subsetName );
  
  void infoInputObjects   ( int nInputObjects, int  nTotEvts, 
                            const std::string& objectsName  );  

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

  HltCounter m_counterEntries;
  HltCounter m_counterOverruled;
  HltCounter m_counterPassed;

protected:

  // Minimum number of candidates needed to set filterPassed to true
  long m_nCandidates;

  void candidateFound() { m_nCandidates++; };

  // The period to always return a positive decision
  int m_passPeriod;

  // always force a positive decision of the algorithms 
  // (controlled by options)
  bool m_filter;

  // monitor this event (depends on a period set by options)
  bool m_monitor;

  // setting the decision, inform Gaudi about the decision
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
