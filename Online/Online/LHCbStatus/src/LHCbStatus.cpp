#include <ctime>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "RTL/rtl.h"
#include "LHCbStatus/LHCbStatus.h"

//extern "C" {
#include "dis.hxx"
//}

using namespace std;
using namespace LHCbStatus;

/// Print information to standard output
ostream& operator<<(ostream& o,const _VELO::_MotionSystem& s);
ostream& operator<<(ostream& o,const _VELO& v);


//template<class T> bool differ_items(const T&    value, const T&    old) {  cout << "Differ: " << typeid(T).name() << " " << sizeof(T) << endl; return value != old;   }
enum { INT_T=1, LONG_T, LONGLONG_T, FLOAT_T, STRING_T, DYNSTRING_T };

void feedData(void* tag, void** buf, int* size, int* first) {
  static const char* empty = "";
  ServiceMap::PubItem* itm = *(ServiceMap::PubItem**)tag;
  char* p = ((char*)itm->entries->current) + itm->offset;
  *buf = itm->buffer;

  switch(itm->typ) {
  case INT_T:
    *size = sprintf(itm->buffer,"%d",*(int*)p);
    return;
  case LONG_T:
    *size = sprintf(itm->buffer,"%ld",*(long*)p);
    return;
  case LONGLONG_T:
    *size = sprintf(itm->buffer,"%lld",*(long long*)p);
    return;
  case FLOAT_T:
    *size = sprintf(itm->buffer,"%f",*(float*)p);
    return;
  case STRING_T:
    *size = ::strlen(p)+1;
    *buf = p;
    return;
  default:
    break;
  }
  *size = 0;
  *buf = (void*)empty;
}

#define PUBLISH(s,x,y)     pubItem(s,#x "." #y,getType(s.current->x.y),((char*)&s.current->x.y)-((char*)s.current),sizeof(s.current->x.y))
#define PUBLISH_STRING(s,x,y)      pubItem(s,#x "." #y,STRING_T,((char*)s.current->x.y)-((char*)s.current),sizeof(s.current->x.y))
#define PUBLISH_DYN_STRING(s,x,y)  pubItem(s,#x "." #y,DYNSTRING_T,((char*)s.current->x.y)-((char*)s.current),sizeof(s.current->x.y))
int  getType(int&)           {   return INT_T; }
int  getType(long&)          {   return LONG_T; }
int  getType(long long&)     {   return LONGLONG_T; }
int  getType(float&)         {   return FLOAT_T; }

bool differ_items(int value, int old, size_t) {  return value != old;  }
bool differ_items(long value, long old, size_t) {  return value != old;  }
bool differ_items(long long value, long long old, size_t) {  return value != old;  }
bool differ_items(float value, float old, size_t) {  return value != old;  }
bool differ_items(const char* value, const char* old, size_t) {  return strcmp(value,old)!=0;  }
bool differ_items(const char value[SUBFARM_NAME_LENGTH][NUM_SUBFARM], const char old[SUBFARM_NAME_LENGTH][NUM_SUBFARM], size_t len) {  return memcmp(value,old,len)!=0;  }

static void pubItem(ServiceMap& s, const char* item_name, int typ, size_t offset,size_t len) {
  if ( memcmp(((char*)s.current)+offset,((char*)s.previous)+offset,len) != 0 )  {
    ServiceMap::Services::iterator i=s.services.find(item_name);
    if ( i == s.services.end() )    {
      string nam = s.current->partition;
      nam += ".";
      nam += item_name;
      ServiceMap::PubItem *item = new ServiceMap::PubItem;
      item->typ = typ;
      item->entries = &s;
      item->offset  = offset;
      i = s.services.insert(make_pair(item_name,item)).first;      
      item->svc = ::dis_add_service((char*)nam.c_str(),(char*)"C",0,0,feedData,(long)item);
      if ( i == s.services.end() )    {
	delete item;
	throw "Failed to access DIM service.";
      }
    }
    cout << "Publish " << s.current->partition << "." << (*i).first << endl;
    ::dis_update_service((*i).second->svc);
  }
}

/// Print information to standard output
ostream& operator<<(ostream& o,const _VELO::_MotionSystem& s) {
  o << " resolPosRC: " << setw(12) << left << s.resolPosRC;
  o << " resolPosLA: " << setw(12) << left << s.resolPosLA;
  o << " resolPosY:  " << setw(12) << left << s.resolPosY;
  return o;
}

