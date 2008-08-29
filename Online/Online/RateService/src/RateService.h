#ifndef RATESERVICE_RATESERVICE_H
#define RATESERVICE_RATESERVICE_H 1

// Include files

//#include "GaudiAlg/GaudiAlgorithm.h"
//#include "GaudiAlg/GaudiHistoAlg.h"

#include "GaudiKernel/Algorithm.h"
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


#include "dis.hxx"


/* deprecated
 */
static const std::string s_converterName("RATE_SERVICE_V2R1");

// Forward declarations
class DimInfoMonRate;


/** RateService is an Algorithm subscribing to MonRate services and publishing
  * structured services to PVSS.
  *
  * @class RateService
  * @author Jean-Francois Menou
  */
class RateService : public Algorithm {
public:
  /// Constructor of this form must be provided
  RateService(const std::string& name, ISvcLocator* pSvcLocator); 
  
  // Three mandatory member functions of any algorithm
  /// Initialize mandatory member functions of any algorithm
  StatusCode initialize();
  /// Execute mandatory member functions of any algorithm
  StatusCode execute();
  /// Finalize mandatory member functions of any algorithm
  StatusCode finalize();

private:

  /** utgid given to this algorithm.
    */
  std::string m_UTGID;

  /// ???
  std::string m_dimName;

  /** name of the service puclishing the MonRate object to be subscribed
    */
  std::string m_monRateServiceName;
  
  /** if the service has been found then don't ; if the service has been found then don't
    */
  bool m_found;
  
  /** Pointers to the DimInfoMonObject handling the MonRate services.
    */
  std::vector<DimInfoMonRate *> m_dimInfoMonRate;
  
  /// not used anymore. 
  time_t time_old,time_new;
  
  /// Time to sleep betwen 
  int sleepTime;
  
  RatePublisher * m_numberOfMonRatesPublisher;

  
  /** Checks if a MonRate service is already being processed by a DimInFoMonRate.
    *
    * @param serviceName The DIM service name to be checked.
    *
    * @return True if the service is already handled.
    */
  bool isHandledService(std::string serviceName);


  /** Discovers MonRate services
    *
    * @return A StatusCode.
    */
  StatusCode findServices();
};

#endif    // RATESERVICE_RATESERVICE_H
