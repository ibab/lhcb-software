#ifndef RATESERVICE_NOPEXTRACTOR_H_
#define RATESERVICE_NOPEXTRACTOR_H_


#include "dis.hxx"
#include "RatePublisher.h"
#include <string>



#include "debugMacro.h"

class MonRate;

/* ...
 */
static const std::string s_NOPServiceNameEnd("NUMBER_OF_PROCESSES");

/** Class extracting the number of processes using a given MonRate
  * and publishing it to PVSS.
  * 
  * @class NOPExtractor 
  * @author Jean-Francois Menou    
  */
class NOPExtractor : public RatePublisher
{
    
public:
  
  /** Constructor
    *
    * @param counter Identifier of the processed counter in the MonRate.
    * @param pMonRate Pointer to the MonRate owning the processed counter.
    */
  NOPExtractor(MonRate * pMonRate);
  
  
  /** Destructor
    */
  ~NOPExtractor();
        
  /** Method extracting the num. of proc. and publishing it.
    * 
    * @param time Time of the last event in the processed cycle.
    *
    * @return True if no error.
   */
  bool     extractData();
  
  /** Method publishing the structured service.
    * 
    * @param nameHeader Header of the complete service name (header+counter name).
    */
  void publishService(std::string nameHeader);
  
private:

  /** Trivial method to create the name of the services.
    *
    * @param nameHeader Header to add before the service name.
    *
    * @return The service name.  
    */
  std::string makeServiceName(std::string nameHeader);
  
  /** Extracts the number of processes from th e given MonRate.
    */
  int getNOPFromMonRate();
  
  /*====================================================*/
  /*======= MONRATE RELATED INFORMATION ================*/
  /*====================================================*/  
  /** Pointer to the MonRate object containing the counter to tranform.
    */
  MonRate * m_pMonRate;
  /*====================================================*/
  /*====================================================*/
};

#endif


