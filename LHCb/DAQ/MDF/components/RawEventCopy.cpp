// $Id: RawEventCopy.cpp,v 1.2 2009-05-25 08:56:21 cattanem Exp $
// Include files from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "MDF/RawEventHelpers.h"
#include "Event/RawEvent.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {
class RawEvent;

  /** @class RawEventCopy RawEventCopy.cpp
    *
    *  Creates and copies RawEvent data structure to new location
    *  in the data store.
    *
    *  @author Markus Frank
    *  @date   2005-10-13
    */
  class RawEventCopy : public GaudiAlgorithm {
    /// Property: source leaf for rawevent copy
    std::string m_source;
    /// Property: target leaf name for rawevent copy
    std::string m_destination;

  public: 
    /// Standard constructor
    RawEventCopy(const std::string& nam, ISvcLocator* pSvc) 
    : GaudiAlgorithm(nam,pSvc) {
      declareProperty("Destination",m_destination="/Event/DAQ/RawCopy");
      declareProperty("Source",     m_source=RawEventLocation::Default);
    }
    /// Destructor
    virtual ~RawEventCopy()  {} 
    /// Main execution
    virtual StatusCode execute()  {
      StatusCode sc = StatusCode::FAILURE;
      if( exist<RawEvent>( m_source ) ) {
        RawEvent *org = get<RawEvent>( m_source );
        RawEvent *res = 0;
        sc = cloneRawEvent(org, res);
        if ( sc.isSuccess() )  {
          sc = eventSvc()->registerObject(m_destination, res);
          if ( !sc.isSuccess() )  {
            delete res;
          }
        }
      }
      return sc;
    }
  };
}

#include "GaudiKernel/AlgFactory.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RawEventCopy)
