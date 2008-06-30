// $Id: HltBase.h,v 1.12 2008-06-30 08:58:15 graven Exp $
#ifndef HLTBASE_HLTBASE_H 
#define HLTBASE_HLTBASE_H 1

// Include files

#include "GaudiAlg/GaudiHistoAlg.h"
#include "Kernel/IANNSvc.h"
#include "HltBase/IHltDataSvc.h"
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

  // finalize
  virtual StatusCode finalize();

protected:

  // initialize Histos via options
  void initializeHistosFromDescriptor();
  
  // initialize Histo (overwrite booking via options)
  Hlt::Histo* initializeHisto(const std::string& name,
                              double min = 0., double max = 100., 
                              int nBins = 100 );

  // fill histogram
  void fillHisto( Hlt::Histo& histo, double x, double weight );

  // fill histogram
  void fillHisto( Hlt::Histo& histo, const std::vector<double>& x, 
                  double weight );

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

  // register a value with a root/key into the hlt configuration
  template <class T>
  void confregister(const std::string& key, const T& value, 
                    const std::string& root = "") {
    std::string myroot = root;
    if (myroot.empty()) myroot = BASE::name();
    std::string mykey = myroot + "/" + key;
    hltConf().add(mykey,value);
    if (m_debug)
      BASE::debug() << " HLT [" << mykey << "] = " << value << endreq;    
  }

  IHltDataSvc& dataSvc() const;
  IANNSvc&     annSvc() const;

  // returns true if we use the TES to store the HltSelection
  bool useTES() {return m_TES;}

  // reset the hltData pointer (for the useTES() mode) 
  void resetHltData() { dataSvc().resetData(); }

  // returns the hlt configuration
  Hlt::Configuration& hltConf() { return dataSvc().config(); }
  
  // returns true if this selection name is valid
  bool validHltSelectionName(const stringKey& name);

  // returns the ID of the extraInfo by name
  int hltInfoID(const std::string& name);

private:
  // returns the ID of the extraInfo by name
  std::string hltInfoName(int id);

protected:

  // bool about the msg level
  bool m_verbose;
  bool m_debug;
  bool m_info;
  bool m_warning;
  bool m_error;
  bool m_fatal;

private:  
  // delegate constructor
  virtual void hltBaseConstructor();

private:
  // Property to rebook histogram from options
  SimpleProperty< std::map<std::string, Gaudi::Histo1DDef> > m_histoDescriptor;

  // hlt data provided service
  mutable IHltDataSvc* m_dataSvc;

  // assigned names and numbers service...
  mutable IANNSvc *m_hltANNSvc;

  // if selection selections will be stored/retrieved from TES
  bool m_TES;

};
#endif // HLTBASE_HLTBASE_H
