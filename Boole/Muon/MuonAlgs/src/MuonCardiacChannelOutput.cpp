#ifndef MuonAlgs_MuonCardiacChannelOutput_CPP
#define MuonAlgs_MuonCardiacChannelOutput_CPP  1

#include <iostream>

#include "MuonDet/DeMuonDetector.h"

#include "MuonCardiacChannelOutput.h"
#include "MuonCardiacTraceBack.h"
#include "SortPhyChOutInTime.h"


void  MuonCardiacChannelOutput::addPhyChannel(MuonPhysicalChannelOutput* value)
{
  
  m_phChannel.push_back(value);  
  std::vector<MuonHitTraceBack>::iterator iterOrigin;  
  for(iterOrigin=value->hitsTraceBack().begin();
      iterOrigin<value->hitsTraceBack().end();iterOrigin++){
    MuonHitTraceBack* po=static_cast<MuonHitTraceBack*> (&(*iterOrigin));
    //float time= (*iterInHits).hitArrivalTime()
    //m_timeList.push_back(std::pair<float,MuonCardiacTraceBack>(time,pi));
    //	  m_Hits.push_back(po);
    if( po->getMCMuonHistory().isHitFiring() ) {
      MuonCardiacTraceBack pi(po);      
      float time= (float)((*po).hitArrivalTime());      
//      std::cout<<chID()<<" "<<time<<std::endl;
      m_timeList.push_back(std::pair<float,MuonCardiacTraceBack>(time,pi));      
    }    
	}
};

// std::vector<MuonPhysicalChannelOutput*>&  MuonCardiacChannelOutput::retrievePhyChannel()
// {
//   return   m_phChannel;
  
// }

