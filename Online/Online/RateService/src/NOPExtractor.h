#ifndef RATESERVICE_NOPEXTRACTOR_H_
#define RATESERVICE_NOPEXTRACTOR_H_


#include "dis.hxx"
#include "RatePublisher.h"
#include <string>

class MonRate;

/* format of structured data sent by DIM
 */
static const std::string s_NOPServiceNameEnd("NUMBER_OF_PROCESSES");

/** Class extracting rate from a given counter in a MonRate object.
  * Publishes a pair of DIM services (rate value and rate comment).
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
        
  /** Extraction method, based on time of the new event.
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
    * @param nameHeader Header to add before the counter name.
    *
    * @return The service name.  
    */
  std::string makeServiceName(std::string nameHeader);
  
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


