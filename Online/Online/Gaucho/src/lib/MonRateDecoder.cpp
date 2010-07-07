#include "Gaucho/MonRateDecoder.h"
#include "Gaucho/MonRate.h"
#include "Gaucho/Misc.h"



MonRateDecoder::MonRateDecoder(IMessageSvc* msgSvc, std::string utgId, std::string monRateSvcName) : m_msgSvc(msgSvc), m_utgId(utgId), m_monRateSvcName(monRateSvcName)
{
  resetValues();
  MsgStream msg(m_msgSvc, "MonRateDecoder");

 /* std::string numberOfProcessSvcName = m_utgId + "/" + 
                            m_monRateSvcName.substr(s_pfixMonRate.length()+1, 
                                                    m_monRateSvcName.length() - s_pfixMonRate.length()+1) +
                                                                 "/NUMBER_OF_PROCESSES";*/
}

void MonRateDecoder::resetValues() {
  m_oldOffsetTimeFirstEvInRun = 0;
  m_oldOffsetTimeLastEvInCycle = 0;
  m_oldOffsetGpsTimeLastEvInCycle = 0;
  m_oldNumEntries = 0;
  m_oldRunNumber = 0;
  m_oldTriggerConfigurationKey = 0;
  m_oldCycleNumber = 0;
  m_oldDesiredDeltaT = 0;

  m_newOffsetTimeFirstEvInRun = 0;
  m_newOffsetTimeLastEvInCycle = 0;
  m_newOffsetGpsTimeLastEvInCycle = 0;
  m_newNumEntries = 0;
  m_newRunNumber = 0;
  m_newTriggerConfigurationKey = 0;
  m_newCycleNumber = 0;
  m_newDesiredDeltaT = 0;

  procexists=false;
  countexists=false;
  TCKexists=false;
  RunNumberexists=false;

}

MonRateDecoder::~MonRateDecoder(){
  for (m_dimSvcRateIt = m_dimSvcRate.begin(); m_dimSvcRateIt != m_dimSvcRate.end(); m_dimSvcRateIt ++) {
    delete m_dimSvcRateIt->second;
    m_dimSvcRateIt->second = 0;
  }
  if (procexists) delete m_dimSvcNumberOfProcess;
  if (countexists) delete m_dimSvcNumberOfCounters;
  if (TCKexists) delete m_dimSvcTCK;
  if (RunNumberexists) delete m_dimSvcRunNumber;
  m_dimSvcRate.clear();
}

