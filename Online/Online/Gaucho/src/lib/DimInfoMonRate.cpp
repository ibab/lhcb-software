//====================================================================
//  DimInfoMonRate.cpp
//--------------------------------------------------------------------
//
//  Package    : 
//
//  Description: 
//
//  Author     : 
//  Created    : 
//  Changes    : 
//
//====================================================================
// Include files
#include "Gaucho/DimInfoMonRate.h"
#include "Gaucho/RateExtractor.h"
#include "Gaucho/NOPExtractor.h"
#include "Gaucho/MonRate.h"
#include "Gaucho/MonObjectCreator.h"
#include <string>
#include <iostream>
#include <math.h>

/* Trivial function to print a char * string.
 * Prints '.' for non printable characters.
 */
void printChar(char * c, int size)
{
  int i=0;
  
 // cout << "content (" << size << " bytes):" << endl;
  
  while(i < size && c != 0)
  {
    if( c[i] >= 33 && c[i] <= 126)
    {
   //   cout << c[i];
    }
    else
    {
   //   cout << ".";
    }
    
    i++;
  }
  
 // cout << endl << "/content" << endl;
}

//--------------------------------------------------------------------
// DimInfoMonRate class
//--------------------------------------------------------------------

DimInfoMonRate::DimInfoMonRate(std::string monRateSvcName,
                                   int refreshTime,
				   std::string source,
				   int nbCounterInMonRate)
    /* it actually subscribe to a char array service
     * (MonObject are serialized and published as
     * char array)
     * so why is the float version use instead ???
     --------------------------------------
     Juan Replay --> because it is the wrong return value when it doesn get a valid value;
     --------------------------------------
     */    
    : DimInfo(monRateSvcName.c_str(),
              refreshTime,
	      (char*)s_noValidMonRate.c_str()),
      m_monRateCreated(false),
      m_monRateServiceName(monRateSvcName),
      m_monRate(0),
      m_profile(0),
      m_currentCycleNumber(0),
      m_msgSvc(0),
      m_stringSize(-1),
      m_sourceName(source),
      m_pNOPExtractor(0),
      m_nbCounterInMonRate(nbCounterInMonRate)
{
  //COUT_DEBUG("m_doubleServiceName = " << m_doubleServiceName)
}

int DimInfoMonRate::getCycleNumberFromMonRate()
{
  return (int)m_profile->GetBinContent(6);
}

longlong DimInfoMonRate::getTimeLastEventInCycleFromMonRate()
{
  //COUT_DEBUG("getTimeLastEventInCycleFromMonRate");
  longlong time = (longlong)m_profile->GetBinContent(3);
  //COUT_DEBUG("getTimeLastEventInCycleFromMonRate DONE");
  
  return time;
}

void DimInfoMonRate::createMonRate()
{
  //COUT_DEBUG("createMonRate");
  m_monRateCreated = false;
  int tmpStringSize = -1;
  tmpStringSize = getSize()/sizeof(char);
  
  char * c;
  while (true) {
    while ( tmpStringSize <= 0 ) {
      //gSystem->Sleep(m_waitTime);
      usleep(10000);
      tmpStringSize = getSize()/sizeof(char);
    //  std::cout << "size for service "<< m_monRateServiceName << " " << tmpStringSize << std::endl;
    }

    c = const_cast<char *>((const char *)getString());
  
    if ( 0 == s_noValidMonRate.compare(c)) {
     // std::cout << MSG::WARNING << s_noValidMonRate << ", for service "<< m_monRateServiceName << std::endl; 
      tmpStringSize = -1;
      usleep(10000);
      continue;
    }
    
    break;
  }
  
  m_stringSize = tmpStringSize;
  //printChar(c, m_stringSize);
  
  /* test phase to ensure we have a valid MonRate
   */
   
  MonObject * monObjectBaseTEST = new MonObject(m_msgSvc, m_sourceName);
  
  std::stringstream isTEST;

  isTEST.rdbuf()->pubsetbuf(c, m_stringSize);
  
  boost::archive::binary_iarchive * iaTEST = new boost::archive::binary_iarchive(isTEST);

  try{
    monObjectBaseTEST->load(*iaTEST, 0);
  }catch(const std::exception & e){
  //  COUT_DEBUG("first load raised exception");
  //  COUT_DEBUG(e.what());
    printChar(c, m_stringSize);
    
    delete iaTEST;
    delete monObjectBaseTEST;
    m_monRateCreated = false;
    throw e;
  }
  
  std::string monObjectTypeNameTEST  = monObjectBaseTEST->typeName();
  
  delete monObjectBaseTEST;
  delete iaTEST;
  
  if(monObjectTypeNameTEST != s_monRate)
  {
   // COUT_DEBUG("error: type = " << monObjectTypeNameTEST);
    m_monRateCreated = false;
    return;
  }
  
  if(m_monRate)
  {
   // COUT_DEBUG("deleting previous object");
    delete m_monRate;
    m_monRate = 0;
  }
  
  m_monRate = (MonRate*)MonObjectCreator::createMonObject(s_monRate, m_msgSvc, m_sourceName);
  if( !m_monRate )
  {
    m_monRateCreated = false;
    return;
  }

  std::stringstream is;
  is.rdbuf()->pubsetbuf(c, m_stringSize);
  boost::archive::binary_iarchive * ia = new boost::archive::binary_iarchive(is);
  
  //COUT_DEBUG("load");
  m_monRateCreated = true;
  
  try{
    m_monRate->load(*ia, 0);
    makeServiceNameHeader();
  }catch(const std::exception & e)
  {
    m_monRateCreated = false;
  //  COUT_DEBUG("exception in createMonRate");
  //  COUT_DEBUG(e.what());
    
    delete m_monRate;
    m_monRate = 0;
    m_monRateCreated = false;
//    if(ia) delete ia;
    
    throw e;
  }
  
  //COUT_DEBUG("createMonRate DONE");
  
//  if(ia) delete ia;
  
  //COUT_DEBUG("gen. header");
  
}