/// Print information to standard output
ostream& operator<<(ostream& o,const _VELO& v) {
  o << "----> VELO:   " << v.MotionSystem << endl;
  return o;
}


/// Publish data contents
int _VELO::publish(ServiceMap& s) {
  PUBLISH(s,VELO,MotionSystem.resolPosRC);
  PUBLISH(s,VELO,MotionSystem.resolPosLA);
  PUBLISH(s,VELO,MotionSystem.resolPosY);
  return 1;
}

/// Print information to standard output
ostream& operator<<(ostream& o,const _General& gen)   {
  o << "----> General:"
    << " status:     " << setw(12) << left << gen.status
    << " partID:     " << setw(12) << left << gen.activePartId
    << " run Number: " << setw(12) << left << gen.runNumber
    << " fillNumber: " << setw(12) << left << gen.fillNumber
    << endl;
  o << "              "
    << " numTAE:     " << setw(12) << left << gen.TAE
    << " NTriggers:  " << setw(12) << left << gen.runNTriggers
    << " runNMinutes:" << setw(12) << left << gen.runNMinutes
    << endl;
  o << "              "
    << " runType:    " << setw(12) << left << gen.runType
    << " dataType:   " << setw(12) << left << gen.dataType
    << endl;
  char s1[128], s2[128], s3[128];
  ::strftime(s1,128,"%b %d %H:%M:%S",::localtime(&gen.runStartTime));
  ::strftime(s2,128,"%b %d %H:%M:%S",::localtime(&gen.runStopTime));
  ::strftime(s3,128,"%b %d %H:%M:%S",::localtime(&gen.runInitialStartTime));
  o << "              "
    << " StartTime:  " << setw(12) << left << s1
    << " StopTime:   " << setw(12) << left << s2
    << " InitialTime:" << setw(12) << left << s3
    << endl;
  return o;
}

/// Publish data contents
int _General::publish(ServiceMap& s) {
  PUBLISH(s,general,status);
  PUBLISH(s,general,activePartId);
  PUBLISH(s,general,runNumber);
  PUBLISH(s,general,fillNumber);
  PUBLISH(s,general,TAE);
  PUBLISH(s,general,runNTriggers);
  PUBLISH(s,general,runNMinutes);
  PUBLISH_STRING(s,general,runType);
  PUBLISH_STRING(s,general,dataType);
  PUBLISH(s,general,runStartTime);
  PUBLISH(s,general,runStopTime);
  PUBLISH(s,general,runInitialStartTime);
  return 1;
}

/// Print information to standard output
ostream& operator<<(ostream& o,const _Trigger& trg) {
  o << "----> Trigger:"
    << " TCK:        " << setw(24) << left << trg.TCK
    << " Label:       " << setw(24) << left << trg.TCKLabel
    << endl;
  o << "              "
    << " HLTType:    " << setw(24) << left << trg.HLTType
    << " L0Type:      " << setw(24) << left << trg.L0Type
    << endl;
  o << "              "
    << " onlineVSN:  " << setw(24) << left << trg.onlineVersion
    << " gaudiVSN:    " << setw(24) << left << trg.gaudiVersion
    << endl;
  o << "              "
    << " mooreVSN:   " << setw(24) << left << trg.mooreVersion
    << " dataflowV:   " << setw(24) << left << trg.dataflowVersion
    << endl;
  return o;
}

/// Publish data contents
int _Trigger::publish(ServiceMap& s) {
  PUBLISH(s,Trigger,TCK);
  PUBLISH_STRING(s,Trigger,TCKLabel);
  PUBLISH_STRING(s,Trigger,HLTType);
  PUBLISH_STRING(s,Trigger,L0Type);
  PUBLISH_STRING(s,Trigger,onlineVersion);
  PUBLISH_STRING(s,Trigger,gaudiVersion);
  PUBLISH_STRING(s,Trigger,mooreVersion);
  PUBLISH_STRING(s,Trigger,dataflowVersion);
  return 1;
}

/// Print information to standard output
ostream& operator<<(ostream& o,const _TFC& tfc) {
  o << "----> TFC:    " 
    << " Odin:       " << setw(12) << left << tfc.OdinData
    << " nTriggers:  " << setw(12) << left << tfc.numTriggers
    << " Rate(Now):  " << setw(12) << left << tfc.triggerRate
    << " Rate(Run):  " << setw(12) << left << tfc.runTriggerRate
    << endl;
  o << "              "
    << " LifeTime:   " << setw(12) << left << tfc.lifeTime
    << " dto.Run:    " << setw(12) << left << tfc.runLifeTime
    << " DeadTime:   " << setw(12) << left << tfc.deadTime
    << " dto.Run:    " << setw(12) << left << tfc.runDeadTime
    << endl;
  o << "              "
    << " nTriggerU:  " << setw(12) << left << tfc.nTriggersU
    << " TrgLost:    " << setw(12) << left << tfc.lostTriggers
    << endl;
  return o;
}

