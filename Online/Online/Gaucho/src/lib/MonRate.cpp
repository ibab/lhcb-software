#include "Gaucho/MonRate.h"

MonRate::MonRate(IMessageSvc* msgSvc, const std::string& source, int version):
  MonProfile(msgSvc, source, version)
{
  m_typeName=s_monRate;
  m_dimPrefix="MonR";
  m_runNumber = new int(0);
  m_triggerConfigurationKey = new unsigned int(0);
  m_cycleNumber = new int(0);
  m_deltaT = new double(0);
  m_offsetTimeFirstEvInRun = new double(0);
  m_offsetTimeLastEvInCycle = new double(0);
  m_offsetGpsTimeLastEvInCycle = new double(0);
  isServer = true;
  m_maxNumCounters = 0;
  m_numCounters = 0;
}
  
MonRate::~MonRate(){
 if (m_profile) { delete m_profile; m_profile = 0; }
}

void MonRate::save(boost::archive::binary_oarchive & ar, const unsigned int version){
  MonObject::save(ar, version);
  MsgStream msg = createMsgStream();
  try {
     if (isServer) {
       m_numCounters = m_counterMap.size();
//       if (m_profile) { delete m_profile; m_profile = 0; }
       if (m_profile) { m_profile->Reset(); }
       else {
         m_profile = new TProfile("MRprofile","MonRate Profile", m_maxNumCounters+8, 0, m_maxNumCounters+8);
      	  //set m_profile as the one used by MonProfile
	  MonProfile::setProfile(m_profile);  
       }      
       m_profile->Fill(0.50, *m_offsetTimeFirstEvInRun, 1.00);
       m_profile->Fill(1.50, *m_offsetTimeLastEvInCycle, 1.00);
       m_profile->Fill(2.50, *m_offsetGpsTimeLastEvInCycle, 1.00);
       m_profile->Fill(3.50, 1.00, 1.00);
       m_profile->Fill(4.50, (double) (*m_runNumber), 1.00);
       m_profile->Fill(5.50, (double) (*m_triggerConfigurationKey), 1.00);
       m_profile->Fill(6.50, (double) (*m_cycleNumber), 1.00);
       m_profile->Fill(7.50, (double) (*m_deltaT), 1.00);   
       msg <<MSG::DEBUG<<"bin[1]=" << (*m_offsetTimeFirstEvInRun) << ", OffsetTimeFirstEvInRun" << endreq;    
       msg <<MSG::DEBUG<<"bin[2]=" << (*m_offsetTimeLastEvInCycle) << ", OffsetTimeLastEvInCycle" << endreq;
       msg <<MSG::DEBUG<<"bin[3]=" << (*m_offsetGpsTimeLastEvInCycle) << ", OffsetGpsTimeLastEvInCycle" << endreq;
       msg <<MSG::DEBUG<<"bin[4]=" << 1 << ", one" << endreq;
       msg <<MSG::DEBUG<<"bin[5]=" << (*m_runNumber) << ", RunNumber" << endreq;
       msg <<MSG::DEBUG<<"bin[6]=" << (*m_triggerConfigurationKey) << ", TCK" << endreq;
       msg <<MSG::DEBUG<<"bin[7]=" << (*m_cycleNumber) << ", CycleNumber" << endreq;
       msg <<MSG::DEBUG<<"bin[8]=" << (*m_deltaT) << ", deltaT" << endreq;            
       int i = 9;
       for (m_counterMapIt = m_counterMap.begin(); m_counterMapIt != m_counterMap.end(); m_counterMapIt++) {
        
          if (m_counterMapIt->second.second.first.compare("int") ==0 ){
            msg <<MSG::DEBUG<<"Counter :" << m_counterMapIt->first << " is int "<< endreq;
            m_profile->Fill((double)i - 0.5, (*(int*)(m_counterMapIt->second.second.second)), 1.00);
          } 
          else if (m_counterMapIt->second.second.first.compare("double") ==0 ){
            msg <<MSG::DEBUG<<"Counter :" << m_counterMapIt->first << " is double "<< endreq;
	    msg <<MSG::DEBUG<<"Counter :" << (*(double*)(m_counterMapIt->second.second.second)) << endreq;
            m_profile->Fill((double)i - 0.5, (*(double*)(m_counterMapIt->second.second.second)), 1.00);
          } 
          else if (m_counterMapIt->second.second.first.compare("long") ==0 ){
            msg <<MSG::DEBUG<<"Counter :" << m_counterMapIt->first << " is long  "<< endreq;
            m_profile->Fill((double)i - 0.5, (*(long*)(m_counterMapIt->second.second.second)), 1.00);
          }
          else if (m_counterMapIt->second.second.first.compare("StatEntityflag") ==0 ){
            msg <<MSG::DEBUG<<"Counter :" << m_counterMapIt->first << " is StatEntity (flag) "<< endreq;
            StatEntity counter = (*(StatEntity*) m_counterMapIt->second.second.second);
	    m_profile->Fill((double)i - 0.5, counter.flag(), 1.00);
	    msg <<MSG::DEBUG<<"Counter :" << counter.flag() << endreq;
          }  
          else if (m_counterMapIt->second.second.first.compare("StatEntitynEntries") ==0 ){
            msg <<MSG::DEBUG<<"Counter :" << m_counterMapIt->first << " is StatEntity (nEntries) "<< endreq;
            StatEntity counter = (*(StatEntity*) m_counterMapIt->second.second.second);
	    m_profile->Fill((double)i - 0.5, counter.nEntries(), 1.00);
	    msg <<MSG::DEBUG<<"Counter :" << counter.nEntries() << endreq;
          }  
         else {
	    //sometimes compare doesn't see it. try find.
	    if (m_counterMapIt->second.second.first.find("StatEntityflag") != std::string::npos) {
               msg <<MSG::DEBUG<<"Counter :" << m_counterMapIt->first << " is StatEntity (flag) "<< endreq;
               StatEntity counter = (*(StatEntity*) m_counterMapIt->second.second.second);
	       m_profile->Fill((double)i - 0.5, counter.flag(), 1.00);
	       msg <<MSG::DEBUG<<"Counter :" << counter.flag() << endreq;	   	    	    	    
	    }
	    else {
               msg <<MSG::WARNING<<"Counter "<< m_counterMapIt->first << " has a type "<< m_counterMapIt->second.second.first << " that is not supported by MonRate." << endreq;
	       return;
	    }   
          }
          i++;
        }
    
        m_profile->GetXaxis()->SetBinLabel(1, "OffsetTimeFirstEvInRun");
        m_profile->GetXaxis()->SetBinLabel(2, "OffsetTimeLastEvInCycle");
        m_profile->GetXaxis()->SetBinLabel(3, "OffsetGpsTimeLastEvInCycle");
        m_profile->GetXaxis()->SetBinLabel(4, "One");
        m_profile->GetXaxis()->SetBinLabel(5, "RunNumber");
        m_profile->GetXaxis()->SetBinLabel(6, "TCK");
        m_profile->GetXaxis()->SetBinLabel(7, "CycleNumber");
        m_profile->GetXaxis()->SetBinLabel(8, "deltaT");
        
        i = 9;
        for (m_counterMapIt = m_counterMap.begin(); m_counterMapIt != m_counterMap.end(); m_counterMapIt++) {
          msg <<MSG::DEBUG<<"label description: " << (*(m_counterMapIt->second.first)).c_str() << endreq;
          if ((*(m_counterMapIt->second.first)).c_str()==0)  m_profile->GetXaxis()->SetBinLabel(i, "no label");
	  else m_profile->GetXaxis()->SetBinLabel(i, (*(m_counterMapIt->second.first)).c_str());
          i++;
        }   
      }
   }    
   catch (const std::exception &ex){
    msg << MSG::WARNING << "std::exception: " << ex.what() << ". MonRate not saved." << endreq;
    return;
  }  
  catch (...){
    msg << MSG::WARNING << "unrecognized exception. MonRate not saved."<< endreq;
    return;
  } 
  //crashes at stop?
  MonProfile::save(ar, version); 
  ar & m_numCounters;
  if (isServer) {
     if (m_profile) { m_profile->Reset(); }
  }
}

