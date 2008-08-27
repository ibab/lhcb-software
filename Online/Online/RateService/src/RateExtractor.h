#ifndef RATESERVICE_RATEEXTRACTOR_H_
#define RATESERVICE_RATEEXTRACTOR_H_


#include "dis.hxx"
#include <string>

class MonRate;


/* standard label to identify rate services.
 * "rate services" comment service should begin with this label.
 */
static const std::string s_rateServiceCommentLabel("[rate]");

/* format of structured data sent by DIM
 */
static const std::string s_rateServiceFormat("D:1;C");

/* maximum size of a rate comment
 * 128 => 127 (max) characters + 1 null character
 */
static const int s_maxRateCommentSize = 128;


/** Class extracting rate from a given counter in a MonRate object.
  * Publishes a pair of DIM services (rate value and rate comment).
  * 
  * @class RateExtractor 
  * @author Jean-Francois Menou    
  */
class RateExtractor
{

  /** Structure storing data to be sent to PVSS.
    */
  struct RateExtractorData
  {
    /** Value of the rate.
      */
    double value;
    
    /** Comment associated to the rate.
      * The size is fixed and too long comment will be truncated.
      */
    char comment[s_maxRateCommentSize];
  };    
    
public:
  
  /** Constructor
    *
    * @param rateId Identifier of the processed counter in the MonRate.
    * @param pMonRate Pointer to the MonRate owning the processed counter.
    */
  RateExtractor(std::string rateId, MonRate * pMonRate);
  
  
  /** Destructor
    */
  ~RateExtractor();
  
  
  /** Get a reference on the value container.
    * @return Reference on m_data.value.
    */
  double & getRateValue(){ return m_data.value; }
  
  /** Get the current comment.
    * @return A pointer on the current comment.
    */
  char *   getRateComment(){ return m_data.comment; }
  
  /** Get the processed counter name.
    * @return The processed counter name.
    */
  std::string getRateId(){ return m_rateId; }
  
  
  /** Extraction method, based on time of the new event.
    * 
    * @param time Time of the last event in the processed cycle.
    *
    * @return True if no error.
   */
  bool     extractData(longlong time);
  
  
  /** Method publishing the structured service.
    * 
    * @param nameHeader Header of the complete service name (header+counter name).
    */
  void publishServices(std::string nameHeader);
  
private:

  //std::string makeValueServiceName(std::string nameHeader);
  //std::string makeCommentServiceName(std::string nameHeader);
  
  
  /** Trivial method to create the name of the services.
    *
    * @param nameHeader Header to add before the counter name.
    *
    * @return The service name.  
    */
  std::string makeServiceName(std::string nameHeader);
  
  
  /** Method formating the comment.
    * @deprecated Comment does not contain label to recognize rate services.
    */
  void reshapeComment();
  
  /*====================================================*/
  /*======= DIM SERVICES ===============================*/
  /*====================================================*/
  /* pointer to published services
   *
  DimService * m_valueService;
  DimService * m_commentService;*/
  
  /** Pointer to the publishded structured.
   */
  DimService * m_structuredService;
  /*====================================================*/
  /*====================================================*/
  
  
  
  
  /*====================================================*/
  /*======= MONRATE RELATED INFORMATION ================*/
  /*====================================================*/
  /** Identifier of the converted counter in the given MonRate object.
    */
  std::string m_rateId;
  
  /** Pointer to the MonRate object containing the counter to tranform.
    */
  MonRate * m_pMonRate;
  /*====================================================*/
  /*====================================================*/
  
  
  
  
  /*====================================================*/
  /*======= SENT DATA AND RELATED ======================*/
  /*====================================================*/
  /** Data extracted of the MonRate counter and stored to be sent by DIM.
    */
  RateExtractorData m_data;
  
  /** Size of the comment including null character.
    * Max size defined by s_maxRateCommentSize
    */
  int m_commentSize;
  
  /** Calculate the size of the data (value+comment) to send.
    *
    * @return Size of the data to send.
    */
  int sentDataSize();
   
  // /* calculated rate value
  //  *
  // double m_rateValue;*/
  // 
  // /* comment of the processed counter.
  //  *
  // std::string m_comment;*/
  
  // /* char * version for publishing
  //  *
  // char * m_cComment;*/
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