/// Publish data contents
int _TFC::publish(ServiceMap& s) {
  PUBLISH_STRING(s,TFC,OdinData);
  PUBLISH(s,TFC,numTriggers);
  PUBLISH(s,TFC,triggerRate);
  PUBLISH(s,TFC,runTriggerRate);
  PUBLISH(s,TFC,lifeTime);
  PUBLISH(s,TFC,runLifeTime);
  PUBLISH(s,TFC,deadTime);
  PUBLISH(s,TFC,runDeadTime);
  PUBLISH(s,TFC,nTriggersU);
  PUBLISH(s,TFC,lostTriggers);
  return 1;
}

/// Print information to standard output
ostream& operator<<(ostream& o,const _Magnet::_Item& i) {
  o << "  Polarity:  " << setw(10) << i.Polarity << "  Current:" << setw(12) << i.Current;
  return o;
}

/// Print information to standard output
ostream& operator<<(ostream& o,const _Magnet& m) {
  o << "----> Magnet:  Settings: " << m.Set << endl;
  o << "               Readings: " << m.Measured << endl;
  return o;
}

/// Publish data contents
int _Magnet::publish(ServiceMap& s) {
  PUBLISH(s,Magnet,Set.Polarity);
  PUBLISH(s,Magnet,Set.Current);
  PUBLISH(s,Magnet,Measured.Polarity);
  PUBLISH(s,Magnet,Measured.Current);
  return 1;
}

/// Print information to standard output
ostream& operator<<(ostream& o,const _LHC& lhc) {
  o << "----> LHC:    " 
    << " Fill:       " << setw(12) << left << lhc.FillNumber 
    << " Energy:     " << setw(12) << left << lhc.Energy
    << " Mode:       " << setw(12) << left << lhc.Mode
    << " State:      " << setw(12) << left << lhc.State
    << endl;
  return o;
}

/// Publish data contents
int _LHC::publish(ServiceMap& s) {
  PUBLISH(s,LHC,FillNumber);
  PUBLISH(s,LHC,Energy);
  PUBLISH_STRING(s,LHC,Mode);
  PUBLISH_STRING(s,LHC,State);
  return 1;
}

/// Print information to standard output
ostream& operator<<(ostream& o,const _HLT& hlt) {
  o << "----> HLT Info:"
    << " # subfarms: " << setw(12) << left << hlt.nSubFarms 
    << " Rate:       " << setw(12) << left << hlt.hltRate
    << " Run Rate:   " << setw(12) << left << hlt.runHltRate
    << " # Triggers: " << setw(12) << left << hlt.hltNTriggers
    << endl;
  return o;
}

/// Standard constructor
_HLT::_HLT() {
  //for(size_t i=0; i<sizeof(subfarms)/sizeof(subfarms[0]); ++i)
  //  subfarms[i] = buffer + i*SUBFARM_NAME_LENGTH;
}

/// Publish data contents
int _HLT::publish(ServiceMap& s) {
  PUBLISH(s,HLTFarm,nSubFarms);
  PUBLISH(s,HLTFarm,hltRate);
  PUBLISH(s,HLTFarm,runHltRate);
  PUBLISH(s,HLTFarm,hltNTriggers);
  PUBLISH_DYN_STRING(s,HLTFarm,subfarms);
  return 1;
}

/// Print information to standard output
ostream& operator<<(ostream& o,const _HLTSubfarm& /* t */) {
  return o;
}

/// Publish data contents
int _HLTSubfarm::publish(ServiceMap& /*s*/) {
  return 1;
}

/// Print information to standard output
ostream& operator<<(ostream& o,const DaqSystem& /* t */) {
  o << "----> DaqSystem: " << endl;
  return o;
}

/// Publish data contents
int DaqSystem::publish(ServiceMap& /*s*/) {
  return 1;
}