void MuonCardiacChannelOutput::calculateTileID(int & numberTileOutput,
                                               LHCb::MuonTileID tileChID[2],
                                               DeMuonDetector* muonDetector)
{
  
 
  bool debug=false;
//  debug=true;  
  unsigned int station=chID().getStation();
  unsigned int region=chID().getRegion();
  unsigned int chamber=chID().getChamber();
  unsigned int readout=chID().getReadout();
  unsigned int quadrant=chID().getQuadrant();
  unsigned int idX=chID().getChIDX()*muonDetector->getCardiacORX(station*4+region,readout);
  unsigned int idY=chID().getChIDY()*muonDetector->getCardiacORY(station*4+region,readout);
  unsigned int newidX,newidY;
  unsigned int idXGlobal,idYGlobal;
  unsigned int idLogX,idLogY;
  int numberOfTileCreated=0;
  unsigned int numberOfPCX, numberOfPCY;


  if(debug){
    std::cout<<station<<" "<<region<<" "<<chamber<<" "<<quadrant<<" "<<
      idX<<" "<<idY<<std::endl;
  }
  
  newidX=0;
  newidY=0;
  numberOfPCX=0;
  numberOfPCY=0;
  numberTileOutput=0;
  // loop over FE channel readout
  //
  if(debug)std::cout<<muonDetector->readoutInRegion(station,region)<<std::endl;
  
  for (int readoutNumber=0;readoutNumber<(int)muonDetector->
         getLogMapInRegion(station,region);readoutNumber++){
    //
    // check if current readout coincides with one of the LogMap readouts
    //
    if(debug) std::cout<<"logmap type, readout "<<
                muonDetector->getLogMapRType(readoutNumber,station,region)<<" "<<
                readout<<" "<<readoutNumber<<
                std::endl;
    
    if(muonDetector->getLogMapRType(readoutNumber,station,region)==readout){
      // define order of FE channel according to the MuonTileID 
      //conventions: from 0,0 left,bottom to radial coordinates
      //
      for(int countReadout=0; countReadout<muonDetector->
            readoutInRegion(station,region);countReadout++){
        if(  muonDetector->getLogMapRType(readoutNumber,station,region)==
             muonDetector->getReadoutType(countReadout,station,region)){
          numberOfPCX=muonDetector->
            getPhChannelNX(countReadout,station,region);
          numberOfPCY=muonDetector->
            getPhChannelNY(countReadout,station,region);
          if(debug) std::cout<<"channels "<<countReadout<<" "<<station<<" "<<region<<" "
                             <<numberOfPCX<<" "<<
                      numberOfPCY<<std::endl;
          
          
        }
      }
      
      // 
      // FE ch address relative to the chamber transformed 
      //in quadrant reference system
      //
      if(quadrant==0){
        newidX=idX; 
        newidY=idY;
      }else if(quadrant==3){
        newidX=numberOfPCX-idX-1; 
        newidY=idY;                          
      }else if(quadrant==2){
        newidX=numberOfPCX-idX-1; 
        newidY=numberOfPCY-idY-1;
      }else if(quadrant==1){
        newidX=idX;
        newidY=numberOfPCY-idY-1;
      }
      // 
      // FE ch address in the whole quadrant
        //
      LHCb::MuonTileID chaTile;
      StatusCode sc=
        muonDetector-> Chamber2Tile(chamber, station,region,chaTile)   ;
      if(sc.isFailure()&&debug)std::cout<<" non identified the tile "<<endreq;
      
      idXGlobal=newidX+chaTile.nX()*numberOfPCX;
      idYGlobal=newidY+chaTile.nY()*numberOfPCY;
      if(debug)std::cout<<"cha tile "<<chaTile.nX()<<" "<<chaTile.nY()<<std::endl;
      
      if(debug)std::cout<< idXGlobal<<" "<< idYGlobal<<std::endl;
      
      //
      //  compute Logical Channel address now
      //
      idLogX=idXGlobal/muonDetector->
        getLogMapMergex(readoutNumber,station,region);    
      idLogY=idYGlobal/muonDetector->
        getLogMapMergey(readoutNumber,station,region);   
      //
      // create the tile of the phys chan
      //  
      if(debug)std::cout<<  idLogX <<" "<< idLogY<<std::endl;
      if(debug)std::cout<< muonDetector->
                 getLogMapMergex(readoutNumber,station,region)<<" "<<
                 muonDetector->
                 getLogMapMergey(readoutNumber,station,region)<<std::endl;
      
      
      ++numberTileOutput;
      
      MuonLayout layout(muonDetector->getLayoutX(readoutNumber,station,region),
                        muonDetector->
                          getLayoutY(readoutNumber,station,region));   
      if(debug)std::cout<<"Layout "<<muonDetector->getLayoutX(readoutNumber,station,region)<<" "
                        <<muonDetector->getLayoutY(readoutNumber,station,region)<<std::endl;
      
      tileChID[numberOfTileCreated].setLayout(layout);
      tileChID[numberOfTileCreated].setStation(station);
      //        phChTileID[numberOfTileCreated].setReadout(usefullPointer->
      //                                                   getLogMapRType
      //                                                   (readoutNumber,part));
      tileChID[numberOfTileCreated].setRegion(region);
      tileChID[numberOfTileCreated].setQuarter(quadrant);
      tileChID[numberOfTileCreated].setX(idLogX);
      tileChID[numberOfTileCreated].setY(idLogY);        
      ++numberOfTileCreated;
    }else{
    }
  }
  
}

