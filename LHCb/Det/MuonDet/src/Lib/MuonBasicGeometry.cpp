// $Id: MuonBasicGeometry.cpp,v 1.3 2002-10-22 07:11:45 cattanem Exp $
// Include files
// local
#include "MuonDet/MuonBasicGeometry.h"
#include <cstdio>

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
  std::string muonRootLocation="/dd/Structure/LHCb/Muon";
  //log << MSG::INFO << "%%% " << "muon geom" << endreq;
  
  SmartDataPtr<DetectorElement> muonSystem(detSvc,"/dd/Structure/LHCb/Muon"); 
  // log << MSG::INFO << "%%% " << muonSystem->name() << endreq;
  m_stationNumber=0; 
  m_regionNumber=4;   
  if(muonSystem){
    //log << MSG::INFO << "%%% " << muonSystem->name() << endreq;    
  }
  else{
    // log << MSG::INFO << "%%%  muon system null"  << endreq;
  }
  
  IDetectorElement::IDEContainer::iterator itStation;  
  for( itStation = muonSystem->childBegin();
     itStation != muonSystem->childEnd();
    itStation++){
  SmartDataPtr<DetectorElement> muStation(detSvc,
                                        (*itStation)->name());
  if(!muStation){
   log << MSG::ERROR << "Could not read station"
       <<(*itStation)->name() <<endreq;      
  }
  //log << MSG::INFO << "+ " << muStation->name() << endreq;
  std::string stationName=findName(muStation->name(),muonRootLocation); 
  //log << MSG::INFO << "+++ " << stationName << endreq;
   
  m_stationNumber=m_stationNumber+1;
  numsta.push_back(stationName);
  }
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


std::string  MuonBasicGeometry::findName(std::string allname,std::string
                                        rootName){
  std::string::size_type allsize=allname.size();
  //  std::cout<<"allsize "<< allsize <<endl;
  
  std::string::size_type findLastSeparator=allname.rfind("/");
  //  std::cout<<"allsize kl "<< findLastSeparator <<endl;

  std::string name=allname.substr(findLastSeparator+1,allsize);
  //  std::cout<<"allsize name "<<name<<endl;
  
  return name;
  
  
}