void MonRateDecoder::update(MonRate *monRate) {
  MsgStream msg(m_msgSvc, "MonRateDecoder");
  msg << MSG::DEBUG << "MonRateDecoder::update"<<endreq;

  m_oldOffsetTimeFirstEvInRun = m_newOffsetTimeFirstEvInRun;
  m_oldOffsetTimeLastEvInCycle = m_newOffsetTimeLastEvInCycle;
  m_oldOffsetGpsTimeLastEvInCycle = m_newOffsetGpsTimeLastEvInCycle;
  m_oldNumEntries = m_newNumEntries;
  m_oldRunNumber = m_newRunNumber;
  m_oldTriggerConfigurationKey = m_newTriggerConfigurationKey;
  m_oldCycleNumber = m_newCycleNumber;
  m_oldDesiredDeltaT = m_newDesiredDeltaT;
  m_oldNumCounters = m_newNumCounters;
 
  int tmpcycle;
  int tmpentries;
  tmpentries = (int) monRate->binEntry(4);
  tmpcycle = (int) monRate->binContent(7);
  msg << MSG::DEBUG << "The old cycle number was :" << m_oldCycleNumber << endreq;
  msg << MSG::DEBUG << "checking the new cycle number :" << tmpcycle << endreq;
  if (m_oldCycleNumber == tmpcycle ) {
    msg << MSG::DEBUG << "No update because the cycle number did not change"<<endreq;
    return;
  }

  m_newOffsetTimeFirstEvInRun = monRate->binContent(1);
  m_newOffsetTimeLastEvInCycle = monRate->binContent(2);
  m_newOffsetGpsTimeLastEvInCycle = monRate->binContent(3);
  m_newNumEntries = (int) monRate->binEntry(4);
  m_newRunNumber = (int) monRate->binContent(5);
  m_newTriggerConfigurationKey = (unsigned int) monRate->binContent(6);
  m_newCycleNumber = (int) monRate->binContent(7);
  m_newDesiredDeltaT = monRate->binContent(8);
  m_newNumCounters = monRate->numbinx();

  Data m_numprocessesData;
  m_numprocessesData.value = m_newNumEntries;
  m_numprocessesData.counter = m_newNumEntries;
  strcpy(m_numprocessesData.comment, "\0");
  std::string numprocessesComment="Number of processes publishing via this MonRate";
  int commentSize = Misc::min(MAX_CAR, numprocessesComment.length()+1);
  strncpy(m_numprocessesData.comment, numprocessesComment.c_str(), commentSize);
  int nbprocdataSize = sizeof(double) + commentSize * sizeof(char);
  std::string numberOfProcessSvcName =  m_monRateSvcName.substr(s_pfixMonRate.length()+1, 
                                                    m_monRateSvcName.length() - s_pfixMonRate.length()+1) +
                                                                 "/NUMBER_OF_PROCESSES";
   msg << MSG::DEBUG << "MonRateDecoder update m_numprocessedData.value: "<< m_numprocessesData.value << endreq;
  
  
  if (!procexists) {
    static const std::string s_nbprocFormat("D:2;C");
    char * ttmpFormat = new char[s_nbprocFormat.length()+1];
    strcpy(ttmpFormat, s_nbprocFormat.c_str());    
    m_dimSvcNumberOfProcess = new DimService(numberOfProcessSvcName.c_str(), ttmpFormat, (void*)&m_numprocessesData,  nbprocdataSize);
    delete ttmpFormat;
    procexists=true;
  } 
  else  m_dimSvcNumberOfProcess->updateService((void*)&m_numprocessesData, nbprocdataSize);
  
  IData m_TCKData;
  m_TCKData.value = m_newTriggerConfigurationKey;
  strcpy(m_TCKData.comment, "\0");
  std::string TCKComment="Currently active TCK";
  int TCKcommentSize = Misc::min(MAX_CAR, TCKComment.length()+1);
  strncpy(m_TCKData.comment, TCKComment.c_str(), TCKcommentSize);
  int TCKdataSize = sizeof(unsigned int) + TCKcommentSize * sizeof(char);
  std::string TCKSvcName =  m_monRateSvcName.substr(s_pfixMonRate.length()+1, 
                                                    m_monRateSvcName.length() - s_pfixMonRate.length()+1) +
                                                                 "/TCK";

   msg << MSG::DEBUG << "MonRateDecoder update m_TCKData.value: "<< m_TCKData.value << endreq;
  
  
  if (!TCKexists) {
    static const std::string s_TCKFormat("I:1;C");
    char * tcmpFormat = new char[s_TCKFormat.length()+1];
    strcpy(tcmpFormat, s_TCKFormat.c_str());    
    m_dimSvcTCK = new DimService(TCKSvcName.c_str(), tcmpFormat, (void*)&m_TCKData,  TCKdataSize);
    delete tcmpFormat;
    TCKexists=true;
  } 
  else  m_dimSvcTCK->updateService((void*)&m_TCKData, TCKdataSize);
  

  std::string RunNumberSvcName =  m_monRateSvcName.substr(s_pfixMonRate.length()+1, 
                                                    m_monRateSvcName.length() - s_pfixMonRate.length()+1) +
                                                                 "/RunNumber"; 

  if (!RunNumberexists) {
    msg << MSG::DEBUG << "Making runnumber service: "<< RunNumberSvcName.c_str() << endreq;
    m_dimSvcRunNumber = new DimService(RunNumberSvcName.c_str(),m_newRunNumber);
    RunNumberexists=true;
  } 

  int updates=0;
  updates=m_dimSvcRunNumber->updateService();
 // msg << MSG::INFO << "MonRateDecoder update m_newRunNumber: "<< m_newRunNumber << " clients updated " << updates << endreq;
  
  Data m_numcountersData;
  //need to add 1 for the number of processes service
  m_numcountersData.value = m_oldNumCounters+1;
  m_numcountersData.counter = m_oldNumCounters+1;
  strcpy(m_numcountersData.comment, "\0");
  std::string numcountersComment="Number of MonRates processed by this RateService";
  int commentcSize = Misc::min(MAX_CAR, numcountersComment.length()+1);
  strncpy(m_numcountersData.comment, numcountersComment.c_str(), commentcSize);
  int nbcountdataSize = sizeof(double) + commentcSize * sizeof(char);
  std::string numberOfCountersSvcName =   m_utgId + "/NumberOfMonRates";
  msg << MSG::DEBUG << "MonRateDecoder update m_numcountersData.value: "<< m_numcountersData.value << endreq;
  
  
  if (!countexists){
    static const std::string s_nbcountersFormat("D:2;C");
    char * tttmpFormat = new char[s_nbcountersFormat.length()+1];
    strcpy(tttmpFormat, s_nbcountersFormat.c_str());    
    m_dimSvcNumberOfCounters = new DimService(numberOfCountersSvcName.c_str(),tttmpFormat, (void*)&m_numcountersData,  nbcountdataSize);
    delete tttmpFormat;
    countexists=true;
  } 
  else m_dimSvcNumberOfCounters->updateService((void*)&m_numcountersData,nbcountdataSize);
  //normally m_newCycleNumber - m_oldCycleNumber = 1
  double realDeltaT = (m_newCycleNumber - m_oldCycleNumber)*m_newDesiredDeltaT;
  
  //maybe this correction is not good
  //realDeltaT += (m_newOffsetGpsTimeLastEvInCycle - m_oldOffsetGpsTimeLastEvInCycle);
  realDeltaT = m_newDesiredDeltaT;
  double tmp = m_newCycleNumber - m_oldCycleNumber;
  double tmp1 = m_newOffsetGpsTimeLastEvInCycle - m_oldOffsetGpsTimeLastEvInCycle;
  
  msg << MSG::DEBUG << "m_newCycleNumber - m_oldCycleNumber = " << tmp1 <<endreq;  
  msg << MSG::DEBUG << "m_newOffsetGpsTimeLastEvInCycle - m_oldOffsetGpsTimeLastEvInCycle = " << tmp <<endreq;
  msg << MSG::DEBUG << "realDeltaT = " << realDeltaT << " m_newDesiredDeltaT = " << m_newDesiredDeltaT << endreq;

  msg << MSG::DEBUG << "m_oldNumCounters = " << m_oldNumCounters <<endreq;
  msg << MSG::DEBUG << "m_newNumCounters = " << m_newNumCounters <<endreq;

  msg << MSG::DEBUG << "number of processes = " << m_newNumEntries <<endreq;
  //this used to be m_oldNumCounters - which could be unitialized the first time round
  msg << MSG::DEBUG << "transfering " << m_newNumCounters << " new to old counters " << endreq;
  for (int i = 0; i < m_newNumCounters; i++) {
    m_oldCounters[i] = m_newCounters[i];
  }

  msg << MSG::DEBUG << "updating " << m_newNumCounters << " new counters " <<endreq;

  for (int i = 0; i < m_newNumCounters; i++) {
    msg << MSG::DEBUG << "updating counter " << i <<endreq;

    std::string comment(monRate->binLabX(8 + i));
    if (comment.compare("") == 0) {
      msg << MSG::DEBUG << "comment empty, then we assume that the counters end is here "<<endreq;
      m_newNumCounters = i;
      break;
    }

    double counterMean = monRate->binContent(9 + i);
    double counterValue = m_newNumEntries*counterMean;
    m_newCounters[i] = std::pair<std::string, double> (comment, counterValue);

    
    msg << MSG::DEBUG << "comment = " << comment <<endreq;

    if(i >= m_oldNumCounters){
      m_oldCounters[i] = std::pair<std::string, double> (comment, 0);
    }

    double rateValue = m_newCounters[i].second - m_oldCounters[i].second;
    rateValue *= 1000000.0;
    rateValue /= realDeltaT;
    
    if((int)m_oldrateValue.size()<=i) {
       //the first time, fill the vector
       m_oldrateValue.push_back(rateValue);
    }
    else {   
       if (rateValue<=0.000001) { rateValue=m_oldrateValue[i];}
       m_oldrateValue[i]=rateValue;
    }  
    Data m_rateData;
    m_rateData.value = rateValue;
    //evh addition: in addition to the rate, we also publish the value of the counters 
    m_rateData.counter = counterValue;
    strcpy(m_rateData.comment, "\0");
    int commentSize = Misc::min(MAX_CAR, comment.length()+1);

    strncpy(m_rateData.comment, comment.c_str(), commentSize);
    int dataSize = 2*sizeof(double) + commentSize * sizeof(char);
    msg << MSG::DEBUG << "rateValue = " << rateValue <<" counterValue = " << counterValue << endreq;
    
    if (m_dimSvcRate.find(i) == m_dimSvcRate.end()){
     /* std::string header = m_utgId + "/" + 
                            m_monRateSvcName.substr(s_pfixMonRate.length()+1, 
                                                    m_monRateSvcName.length() - s_pfixMonRate.length()+1);*/
      std::string header = m_monRateSvcName.substr(s_pfixMonRate.length()+1, 
                                                    m_monRateSvcName.length() - s_pfixMonRate.length()+1);
      std::string serviceName =	makeServiceName(header,i,comment);
      					    
      static const std::string s_rateServiceFormat("D:2;C");
      char * tmpFormat = new char[s_rateServiceFormat.length()+1];
      strcpy(tmpFormat, s_rateServiceFormat.c_str());
      msg << MSG::DEBUG << "It's a new counter, then we create DimService " << serviceName <<endreq;
      m_dimSvcRate[i] = new DimService (serviceName.c_str(), tmpFormat, (void*)&m_rateData, dataSize);
      delete tmpFormat;
    }
    else{
      msg << MSG::DEBUG << "It's an already created service, then we update DimService " <<endreq;
      m_dimSvcRate[i]->updateService((void*)&m_rateData, dataSize);
    }
  }
}


