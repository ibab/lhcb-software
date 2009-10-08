#ifndef GAUCHO_MONRATEDECODER_H
#define GAUCHO_MONRATEDECODER_H 1

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaucho/MonObject.h"
#include "TProfile.h"
#include <map>
#include "dis.hxx"


#define MAX_CAR 512


struct Data
{
  double value;
  double counter;
  char comment[MAX_CAR+1];
};

struct IData
{
  unsigned int value;
  char comment[MAX_CAR+1];
};

class MonRate;
class MonRateDecoder {

protected:
  IMessageSvc* m_msgSvc;
  std::string m_utgId;
  std::string m_monRateSvcName;
  double m_oldOffsetTimeFirstEvInRun;
  double m_oldOffsetTimeLastEvInCycle;
  double m_oldOffsetGpsTimeLastEvInCycle;
  int m_oldNumEntries;
  int m_oldRunNumber;
  unsigned int m_oldTriggerConfigurationKey;
  int m_oldCycleNumber;
  double m_oldDesiredDeltaT;

  double m_newOffsetTimeFirstEvInRun;
  double m_newOffsetTimeLastEvInCycle;
  double m_newOffsetGpsTimeLastEvInCycle;
  int  m_newNumEntries;
  int m_newRunNumber;
  unsigned int m_newTriggerConfigurationKey;
  int m_newCycleNumber;
  double m_newDesiredDeltaT;

  int m_oldNumCounters;
  int m_newNumCounters;
  
  bool procexists;
  bool countexists;
  bool TCKexists;
  bool RunNumberexists;
  

  DimService* m_dimSvcNumberOfProcess;
  DimService* m_dimSvcNumberOfCounters; 
  DimService* m_dimSvcTCK; 
  DimService* m_dimSvcRunNumber; 
  std::vector<double >m_oldrateValue;
  std::map<int, std::pair<std::string, double> > m_oldCounters;
  std::map<int, std::pair<std::string, double> > m_newCounters;
  std::map<int, DimService* > m_dimSvcRate;
  std::map<int, DimService* >::iterator m_dimSvcRateIt;

public:
  MonRateDecoder(IMessageSvc* msgSvc, std::string utgId, std::string monRateSvcName);
  virtual ~MonRateDecoder();

  void resetValues();
  void update(MonRate *monRate);
  std::string makeServiceName(std::string nameHeader, int counterId, std::string comment);
  void print();

};

#endif //GAUCHO_MONRATEDECODER_H

