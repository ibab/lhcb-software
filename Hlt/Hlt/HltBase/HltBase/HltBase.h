// $Id: HltBase.h,v 1.7 2008-05-15 08:56:54 graven Exp $
#ifndef HLTBASE_HLTBASE_H 
#define HLTBASE_HLTBASE_H 1

// Include files

#include "GaudiAlg/GaudiHistoAlg.h"
#include "HltBase/IANNSvc.h"
#include "HltBase/HltTypes.h"

/** @class HltBase
 *  
 *  Provide services to HLT algorithms and tools
 *
 *  Functionality:
 *       Access to HltSvc, Hlt::Data and Hlt::Configuration
 *       Retrieve, register Hlt::Selections
 *       Histogram booking via options
 *       Counters 
 *
 *  Options
 *       HltDataLocation 
 *       HistogramDescriptor 
 *
 *  @author Hugo Ruiz Perez
 *  @author Jose Angel Hernando Morata
 *  @date   2006-06-15
 */

namespace Hlt {
  class  Counter {
  public:
    Counter () : m_histo(0),m_counter(0) {}
    virtual ~Counter(){}

    operator int () const { return m_counter;}
    const std::string& name() const { return m_name;}
  public:
    Hlt::Histo* m_histo;
    std::string m_name;
    long m_counter;
  };
}

template <class BASE>
class HltBase : public BASE {
public:

  // Algorithm constructor
  HltBase( const std::string& name, ISvcLocator* pSvcLocator );

  // Tool constructor
  HltBase ( const std::string& type   ,
            const std::string& name   ,
            const IInterface*  parent );
  
  // destructor
  virtual ~HltBase( ); ///< Destructor
  // initialize
  virtual StatusCode initialize();

  // execute
  virtual StatusCode execute() {return StatusCode::SUCCESS;}
  
  // finalize
  virtual StatusCode finalize();

private:  
  // delegate constructor
  virtual void create();

protected:

  // to fill or not the histograms
  bool m_monitor; 

private: 
  // Property to rebook histogram from options
  SimpleProperty< std::map<std::string, Gaudi::Histo1DDef> > m_histoDescriptor;

protected:

  // initialize Histos via options
  void initializeHistosFromDescriptor();
  
  // initialize Histo (overwrite booking via options)
  Hlt::Histo* initializeHisto(const std::string& name,
                              float min = 0., float max = 100., 
                              int nBins = 100 );

  // fill histogram
  void fillHisto( Hlt::Histo& histo, float x, float weight );

  // fill histogram
  void fillHisto( Hlt::Histo& histo, const std::vector<double>& x, 
                  float weight );

protected:

  // initialize counter
  void initializeCounter( Hlt::Counter& counter, 
                          const std::string& name );
  
  // increase counter
  void increaseCounter( Hlt::Counter& counter, int increase = 1);
  
  // print info of total counter
  void infoTotalEvents(int  n);
  
  // print info of fraction
  void infoSubsetEvents( int nEventsInSubset, int  nTotEvts, 
                         const std::string& comment );

  // print average number of candidates
  void infoCandidates( int nTotCandidates, int  nTotEvts, 
                       const std::string& comment );

protected:

  // initialize the Msg service (set internal bool variables m_debug, etc)
  void initializeMsg();

  // print info of this container
  template <class CONT>
  void printInfo(const std::string& title, const CONT& cont) {
    BASE::info() << title << cont.size() << endreq;
    typedef typename CONT::const_iterator iter;
    for (iter it = cont.begin(); it != cont.end(); ++it)
      printInfo(title,*(*it));  
  }
  
  // print info from track
  void printInfo(const std::string& title, const LHCb::Track& track);

  // print info from vertex
  void printInfo(const std::string& title, const LHCb::RecVertex& ver);

  // print info from extraInfo
  void printInfo(const std::string& title,
                 const GaudiUtils::VectorMap<int,double>& info);

protected:

  // bool about the msg level
  bool m_verbose;
  bool m_debug;
  bool m_info;
  bool m_warning;
  bool m_error;
  bool m_fatal;

  
protected:
  
  // register a value with a root/key into the hlt configuration
  template <class T>
  void confregister(const std::string& key, const T& value, 
                    const std::string& root = "") {
    std::string myroot = root;
    if (myroot.empty()) myroot = BASE::name();
    std::string mykey = myroot + "/" + key;
    hltConf().add(mykey,value);
    if (m_debug)
      BASE::info() << " HLT [" << mykey << "] = " << value << endreq;    
  }

protected:

  // returns the hlt service
  IDataProviderSvc& hltSvc() const;
  IANNSvc&          annSvc() const;

  // returns true if we use the TES to store the HltSelection
  bool useTES() {return m_TES;}

  // reset the hltData pointer (for the useTES() mode) 
  void resetHltData() {m_hltData = 0;}

  // returns the Hlt::Data (container of all Hlt::Selections)
  Hlt::Data& hltData();

  // returns the hlt configuration
  Hlt::Configuration& hltConf();
  
  // returns true if this selection name is valid
  bool validHltSelectionName(const stringKey& name);

  // returns the ID of this selection name
  int hltSelectionID(const stringKey& name);

  // returns the selection name of this ID
  stringKey hltSelectionName(int id);

  // returns the ID of the extraInfo by name
  int hltInfoID(const std::string& name);

  // returns the ID of the extraInfo by name
  std::string hltInfoName(int id);

  // return the location of the data
  const std::string& hltDataLocation() 
  {return m_hltDataLocation;}

  // return the location of the data
  const std::string& hltConfigurationLocation() 
  {return m_hltConfigurationLocation;}

private:

  // hlt data provided service
  mutable IDataProviderSvc* m_hltSvc;

  // assigned names and numbers service...
  mutable IANNSvc *m_hltANNSvc;

  // if selection selections will be stored/retrieved from TES
  bool m_TES;

  // location of the data summary
  std::string m_hltDataLocation;

  // pointer to the hlt data
  Hlt::Data* m_hltData;

  // location of the data summary
  std::string m_hltConfigurationLocation;
  
  // hlt configuration
  Hlt::Configuration* m_hltConf;

};
#endif // HLTBASE_HLTBASE_H