void MuonCardiacChannelOutput::processForDeadTime(float value, float timeGate)
{

  fillhitsTraceBack() ;
  //check the hit contente 
    std::vector<MuonHitTraceBack*>::iterator ithit;
 //   std::cout<<" new ch  size "<<hitsTraceBack().size()<<std::endl;
//   for(ithit=hitsTraceBack().begin();
//       ithit<hitsTraceBack().end();ithit++){
//  std::cout<<" prima info a new hit "<<std::endl;
//  std::cout<<"  prima a new hit "<<(*ithit) ->getMCMuonHistory()<<std::endl;;
//   }
        
 
//  std::cout<<" new cardiaca channel "<<std::endl;
  std::stable_sort(m_timeList.begin(),m_timeList.end(),SortPhyChOutInTime());
  
  double lenghtOfDead=value;
  double timeOfStart=-5000;
  std::vector<std::pair<float,MuonCardiacTraceBack> >::iterator it;
  double timeOfStartOfDeadtime=timeOfStart;;
  double lenghtOfDeadtime=lenghtOfDead;;
  for(it=m_timeList.begin();it<m_timeList.end();it++){ 
    double timeOfHit=(*it).first;
  //  std::cout<<chID()<<" "<<timeOfHit<<std::endl;
    if(timeOfHit<timeOfStartOfDeadtime+lenghtOfDeadtime){
      //        hit in deadtime
      (*it).second.setDeadTime();
      (*it).second.setHitDialogDeadtime();
      
    }else{
      
      //                   hit start the deadtime
      timeOfStartOfDeadtime=timeOfHit;
      
      //         extract the value of lenghtOfDeadtime
    }
    
  }
  
  //now set the firing time in the current BX
  bool fired=false;
  
  for(it=m_timeList.begin();it<m_timeList.end();it++){ 
    double timeOfHit=(*it).first;
        //std::cout<<" time "<<(*it).first<<" "<<((*it).second).getDeadTime()<<std::endl;
    bool dead=((*it).second).getDeadTime();
    if(!dead&&timeOfHit>0&&timeOfHit<timeGate){
      if(fired)std::cout<<" two times "<<timeOfHit<<" "
        <<firingTime()<<std::endl;
      setFiringTime( timeOfHit);
      //      m_phChInfo=((*it).second).getPhyChOut()->phChInfo();
      fired=true;
      setChInfoFiring((*it).second);
    
      //std::cout<<" fired "<<m_phChInfo.isAlive()<<" "<<
      //  (((*it).second).getPhyChOut()->phChInfo()).isAlive()<<std::endl;
//      break;
      
    }
  }
  //if all not fired put the info of the one that would have 
  //fired without deadtime
  if(fired) return;
  



  for(it=m_timeList.begin();it<m_timeList.end();it++){ 
    //if(((*it).second).getPhyChOut()-> phChInfo().isAlive()){
      //       m_phChInfo=((*it).second).getPhyChOut()->phChInfo();
      fired=true;
      setChInfoNotFiringDueDeadtime((*it).second);
      break;
      //}
  }
if(fired)return;
  // if there was no firing then set the first one
  //  it=m_timeList.begin();
  setChInfoNotFiring();
  //m_phChInfo=(*(m_phChannel.begin())->phChInfo();

}
void MuonCardiacChannelOutput::fillhitsTraceBack() 
{ 
  //std::cout<<" hit size deve "<<m_Hits.size()<<std::endl;
  
  // std::vector<MuonHitTraceBack*> output_list;
  std::vector<MuonPhysicalChannelOutput*>::iterator it;  
  std::vector<MuonHitTraceBack>::iterator iterOrigin;
  for(it=m_phChannel.begin();it<m_phChannel.end();it++){
    std::vector<MuonHitTraceBack>::iterator iterOrigin;
    
    for(iterOrigin=(*it)->hitsTraceBack().begin();
        iterOrigin<(*it)->hitsTraceBack().end();iterOrigin++){
      MuonHitTraceBack* po=static_cast<MuonHitTraceBack*> (&(*iterOrigin));
      //float time= (*iterInHits).hitArrivalTime()
      //m_timeList.push_back(std::pair<float,MuonCardiacTraceBack>(time,pi));
      //	  m_Hits.push_back(po);
      m_Hits.push_back(po);
      //    std::cout<<" ma che vaolo "<<po->getMCMuonHistory()<<std::endl;
      // m_Hits
    }
    
  }
  
  
  
//   std::cout<<" hit size "<<m_Hits.size()<<std::endl;  
//   std::vector<MuonHitTraceBack*>::iterator iterTest;

//   for(iterTest=m_Hits.begin();iterTest<m_Hits.end();iterTest++){
//     std::cout<< " disparata "<<(*iterTest)->getMCMuonHistory()<<std::endl;
//   }
  
  
  //create the list  now 
}

