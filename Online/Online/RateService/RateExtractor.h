#ifndef RATESERVICE_RATEEXTRACTOR_H_
#define RATESERVICE_RATEEXTRACTOR_H_


#include "dis.hxx"
#include "RatePublisher.h"
#include <string>


#include "debugMacro.h"

class MonRate;


/** Class extracting rate from a given counter in a MonRate object.
  * Publishes a pair of DIM services (rate value and rate comment).
  * 
  * @class RateExtractor 
  * @author Jean-Francois Menou    
  */
class RateExtractor : public RatePublisher
{
    
public:
  
  /** Constructor
    *
    * @param counter Identifier of the processed counter in the MonRate.
    * @param pMonRate Pointer to the MonRate owning the processed counter.
    */
  RateExtractor(int counterId, MonRate * pMonRate);
  
  
  /** Destructor
    */
  ~RateExtractor();
    
  /** Get the processed counter id.
    * @return The processed counter id.
    */
  int getCounterId(){ return m_counterId; }
    
  /** Extraction method, based on time of the new event.
    * 
    * @param time Time of the last event in the processed cycle.
    *
    * @return True if no error.
   */
  bool extractData(longlong time);
  
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
  
  double getCounterFromMonRate();
  std::string getCommentFromMonRate();
  long long getCycleLengthFromMonRate();
  
  /*====================================================*/
  /*======= MONRATE RELATED INFORMATION ================*/
  /*====================================================*/
  /** Identifier of the converted counter in the given MonRate object.
    */
  int m_counterId;
  
  /** Pointer to the MonRate object containing the counter to tranform.
    */
  MonRate * m_pMonRate;
  /*====================================================*/
  /*====================================================*/
      
    
  /*====================================================*/
  /*======= COUNTERS AND TIME ==========================*/
  /*====================================================*/
  /** Counter values got from MonRate, used to make counter's value comparisons
    */
  double m_counterOldValue;
  double m_counterNewValue;
  
  /** Time values got from MonRate, used to make time comparisons.
    */
  longlong m_oldTime;
  longlong m_newTime;
  /*====================================================*/
  /*====================================================*/
};

#endif