void DimInfoMonRate::loadMonRate()
{
 // COUT_DEBUG("loadMonRate");
  int tmpStringSize = -1;
  
  char * c;
  while (true) {
    while ( tmpStringSize <= 0 ) {
      //gSystem->Sleep(m_waitTime);
      usleep(10000);
      tmpStringSize = getSize()/sizeof(char);
      //std::cout << "size for service "<< m_monRateServiceName << " " << tmpStringSize << std::endl;
    }

    c = const_cast<char *>((const char *)getString());
  
    if ( 0 == s_noValidMonRate.compare(c)) {
     // std::cout << MSG::WARNING << s_noValidMonRate << ", for service "<< m_monRateServiceName << std::endl; 
      tmpStringSize = -1;
      usleep(10000);
      continue;
    }
    
    break;
  }


  m_stringSize = tmpStringSize;

  //printChar(c, m_stringSize);
  
  /* just reset differs from createMonDouble
   */
  m_monRate->reset();
  
  std::stringstream is;
  is.rdbuf()->pubsetbuf(c, m_stringSize);
  boost::archive::binary_iarchive * ia = new boost::archive::binary_iarchive(is);
  m_monRate->load(*ia, 0);
  
  m_profile = m_monRate->profile();
  
  if(ia) delete ia; 
  
 // COUT_DEBUG("loadMonRate DONE");
}

//destructor
DimInfoMonRate::~DimInfoMonRate()
{
  for(ExtractorMap::iterator it = m_extractorMap.begin();
      it != m_extractorMap.end();
      it++)
  {
  //  COUT_DEBUG("deleting extractor for counter " << it->first);
    if(it->second) delete it->second;
  }
    
 // COUT_DEBUG("deleting MonRate");
  if(m_monRate)
  {
    m_monRate->deleteProfile();
    delete m_monRate;  
  //  COUT_DEBUG("MonRate deleted");
  }
  
  if (m_pNOPExtractor)
  {
    delete m_pNOPExtractor;  
  //  COUT_DEBUG("NOPExtractor deleted");
  }
  
}

/* called whenever the MonDouble service changes.
 * extract the double data to update the C-double
 * service.
 * so the C-double service will be updated sometime.
 */
void DimInfoMonRate::infoHandler() 
{
 // COUT_DEBUG("infoHandler");
  if (!m_monRateCreated) return;
  try
  {
    //COUT_DEBUG("void DimInfoMonRate::infoHandler() ")
  
    /* updating the MonDouble from the service
     */
    if(m_monRate == 0)
    {
      /* if the MonRate is not created then do nothing.
         it's up to the user to call createDouble().
       */
   //   COUT_DEBUG("infoHandler   MonRate has not been created (createMonRate not called yet)");
    }else{    
      /* else : reload it
       */
      loadMonRate();
          
    //  COUT_DEBUG("infoHandler   MonRate loaded");
      
      if(!m_pNOPExtractor)
      {
      //  COUT_DEBUG("infoHandler   NOP extractor not created yet");
        m_pNOPExtractor = new NOPExtractor(m_monRate);
	m_pNOPExtractor->publishService(serviceNameHeader);
      //  COUT_DEBUG("infoHandler   NOP has been created");
      }

      
     // m_monRate->print();
    }
    

    /* updating the double value from the MonDouble
     */
    extractData();
  }
  catch(const std::exception & e/*boost::archive::archive_exception exception*/)
  {
    /*std::cout << exception.what() << endl;*/
   // COUT_DEBUG(e.what());
   // COUT_DEBUG("exception in infoHandler : " << m_monRateServiceName);
  }
  
 // COUT_DEBUG("infoHandler DONE");
}


/* stores the double value contained in the MonDouble
 * into the "equivalent" variable.
 * and updates the comment by the way.
 */
