#ifndef RATESERVICE_RATEPUBLISHER_H_
#define RATESERVICE_RATEPUBLISHER_H_


#include "dis.hxx"
#include <string>


#include "debugMacro.h"

#define MAX_CAR 512

/* format of structured data sent by DIM
 */
static const std::string s_rateServiceFormat("D:1;C");

/** Structure storing data to be sent to PVSS.
  */
struct RateData
{
  /** Value of the rate.
    */
  double value;
  
  /** Comment associated to the rate.
    */
  char comment[MAX_CAR+1];
};    

int min(int, int);

/** Class publishing a value/comment pair in a structured DIM service for PVSS.
  * 
  * @class RatePublisher
  * @author Jean-Francois Menou    
  */
class RatePublisher
{

public:
  
  /** Constructor
    */
  RatePublisher();
  
  
  /** Destructor
    */
  virtual ~RatePublisher();
  
  void setValue(double value){ m_value = value; }
  void setComment(std::string comment){ m_comment = comment; }
  
  double getValue(){ return m_value; }
  std::string getComment(){ return m_comment; }
  
  /** Method publishing the structured service.
    * 
    * @param serviceName Name to give to the service.
    */
  virtual void publishService(std::string serviceName);
  
  /** Updates the service with current values
    */
  void updateService();
  
  /** Updates the service with given values and save them.
    *
    * @param value New value for the rate
    * @param comment New commetn for the rate
    */
  void updateService(double value, std::string comment);
  
  /** Updates the service with given value. Current comment is used.
    *
    * @param value New value for the rate
    */
  void updateService(double value);
  
  /** Calculates the size of the service to tranfsert the value and the comment.
    *
    * @return the size of the service.
    */
  int RatePublisher::sentDataSize();
  
  void printData();
    
  
private:

  /** Pointer to the publishded structured.
   */
  DimService * m_structuredService;
  
  /** The name given to the service to publish the rate.
   */
  std::string m_serviceName;
  
  /** Value of the rate
   */
  double m_value;
  
  /** Comment of the rate
   */
  std::string m_comment;
  
  /** Structure to store data when calling updateService or publishService.
   */
  RateData m_rateData;
  
  /** Sets value and comment in m_rateData after having cleaned it up.
    */
  void fillRateData();
  
  /** Cleans m_rateData up by deleting the comment.
    */
  void clearRateData();
};

#endif


