// Include files
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"


// Detector description classes
#include "DetDesc/DetectorElement.h"

// local
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/MuonNamespace.h"

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
  
  IDetectorElement::IDEContainer::iterator itStation; 

 // for(itStation=muonSystem->childBegin(); itStation<muonSystem->childEnd(); itStation++){
 //   // Test if M1 is defined in the DB
 //   m_isM1defined |= (name.find("/M1") != name.npos);
 // }
 // bool isM1defined=m_isM1defined;
 // log<<MSG::INFO<< "Retrieved M1 definition status: " << isM1defined <<endmsg;

  for( itStation = muonSystem->childBegin();
     itStation != muonSystem->childEnd();
    itStation++){
    std::string name=((*itStation)->name()).c_str();
    m_isM1defined |= (name.find("/M1") != name.npos);
    if(name.find("/MF") != name.npos) continue;
    SmartDataPtr<DetectorElement> muStation(detSvc,(*itStation)->name());
    if(!muStation){
      log << MSG::ERROR << "Could not read station"
        <<(*itStation)->name() <<endmsg;      
    }
    //log << MSG::INFO << "+ " << muStation->name() << endmsg;
    std::string stationName=findName(muStation->name(),DeMuonLocation::Default); 
    //log << MSG::INFO << "+++ " << stationName << endmsg;
   
    m_stationNumber=m_stationNumber+1;
    numsta.push_back(stationName);
    
  }
  bool isM1defined=m_isM1defined;
  if( log.level() <= MSG::DEBUG )
    log << MSG::DEBUG << "Retrieved M1 definition status: " << isM1defined <<endmsg;
  m_partition=m_stationNumber*m_regionNumber;
  
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
  return numsta[station];  
}


std::string  MuonBasicGeometry::findName(std::string allname,
                                         std::string /*rootName*/){
  std::string::size_type allsize=allname.size();
  //  std::cout<<"allsize "<< allsize <<endl;
  
  std::string::size_type findLastSeparator=allname.rfind("/");
  //  std::cout<<"allsize kl "<< findLastSeparator <<endl;

  std::string name=allname.substr(findLastSeparator+1,allsize);
  //  std::cout<<"allsize name "<<name<<endl;
  
  return name;
  
  
}

int MuonBasicGeometry::retrieveM1status() {
  return m_isM1defined;
}