void MuonCardiacChannelOutput::setNotFiringDigit()
{

//  std::vector<std::pair<float,MuonCardiacTraceBack> >::iterator it;
//  for(it=m_timeList.begin();it<m_timeList.end();it++){ 
//    MuonCardiacTraceBack tt=it->second;
//    //   MuonPhysicalChannelOutput* ph=tt.getPhyChOut();
//    std::vector<MuonHitTraceBack>::iterator iterOnHits;
//    for(iterOnHits=ph->hitsTraceBack().begin();
//        iterOnHits<ph->hitsTraceBack().end();
//        iterOnHits++){
//      (*iterOnHits).getMCMuonHistory().setFiredFlag(0);			 
//    }
   
//  }
 
}
void MuonCardiacChannelOutput::setFiringTime()
{
  std::vector<std::pair<float,MuonCardiacTraceBack> >::iterator it;
  for(it=m_timeList.begin();it<m_timeList.end();it++){ 
    double timeOfHit=(*it).first;
    bool dead=((*it).second).getDeadTime();
    if(!dead){
      if(timeOfHit>0&&  timeOfHit<25){
        setFiringTime(timeOfHit);
//        return;
        
      }
      
    }
    
  }
return;
}



// std::vector<MuonHitTraceBack>  MuonCardiacChannelOutput::getAllMuonHits()
// {
//   std::vector<MuonHitTraceBack> listoutput;
  
// }

//void Muon
void MuonCardiacChannelOutput::setChInfoFiring(MuonCardiacTraceBack pointer)
{
  //this hit has fired the electronics ...
  //so get the PH to which belongs and copy the chinfo
  MuonPhysicalChannelOutput* phchannel=getPhCh(pointer);
  m_phChInfo=phchannel->phChInfo();

  

}
void MuonCardiacChannelOutput::setChInfoNotFiringDueDeadtime(MuonCardiacTraceBack pointer)
{
  //this hit has fired the electronics ...
  //but the dialog is in deadtime
  MuonPhysicalChannelOutput* phchannel=getPhCh(pointer);
  m_phChInfo=phchannel->phChInfo();
  m_phChInfo.setDialogDeadtimeDigit(1);
  m_phChInfo.setAliveDigit(0);
  

}
void MuonCardiacChannelOutput::setChInfoNotFiring()
{
  //there is no hit that has fired the electronics ...
  //so get the first ch and copy such info
//std::cout<<(*(m_phChannel.begin()))->phChInfo().isAlive()<<std::endl;
  m_phChInfo=(*(m_phChannel.begin()))->phChInfo();
               m_phChInfo.setAliveDigit(0);
}
  

MuonPhysicalChannelOutput* MuonCardiacChannelOutput::getPhCh(MuonCardiacTraceBack pointer)
{
  std::vector<MuonPhysicalChannelOutput*>::iterator itph;
  
  for(itph=m_phChannel.begin();itph<m_phChannel.end();itph++){
    
    
    std::vector<MuonHitTraceBack>::iterator iterOrigin;
    MuonPhysicalChannelOutput* phreturn=static_cast<MuonPhysicalChannelOutput*> ((*itph));
    for(iterOrigin=(*itph)->hitsTraceBack().begin();
        iterOrigin<(*itph)->hitsTraceBack().end();iterOrigin++){
      MuonHitTraceBack* po=static_cast<MuonHitTraceBack*> (&(*iterOrigin));
      if(po==pointer.getHitTrace())return phreturn;;
    }
  }
  return NULL;
}

#endif