void MonRate::load(boost::archive::binary_iarchive  & ar)
{
  MsgStream msg = createMsgStream();
  isServer = false;
  MonObject::load(ar);
  MonProfile::load(ar);
  ar & m_numCounters;
  
}

void MonRate::combine(MonObject * monObject) {
  MsgStream msg = createMsgStream();
  if (monObject==0) return;
  if (monObject->typeName() != this->typeName()){
    msg <<MSG::ERROR<<"Trying to combine "<<this->typeName() <<" and "<< monObject->typeName() << " failed." << endreq;
    return;
  }
  if (monObject->endOfRun() != this->endOfRun()){
    //this is normal; after a fast run change the buffers are still full with 
    //events from the previous run
  //  msg <<MSG::DEBUG<<"Trying to combine two objects with diferent endOfRun flag failed." << endreq;
    return;
  }
  MonProfile::combine(monObject);  
}

void MonRate::copyFrom(MonObject * monObject){
  MsgStream msg = createMsgStream();
  isServer = false;
  if (monObject->typeName() != this->typeName()) {
    MsgStream msgStream = createMsgStream();
    msgStream <<MSG::ERROR<<"Trying to copy "<<this->typeName() <<" and "<<monObject->typeName() << " failed." << endreq;
    return;
  }
  MonProfile::copyFrom(monObject);
  
  m_numCounters = ((MonRate*)monObject)->numCounters();
}


void MonRate::reset(){
  MonProfile::reset();
}

void MonRate::print(){
  //MonObject::print();
  MonProfile::print();
  MsgStream msgStream = createMsgStream();
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << " runNumber: "<<  runNumber() << endreq;
  msgStream << MSG::INFO << " TCK: "<<  triggerConfigurationKey() << endreq;
  msgStream << MSG::INFO << " cycleNumber: "<<  cycleNumber() << endreq;
  msgStream << MSG::INFO << " deltaT: "<<  deltaT() << endreq;
  msgStream << MSG::INFO << " offsetTimeFirstEvInRun: "<<  offsetTimeFirstEvInRun() << endreq;
  msgStream << MSG::INFO << " offsetTimeLastEvInCycle: "<<  offsetTimeLastEvInCycle() << endreq;
  msgStream << MSG::INFO << " offsetGpsTimeLastEvInCycle: "<<  offsetGpsTimeLastEvInCycle() << endreq;
  msgStream << MSG::INFO << "*************************************"<<endreq;
  msgStream << MSG::INFO << m_typeName << " counter size :"<< m_counterMap.size() << endreq;
  msgStream << MSG::INFO << m_typeName << " values :" << endreq;
  
  
  for (m_counterMapIt = m_counterMap.begin(); m_counterMapIt != m_counterMap.end(); ++m_counterMapIt)  {
    msgStream << MSG::INFO << " counter: "<<  m_counterMapIt->first  << endreq;  
  }
   msgStream << MSG::INFO << "*************************************"<<endreq;
}

//BOOST_CLASS_VERSION(MonRate, 1)
