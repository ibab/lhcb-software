#ifndef DimInfoMonRate_DimInfoMonRate_H
#define DimInfoMonRate_DimInfoMonRate_H 1


#include "dic.hxx"
#include "dis.hxx"
#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include <sstream>
#include <TStopwatch.h>


using namespace std;


#include "debugMacro.h"

//forward declaration
class RateExtractor;
class NOPExtractor;

class MonRate;
class TProfile;


/* useful typedefs to make code easier to read.
 */
 
/** Map associating a counter name to a RateExtractor.
  */
typedef std::map<int, RateExtractor *> ExtractorMap;

/** Map associating a counter name to a "value-comment' pair.
  * Used to get the MonRate counter map.
  */
typedef std::map<const std::string, std::pair<double*, std::string*>,
                 std::less<std::string> > MonRateMap;


/** A DimInfo based class subscribing to a MonRate service.
  * Uses RateExtractor objects to publish services included in the MonRate.
  *
  * @class DimInfoMonRate
  * @author Jean-Francois Menou
  */
class DimInfoMonRate : public DimInfo {
  
public : 
        /** Constructor.
	  *
	  * @param monRateSvcName Name of the DIM service to subscribe and handle.
	  * @param refreshTime DimInfo refreshTime parameter.
	  * @param source Name of the algorithm creating this instance.
	  */
	DimInfoMonRate(std::string monRateSvcName,
	                 int refreshTime,
			 std::string source);
	
	
	virtual ~DimInfoMonRate();
	
	
	/** Initialize the MonRate from the subscribed DIM service.
	  */
	void createMonRate();
	
	
	/** Updates the MonRate content from the subscribed DIM service.
	  */
	void loadMonRate();
	
	
	/** Callback on MonRate service changes.
	  */
	virtual void infoHandler();
	
	
	/** Get a pointer on the MonRate object receiving data from the DIM service.
	  *
	  * @return The pointer on the MonRate.
	  */
	MonRate * getMonRate(){ return m_monRate; }
	
	
	/** Checks if the MonRate ha been created.
	  *
	  * @return True if the MonRate has been created.
	  */
	bool isMonRateCreated(){ return m_monRateCreated; }
	
	
	/** Set the message service.
	  * 
	  * @param ims The message service interface to use.
	  */
	void setMsgService(IMessageSvc * ims){ m_msgSvc = ims; }
	
	
	/** Get the name of the DIM service handled by the instance.
	  *
	  * @return The service name.
	  */
	std::string getServiceName(){ return m_monRateServiceName; }
	
private :
	/** those 2 functions do stuff that MonRate should do for us...
	  */
	int getCycleNumberFromMonRate();
	
	longlong getTimeLastEventInCycleFromMonRate();


	/** Flag indicating if the MonRate object has correctly been
	  * initialized in the createMonRate() method.
	  */
	bool m_monRateCreated;
	
	
	/** The name of the MonRate service that is
	  * processed by the instance
	  */
	std::string m_monRateServiceName;
	
	
	/** Pointer on the MonObject actually
	  * owned by the DimInfoMonObject object
	  */
	MonRate * m_monRate;
	TProfile *m_profile;
	
	/** Indication on the last processed cycle.
	  * updated in extractDate() method.
	  */
	int m_currentCycleNumber;
	
	
	/** Pointer to the message service...
	  */
	IMessageSvc * m_msgSvc;
	
	
	/** Length of the last read DIM buffer for the subscribed service.
	  */
	int m_stringSize;


	/** Method processing the MonRate object : call the extractData() method
	  * for each rate contained in the MonRate.
	  *
	  */
	void extractData();
	
	
	/** Name of the algorithm owning this object...
	  */
	std::string m_sourceName;
		
	/** List of object in charge of extracting and republishing rates from MonRate.
	  */
	ExtractorMap m_extractorMap;
	
	NOPExtractor * m_pNOPExtractor;
	
	/** Looks for new counters (not beeing converted into rate by a RateExtractor object yet).
	  *
	  * @return The number of new counters found.
	  */
	int lookForNewRates();
	
	
	/** Create a header string (stored in serviceNameHeader) based on algorithm name and MonRate service name.
	  *
	  * @return The header.
	  */
	std::string makeServiceNameHeader();
	
	
	/** This string is to be used by RateExtractor object to publish their services.
	  * in a coherent way.
	  */	
	std::string serviceNameHeader;
	
};
#endif    // DimInfoMonRate_DimInfoMonRate_H