/// Print information to standard output
ostream& operator<<(ostream& o,const SubDetector& sd) {
  o << setw(8) << left << sd.name 
    << setw(12) << left << sd.hvStatus
    << setw(12) << left << sd.dcsStatus
    << setw(12) << left << sd.daiStatus
    << setw(14) << left << sd.daqStatus
    << setw(14) << left << sd.hwStatus
    << setw(12) << left << sd.hwLock
    << setw(12) << left << sd.hwIncluded
    << setw(12) << left << sd.hwComplete
    << endl;
  return o;
}

/// Publish data contents
int SubDetector::publish(ServiceMap& /*s*/) {
  return 1;
}

/// Print information to standard output
ostream& operator<<(ostream& o,const ExperimentStatus& s) {
  size_t i;
  o << "Experiment Status Dump:" << s.partition << endl;
  o << s.general 
    << s.LHC
    << s.Magnet
    << s.HLTFarm
    << s.TFC
    << s.VELO
    << s.Trigger
    << endl;

  if ( s.numDetectors > 0 ) {
    o << endl
      << "      " << setw(8) << left << "SD"
      << setw(12) << left << "HV"
      << setw(12) << left << "DCS"
      << setw(12) << left << "DAI"
      << setw(14) << left << "DAQ"
      << setw(14) << left << "HW"
      << setw(12) << left << "Lock"
      << setw(12) << left << "Included"
      << setw(12) << left << "Complete"
      << endl;  
    for(i=0; i<13; ++i)
      o << "---------";
    o << endl;
    for(i=0; i<size_t(s.numDetectors); ++i)
      o << "      " << *s.detector(i);
  }
  o << endl;
  return o;
}

/// Publish data contents
int ExperimentStatus::publish(ServiceMap& s) {
  //partition.publish(s);
  general.publish(s);
  LHC.publish(s);
  Magnet.publish(s);
  HLTFarm.publish(s);
  TFC.publish(s);
  VELO.publish(s);
  Trigger.publish(s);
  for(size_t i=0; i<size_t(numDetectors); ++i)
    detector(i)->publish(s);
  return 1;
}

/// Print information to standard output
void SubDetector::dump()             {  cout << *this << endl;  }

/// Print information to standard output
void DaqSystem::dump()               {  cout << *this << endl;  }

/// Print information to standard output
void _LHC::dump()                    {  cout << *this << endl;  }

/// Print information to standard output
void _Magnet::_Item::dump()          {  cout << *this << endl;  }

/// Print information to standard output
void _Magnet::dump()                 {  cout << *this << endl;  }

/// Print information to standard output
void _HLTSubfarm::dump()             {  cout << *this << endl;  }

/// Print information to standard output
void _HLT::dump()                    {  cout << *this << endl;  }

/// Print information to standard output
void _TFC::dump()                    {  cout << *this << endl;  }

/// Print information to standard output
void _Trigger::dump()                {  cout << *this << endl;  }

/// Print information to standard output
void _General::dump()                {  cout << *this << endl;  }

/// Print information to standard output
void _VELO::_MotionSystem::dump()    {  cout << *this << endl;   }

/// Print information to standard output
void _VELO::dump()                   {  cout << *this << endl;  }

/// Print information to standard output
void ExperimentStatus::dump()        {  cout << *this << endl; }

/// Standard constructor
Server::Server()    {
  m_services.current = new LHCbStatus::ExperimentStatus();
  ::memset(m_services.current,0,sizeof(LHCbStatus::ExperimentStatus));
  m_services.previous = new LHCbStatus::ExperimentStatus();
  ::memset(m_services.previous,0,sizeof(LHCbStatus::ExperimentStatus));
}

/// Standard destructor
Server::~Server()     {
  if ( 0 != m_services.current ) {
    delete m_services.current;
    m_services.current = 0;
  }
  if ( 0 != m_services.previous ) {
    delete m_services.previous;
    m_services.previous = 0;
  }
}

/// Start serving data to clients
void Server::start() {
  static bool first = true;
  if ( first ) {
    first = false;
    string server = RTL::processName();
    cout << server << "> Start serving data...." << endl;
    ::dis_start_serving((char*)server.c_str());
  }
}

/// Connect datapoints to PVSS
bool Server::connect() {
  return true;
}

/// Load information from PVSS
bool Server::load() {
  return true;
}

void Server::swapBuffers() {
  ExperimentStatus* tmp = m_services.current;
  m_services.current = m_services.previous;
  m_services.previous = tmp;
}

/// Publish data contents
int Server::publish() {
  if ( m_services.current ) {
    return m_services.current->publish(m_services);
  }
  return 0;
}
