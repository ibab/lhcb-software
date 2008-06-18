#include "Gaucho/MonRate.h"

MonRate::MonRate(IMessageSvc* msgSvc, const std::string& source, int version):
  MonObject(msgSvc, source, version)
{
  m_typeName=s_monRate;
  m_dimPrefix="MonR";
  m_runNumber = new int(0);
  m_cycleNumber = new int(0);
  m_timeFirstEvInRun = new longlong(0);
  m_timeLastEvInCycle = new longlong(0);
}
  
MonRate::~MonRate(){
}

void MonRate::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar,version);
  
  int size = m_counterMap.size();
  ar & size;
  for (m_counterMapIt = m_counterMap.begin(); m_counterMapIt != m_counterMap.end(); ++m_counterMapIt){
    ar & m_counterMapIt->first;
    ar & (*m_counterMapIt->second);
  }
  ar & (*m_timeFirstEvInRun);
  ar & (*m_timeLastEvInCycle);
  ar & (*m_runNumber);
  ar & (*m_cycleNumber);
}

void MonRate::load(boost::archive::binary_iarchive  & ar, const unsigned int version)
{
  MonObject::load(ar, version);
  int size = 0;
  MsgStream msg = createMsgStream();
  ar & size;
  for (int i = 0; i < size; i++)
  {
    std::string name;
    double val = 0.00;
    ar & name;
    ar & val;
    m_counterMap[name] = new double(val);
  }
  ar & (*m_timeFirstEvInRun);
  ar & (*m_timeLastEvInCycle);
  ar & (*m_runNumber);
  ar & (*m_cycleNumber);
}

void MonRate::combine(MonObject * monObject){
  MsgStream msg = createMsgStream();
  if (monObject->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<< monObject->typeName() << " failed." << endreq;
    return;
  }
  if (monObject->endOfRun() != this->endOfRun()){
    msg <<MSG::WARNING<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  if (((MonRate*)monObject)->counterMap().size() != this->counterMap().size()){
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to combine MonRate MonObject with counter's size "<<this->counterMap().size() <<" and "<< ((MonRate*)monObject)->counterMap().size() << " failed." << endreq;
    return;
  }
  add((MonRate*) monObject);
}

void MonRate::copyFrom(MonObject * monObject){
  if (monObject->typeName() != this->typeName()) {
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monObject->typeName() << " failed." << endreq;
    return;
  }
  MonRate *mo = (MonRate*) monObject;
  m_endOfRun = mo->endOfRun();
  m_counterMap = mo->counterMap();
  m_comments = mo->comments();
  
  (*m_timeFirstEvInRun) = mo->timeFirstEvInRun();
  (*m_timeLastEvInCycle) = mo->timeLastEvInCycle();  
  (*m_runNumber) = mo->runNumber();  
  (*m_cycleNumber) = mo->cycleNumber();  
  
}

void MonRate::reset(){
//TODO How reset counters ?
//  for (m_counterMapIt = m_counterMap.begin(); m_counterMapIt != m_counterMap.end(); ++m_counterMapIt)
//    (*m_counterMapIt->second) = 0;
  
  (*m_timeFirstEvInRun) = 0; // This is a Time may be we have to let it with the last value 
  (*m_timeLastEvInCycle) = 0; // This is a Time may be we have to let it with the last value 
  (*m_runNumber) = 0;
  (*m_cycleNumber) = 0;
}

void MonRate::add(MonRate *monRate){
  //if (m_int !=NULL)
  for (m_counterMapIt = m_counterMap.begin(); m_counterMapIt != m_counterMap.end(); ++m_counterMapIt)
    (*m_counterMapIt->second) =+  monRate->counter(m_counterMapIt->first);
  
  (*m_timeFirstEvInRun) =+ monRate->timeFirstEvInRun();
  (*m_timeLastEvInCycle) =+ monRate->timeLastEvInCycle();  
  (*m_runNumber) =+ monRate->runNumber();  
  (*m_cycleNumber) =+ monRate->cycleNumber();  
  
}

void MonRate::print(){
  MonObject::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " counter size :"<< m_counterMap.size() << endreq;
  msgStream << MSG::INFO << m_typeName << " values :" << endreq;
  
  for (m_counterMapIt = m_counterMap.begin(); m_counterMapIt != m_counterMap.end(); ++m_counterMapIt)
    msgStream << MSG::INFO << " counter: "<<  m_counterMapIt->first << ", value: " << (*m_counterMapIt->second) << endreq;
  
  msgStream << MSG::INFO << "*************************************"<<endreq;
  
  msgStream << MSG::INFO << " timeFirstEvInRun: "<<  timeFirstEvInRun() << endreq;
  msgStream << MSG::INFO << " timeLastEvInCycle: "<<  timeLastEvInCycle() << endreq;
  msgStream << MSG::INFO << " runNumber: "<<  runNumber() << endreq;
  msgStream << MSG::INFO << " cycleNumber: "<<  cycleNumber() << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  
}