void DimInfoMonRate::extractData()
{
 // COUT_DEBUG("extractData");
  if(m_monRate)
  {
    //COUT_DEBUG("extractData   m_monRate != 0");
    
    if(m_pNOPExtractor)
    {
      m_pNOPExtractor->extractData();
   //   COUT_DEBUG("extractData   NOP extracted");
    }else{
  //    COUT_DEBUG("extractData   NOP doesn not exist !!!!");
    }
    
    //COUT_DEBUG("extractData    <=================");
    int newRates = lookForNewRates();
    
    if(newRates != 0)
       COUT_DEBUG("extractData    new = " << newRates << "<=================");
    
    //COUT_DEBUG("extractData   new counters search done");
    
    int gotCycleNumber = getCycleNumberFromMonRate();
    int nbElapsedCycles = gotCycleNumber - m_currentCycleNumber;
    
    //COUT_DEBUG("extractData   got cycle number ===> " << gotCycleNumber);
    
    /* if the current cycle has finished
     */
    if(gotCycleNumber != m_currentCycleNumber)
    {
      m_currentCycleNumber = gotCycleNumber;
      
   //   COUT_DEBUG("extractData   ---------- >>> NEW CYCLE " << m_currentCycleNumber);
      
      /* get the time reference
       */
      longlong time = getTimeLastEventInCycleFromMonRate();
      
      //COUT_DEBUG("extractData   got last evt in cycle ===> " << time);


     // COUT_DEBUG("Rates calculated after counters:");
      /* try to extract the rate value for each counter
       */
      for(ExtractorMap::iterator it = m_extractorMap.begin();
          it != m_extractorMap.end();
          it++)
      {
        try{
	  RateExtractor * extractor = it->second;
	  extractor->extractData(time, nbElapsedCycles);
	 // double value = extractor->getValue();
	 // COUT_DEBUG("counter #" << extractor->getCounterId() << " ===> " << value << "Hz");
	}catch(const std::exception & e){
	 // COUT_DEBUG(e.what());
	  /* unable to process this counter
	   */
	}
      }
      
    //  COUT_DEBUG("extractData   loop done");
      
    }else
    {
     // COUT_DEBUG("extractData   SAME CYCLE " << gotCycleNumber);
    }
  }
  else
  {
    //COUT_DEBUG("extractData   NO MON RATE");  
  }
  
 // COUT_DEBUG("extractData DONE");
}


int DimInfoMonRate::lookForNewRates()
{
  //COUT_DEBUG("lookForNewRates <=================");
    
  int currentNumberOfCounters;
  
  currentNumberOfCounters = m_extractorMap.size();
    
  //COUT_DEBUG("lookForNewRates   currentNumberOfCounters = " << currentNumberOfCounters);
    
  if(currentNumberOfCounters == m_nbCounterInMonRate)
    return 0;
    
  for(int i = 0; i < m_nbCounterInMonRate; i++)
  {
    m_extractorMap[i] = new RateExtractor(i, m_monRate);
    m_extractorMap[i]->publishService(serviceNameHeader);
  }
      
  //COUT_DEBUG("lookForNewRates   after adding currentNumberOfCounters = " << currentNumberOfCounters);
  
  //COUT_DEBUG("lookForNewRates DONE <=================");
  
  return m_nbCounterInMonRate;
}

// int DimInfoMonRate::lookForNewRates()
// {
//   //COUT_DEBUG("lookForNewRates");
//   
//   if(m_monRate == 0)
//     return 0;
//     
//   int newRates = 0;
//     
//   MonRateMap monRateCounters = m_monRate->counterMap();
//   MonRateMap::iterator browserIt = monRateCounters.begin();
//   
//   ExtractorMap::iterator checkIt;
//   
//   while(browserIt != monRateCounters.end())
//   {
//     checkIt = m_extractorMap.find(browserIt->first);
//     
//     /* if service is not converted yet
//      * then add an entry in the extractor map
//      */
//     if(checkIt == m_extractorMap.end())
//     {
//       COUT_DEBUG("NEW ONE : " << browserIt->first << "<==================");
//       
//       RateExtractor * newExtractor = new RateExtractor(
//                                               browserIt->first,
// 					      m_monRate);
//       m_extractorMap[browserIt->first] = newExtractor;
//       
//       newExtractor->publishServices(serviceNameHeader);
//       
//       newRates++;
//     }else{
//       //COUT_DEBUG("OLD ONE : " << browserIt->first << "<------------------");
//     }
//     
//     browserIt++;
//   } 
//   
//   return newRates;
// }


std::string DimInfoMonRate::makeServiceNameHeader()
{ 
 // COUT_DEBUG("makeServiceNameHeader");
  
  int len = m_monRateServiceName.length();
  std::string header;
  
  /* removing the MonDouble prefix and the slash
   * the prefix usually is MonR but refer to the
   * MonObject.h file (s_pfixMonRate) //v2r0 note: maybe
   */
  const string monRateHeader = s_pfixMonRate;
  int remove = monRateHeader.length()+1; // +1 for the "/"
  header = m_sourceName+"/";
  header += m_monRateServiceName.substr(remove, len-remove);
  
  serviceNameHeader = header;
  
 // COUT_DEBUG("makeServiceNameHeader => " << serviceNameHeader);
  
  return header;
}
