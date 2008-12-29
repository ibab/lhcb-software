// $Id: HltBase.h,v 1.20 2008-12-29 16:42:23 graven Exp $
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
 *       Access to HltSvc, Hlt::Data 
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

  IHltDataSvc& dataSvc() const;
  IANNSvc&     annSvc() const;


  // returns the ID of the extraInfo by name
  int hltInfoID(const std::string& name);

private:
  // returns the ID of the extraInfo by name
  std::string hltInfoName(int id);

  // delegate constructor
  virtual void hltBaseConstructor();

  // Property to book histogram from options
  SimpleProperty< std::map<std::string, Gaudi::Histo1DDef> > m_histoDescriptor;

  // hlt data provided service
  mutable IHltDataSvc* m_dataSvc;

  // assigned names and numbers service...
  mutable IANNSvc *m_hltANNSvc;

};
#endif // HLTBASE_HLTBASE_H