std::string MonRateDecoder::makeServiceName(std::string nameHeader, int counterId, std::string comment)
{
  bool finished=false;
  std::string countertype="";
  while (!finished) {
     std::size_t starpos = comment.find("*");
     if (starpos != std::string::npos) {
        comment=comment.substr(0,starpos)+"star"+comment.substr(starpos+1);
     }  
     else finished=true;
  }
  finished=false;
  while (!finished) {    
     std::size_t qmpos = comment.find("?");
     if (qmpos != std::string::npos) {
        comment=comment.substr(0,qmpos)+"qm"+comment.substr(qmpos+1);
     } 
     else finished=true;
  }
  finished=false;
  while (!finished) {
     std::size_t cmpos = comment.find(",");
     if (cmpos != std::string::npos) {
        comment=comment.substr(0,cmpos)+"cm"+comment.substr(cmpos+1);
     } 
     else finished=true;  
  }
  finished=false;
  while (!finished) {
     std::size_t orpos = comment.find("|");
     if (orpos != std::string::npos) {
        comment=comment.substr(0,orpos)+"o"+comment.substr(orpos+1);
     } 
     else finished=true;  
  }
  finished=false;
  while (!finished) {     
     std::size_t exclpos = comment.find("!");
     if (exclpos != std::string::npos) {
        comment=comment.substr(0,exclpos)+"ex"+comment.substr(exclpos+1);
     }  
     else finished=true;
  }  
  while (!finished) {     
     std::size_t eqpos = comment.find("=");
     if (eqpos != std::string::npos) {
        comment=comment.substr(0,eqpos)+"eq"+comment.substr(eqpos+1);
     }  
     else finished=true;
  } 
   
  std::size_t typepos = comment.find("flag");
  if (typepos != std::string::npos) {
     countertype="-flag";
  }  
  else {
     typepos = comment.find("nEntries");
     if (typepos != std::string::npos) {
        countertype="-nEntries";
    } 
  }   
           
  char * c = new char[comment.length()+1];
  strcpy(c, comment.c_str());

  std::string name = "";
  char * part = NULL;

  part = strtok(c, " ");
  while(part != NULL)
  {
    name += part;
    part = strtok(NULL, " ");
  }

  if(name == "")
  {
    std::stringstream streamName;
    streamName << nameHeader << "/rateFromCounter#" << counterId;

    name = streamName.str();
  }else{
    name = nameHeader + "/" + name;
  }

  if (name.length()> 120) {
     MsgStream msg(m_msgSvc, "MonRateDecoder");
     msg << MSG::INFO << "Name "<< name << "too long "<< name.length() << endreq;
     name=name.substr(0,120)+countertype; 
     msg << MSG::INFO << "New name "<< name <<  endreq;
  }


  return name;
}
void MonRateDecoder::print(){
  MsgStream msg(m_msgSvc, "MonRateDecoder");
  msg << MSG::INFO << "************************************************"<<endreq;
  msg << MSG::INFO << "**************MonRateDecoder Data***************"<<endreq;
  msg << MSG::INFO << "************************************************"<<endreq;
  msg << MSG::INFO << " newOffsetTimeFirstEvInRun: "<<  m_newOffsetTimeFirstEvInRun << endreq;
  msg << MSG::INFO << " oldOffsetTimeFirstEvInRun: "<<  m_oldOffsetTimeFirstEvInRun << endreq;
  msg << MSG::INFO << "************************************************"<<endreq;
  msg << MSG::INFO << " newOffsetTimeLastEvInCycle: "<<  m_newOffsetTimeLastEvInCycle << endreq;
  msg << MSG::INFO << " oldOffsetTimeLastEvInCycle: "<<  m_oldOffsetTimeLastEvInCycle << endreq;
  msg << MSG::INFO << "************************************************"<<endreq;
  msg << MSG::INFO << " newOffsetGpsTimeLastEvInCycle: "<<  m_newOffsetGpsTimeLastEvInCycle << endreq;
  msg << MSG::INFO << " oldOffsetGpsTimeLastEvInCycle: "<<  m_oldOffsetGpsTimeLastEvInCycle << endreq;
  msg << MSG::INFO << "************************************************"<<endreq;
  msg << MSG::INFO << " newNumEntries: "<<  m_newNumEntries << endreq;
  msg << MSG::INFO << " oldNumEntries: "<<  m_oldNumEntries << endreq;
  msg << MSG::INFO << "************************************************"<<endreq;
  msg << MSG::INFO << " newRunNumber: "<<  m_newRunNumber << endreq;
  msg << MSG::INFO << " oldRunNumber: "<<  m_oldRunNumber << endreq;
  msg << MSG::INFO << "************************************************"<<endreq;
  msg << MSG::INFO << " newTCK: "<<  m_newTriggerConfigurationKey << endreq;
  msg << MSG::INFO << " oldTCK: "<<  m_oldTriggerConfigurationKey << endreq;
  msg << MSG::INFO << "************************************************"<<endreq;
  msg << MSG::INFO << " newCycleNumber: "<<  m_newCycleNumber << endreq;
  msg << MSG::INFO << " oldCycleNumber: "<<  m_oldCycleNumber << endreq;
  msg << MSG::INFO << "************************************************"<<endreq;
  msg << MSG::INFO << " newDeltaT: "<<  m_newDesiredDeltaT << endreq;
  msg << MSG::INFO << " oldDeltaT: "<<  m_oldDesiredDeltaT << endreq;
  msg << MSG::INFO << "************************************************"<<endreq;

  double realDeltaT = (m_newCycleNumber - m_oldCycleNumber)*m_newDesiredDeltaT;
//  realDeltaT += (m_newOffsetGpsTimeLastEvInCycle - m_oldOffsetGpsTimeLastEvInCycle);
  realDeltaT = m_newDesiredDeltaT;
  msg << MSG::INFO << " Real DeltaT: "<<  realDeltaT << endreq;
  msg << MSG::INFO << "************************************************"<<endreq;

  for (int i = 0; i < m_newNumCounters; i++) {
    if (m_newCounters[i].first.compare("") == 0){
      msg << MSG::INFO << "comment empty, then we assume that the counters end is here "<<endreq;
      break;
    }
    msg << MSG::INFO << " Counter     ==> " << m_newCounters[i].first << endreq;
    msg << MSG::INFO << "   new counter value = " << m_newCounters[i].second << endreq;
    msg << MSG::INFO << "   old counter value = " << m_oldCounters[i].second << endreq;

    double rateValue = m_newCounters[i].second - m_oldCounters[i].second;
    rateValue *= 1000000.0;
    rateValue /= realDeltaT;
    msg << MSG::INFO << "   rate value = "<<  rateValue << endreq;
    msg << MSG::INFO << "------------------------------------------------"<<endreq;
  }
  msg << MSG::INFO << "************************************************"<<endreq;
}

