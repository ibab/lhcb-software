// $Id: DimInfoCounter.cpp,v 1.1 2007-06-22 16:37:57 psomogyi Exp $
// Include files 

#include <iostream>

#ifdef WIN32
namespace win {
#include <windows.h>
}
# define mysleep win::Sleep
#else
# define mysleep usleep
#endif


#include <stdlib.h>

// DIM
#include "dic.hxx"

// local
#include "DimInfoCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DimInfoCounter
//
// 2006-08-07 : Ulrich Kerzel
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DimInfoCounter::DimInfoCounter(std::string serviceName,
                               int         refreshTime,
                               int         verbosity) :
  DimInfo(serviceName.c_str(), refreshTime, -1),
  m_counterType(DimInfoCounter::NotFound),
  m_serviceOK(false),
  m_serviceUpdated(false),
  m_intValue(0),
  m_floatValue(0.0),
  m_verbosity(verbosity)
{
  

  if (m_verbosity > 0) {    
    std::cout << "*** initialsing DIM counter for service " << serviceName << std::endl;
    std::cout << "    refresh time is                     " << refreshTime << std::endl;
  } //verbosity
  
  
  //
  // check if service exists
  //
  DimBrowser dimBrowser;
  int nServices = dimBrowser.getServices(serviceName.c_str());  
  if (nServices != 0) {
    
    // Server publishes the counter requested.
    m_serviceOK   = true;        

    // assume integer type by default, now check for other types 
    char *dimService; 
    char *dimFormat;
    int   dimType;
    while(dimType = dimBrowser.getNextService(dimService, dimFormat)) {      
      if (m_verbosity > 0){        
        std::cout << "DIM service : " << dimService << " format " << dimFormat << std::endl;      
      } //verbosity
      
      std::string stringFormat = dimFormat;
      if ( (stringFormat.find("I",0) != std::string::npos) || (stringFormat.find("L",0) != std::string::npos))
        m_counterType = DimInfoCounter::Integer;
      
      if  (stringFormat.find("F",0) != std::string::npos)
        m_counterType = DimInfoCounter::Float;

      if  (stringFormat.find("D",0) != std::string::npos)
        m_counterType = DimInfoCounter::Double;      

      if (stringFormat.find("C",0) != std::string::npos)
        m_counterType = DimInfoCounter::Character;

    } //while
  } //if nServices
  
  if (m_verbosity > 0) {
    std::cout << "counter status " << m_serviceOK   << std::endl;
    std::cout << "        type   " << m_counterType << std::endl;    
  } // if verbosity
  

  

} //constructor
//=============================================================================
// Destructor
//=============================================================================
DimInfoCounter::~DimInfoCounter() {} 

//=============================================================================
void DimInfoCounter::infoHandler()  {
  // call back: new data received

  if (m_verbosity > 0)
    std::cout << "DimInfoCounter::infoHandler service has been updated " << std::endl;
  m_serviceUpdated = true;

  if (m_counterType == DimInfoCounter::Integer) {
    m_intValue = getInt();
    if (m_verbosity > 1)
      std::cout << "Received new int counter   : " <<  m_intValue << std::endl;
  } else  
    if (m_counterType == DimInfoCounter::Float) {
      m_floatValue = getFloat();
      if (m_verbosity > 1)
        std::cout << "Received new float counter : " <<  m_floatValue << std::endl;
    } else
      if (m_counterType == DimInfoCounter::Double) {
        m_doubleValue = getDouble();
      if (m_verbosity > 1)
        std::cout << "Received new double counter : " <<  m_doubleValue << std::endl;        
      }// if m_counterType
  
} // constructor
//=============================================================================
int DimInfoCounter::getIntValue() {  
  return m_intValue;  
} // int getIntValue

//=============================================================================
float DimInfoCounter::getFloatValue() {  
  return  m_floatValue;  
} // int getFloatValue

//=============================================================================
double DimInfoCounter::getDoubleValue() {  
  return m_doubleValue;  
} // int getDoubleValue

//=============================================================================

bool DimInfoCounter::serviceOK () {
  return m_serviceOK;
}
//=============================================================================
DimInfoCounter::CounterType DimInfoCounter::getType() {
  return m_counterType;  
} // getCounterType
//=============================================================================
bool DimInfoCounter::serviceUpdated() {  
  return m_serviceUpdated;  
} // bool service updated
//=============================================================================
void DimInfoCounter::ResetServiceUpdated() {
  m_serviceUpdated = false;
  
  if (m_verbosity > 0)
    std::cout << "DimInfoHisto set flag serviceUpdated to "  << m_serviceUpdated 
              << std::endl;
  
} // void ResetServiceUpdated
