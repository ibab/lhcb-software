// $Id: MuonBasicGeometry.cpp,v 1.8 2010-03-17 16:19:07 cattanem Exp $

// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"


// Detector description classes
#include "DetDesc/DetectorElement.h"

// local
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/MuonNamespace.h"

namespace {
std::string  findName(const std::string& allname,
                      const std::string& /*rootName*/){
  std::string::size_type allsize=allname.size();
  //  std::cout<<"allsize "<< allsize <<endl;
  
  std::string::size_type findLastSeparator=allname.rfind("/");
  //  std::cout<<"allsize kl "<< findLastSeparator <<endl;

  std::string name=allname.substr(findLastSeparator+1,allsize);
  //  std::cout<<"allsize name "<<name<<endl;
  
  return name;
  
}
}
//-----------------------------------------------------------------------------
// Implementation file for class : MuonBasicGeometry
//
// 2002-10-07 : Alessia Satta
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonBasicGeometry::MuonBasicGeometry(IDataProviderSvc* detSvc ,
                                     IMessageSvc * msgSvc ) {
  MsgStream log(msgSvc, "MuonBasicGeometry");
  //log << MSG::INFO << "%%% " << "muon geom" << endmsg;
  SmartDataPtr<DetectorElement> muonSystem(detSvc,DeMuonLocation::Default); 
  m_stationNumber=0; 
  m_regionNumber=4;   
  m_isM1defined=false;
  std::string stationName;  

 // for(itStation=muonSystem->childBegin(); itStation<muonSystem->childEnd(); itStation++){
 //   // Test if M1 is defined in the DB
 //   m_isM1defined |= (name.find("/M1") != name.npos);
 // }
 // bool isM1defined=m_isM1defined;
 // log<<MSG::INFO<< "Retrieved M1 definition status: " << isM1defined <<endmsg;

  for(auto  itStation = muonSystem->childBegin();
     itStation != muonSystem->childEnd();
    itStation++){
    const std::string& name=(*itStation)->name();
    m_isM1defined |= (name.find("/M1") != name.npos);
    if(name.find("/MF") != name.npos) continue;
    SmartDataPtr<DetectorElement> muStation(detSvc,(*itStation)->name());
    if(!muStation){
      log << MSG::ERROR << "Could not read station"
        <<(*itStation)->name() <<endmsg;      
    }
    //log << MSG::INFO << "+ " << muStation->name() << endmsg;
    //    log << MSG::INFO << "+ " << muStation->name() << endmsg; //GP
    stationName=findName(muStation->name(),DeMuonLocation::Default); 
    //log << MSG::INFO << "+++ " << stationName << endmsg;
   
    //    m_stationNumber=m_stationNumber+1;
    m_stations.emplace_back(stationName,m_stationNumber);
    //    log << MSG::INFO << "+++ " << (m_stations.at(m_stationNumber)).first 
    //	<< " is station: "<<(m_stations.at(m_stationNumber)).second <<endmsg; //GP

    m_stationNumber++; //GP
    
  }

  //  bool isM1defined=m_isM1defined;
  //  log << MSG::DEBUG << "Retrieved M1 definition status: " << isM1defined <<endmsg;
  //  log << MSG::INFO << "Retrieved M1 definition status: " << isM1defined <<endmsg; //GP
  m_partition=m_stationNumber*m_regionNumber;
  //  log << MSG::INFO << "stations partitions " <<m_stationNumber<<" "<<m_partition <<endmsg; //GP
  
}
//=============================================================================



int MuonBasicGeometry::getStations(  ) {
  return m_stationNumber;  
}

int MuonBasicGeometry::getRegions(  ) {
  return m_regionNumber;  
}
int MuonBasicGeometry::getPartitions(  ) {
  return m_partition;  
}
std::string MuonBasicGeometry::getStationName(int station  ) {
  return (m_stations.at(station)).first;  
}

int MuonBasicGeometry::getStationNumber(std::string stationName){
  std::vector< std::pair<std::string, int> >::iterator sIt = m_stations.begin();

  for(int i=0; sIt != m_stations.end(); sIt++, i++){
    if( (*sIt).first == stationName) return i;
  }
  return -1;
}



int MuonBasicGeometry::retrieveM1status() {
  return m_isM1defined;
}

