#ifndef RATESERVICE_RATESERVICE_H
#define RATESERVICE_RATESERVICE_H 1

// Include files


#include "GaudiKernel/Service.h"
#include "GaudiKernel/Property.h"


#include "GaudiKernel/RndmGenerators.h"

#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>

#include "RatePublisher.h"

// boost
#include "boost/lexical_cast.hpp"
#include<time.h>


#include <dis.hxx>


/* deprecated
 */
static const std::string s_converterName("RATE_SERVICE_V2R3");

// Forward declarations
class DimInfoMonRate;


namespace LHCb {

/** @class RateService is a Service subscribing to MonRate services and publishing
  * structured services to PVSS.
  *
  * @class RateService
  * @author Jean-Francois Menou
  */
class RateService : public Service, public DimTimer {
public:
  /// Constructor of this form must be provided
  RateService(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~RateService();
  /// Initialize mandatory member functions of any service
  StatusCode initialize();

  /// Finalize mandatory member functions of any service
  StatusCode finalize();
  void timerHandler();
  /** Discovers MonRate services
    *
    * @return A StatusCode.
    */

  int checktime;
  int nbNewServices;
private:

  /** utgid given to this algorithm.
    */
  std::string m_UTGID;

  /// ???
  std::string m_dimName;

  /** name of the service puclishing the MonRate object to be subscribed
    */
  std::string m_monRateServiceName;
  
  /** option-defined number of counter per MonRate
    */
  int m_nbCounterInMonRate;
  
  /** if the service has been found then don't ; if the service has been found then don't
    */
  bool m_found;
  
  /** Pointers to the DimInfoMonObject handling the MonRate services.
    */
  std::vector<DimInfoMonRate *> m_dimInfoMonRate;
    
  /// partitionname
  std::string m_partitionName;
  /* RatePublisher to publish the number of MonRate services currently processed
   * by the RateService.
   */
  RatePublisher * m_numberOfMonRatesPublisher;

  
  /** Checks if a MonRate service is already being processed by a DimInFoMonRate.
    *
    * @param serviceName The DIM service name to be checked.
    *
    * @return True if the service is already handled.
    */
  bool isHandledService(std::string serviceName);

  StatusCode findServices();

};

} //end namespace LHCb
#endif    // RATESERVICE_RATESERVICE_H
