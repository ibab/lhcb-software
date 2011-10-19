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



void MuonCardiacChannelOutput::processForDeadTime(float value, float timeGate)
{

  fillhitsTraceBack() ;
  //check the hit content
  std::vector<MuonHitTraceBack*>::iterator ithit;
  
  
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
      //(*it).second.setHitDialogDeadtime();
      //now add the info at the level of PCChinfo
      //      if(timeOfHit>0&&timeOfHit<timeGate){
      
      //  MuonPhysicalChannelOutput* pchtochange= getPhCh ((*it).second);
      //  pchtochange-> phChInfo().setDialogDeadtimeDigit(1);
      // }
      
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
    
    bool dead=((*it).second).getDeadTime();
    if(!dead&&timeOfHit>0&&timeOfHit<timeGate){
      if(fired)std::cout<<" two times "<<timeOfHit<<" "
        <<firingTime()<<std::endl;
      setFiringTime( timeOfHit);
      //      m_phChInfo=((*it).second).getPhyChOut()->phChInfo();
      fired=true;
      setChInfoFiring((*it).second);
      m_fired=true;          
    }
  }
  //if all not fired put the info of the one that would have 
  //fired without deadtime
  if(fired) return;
  


  UpdateMCInfo();
  
 

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


void MuonCardiacChannelOutput::UpdateMCInfo()
{
  std::vector<MuonPhysicalChannelOutput*>::iterator iterpc;
  
  //loop on all ph ch and create the MCMuonDigitInfo for the Cardiac ch.
  if(!m_fired){
    //set the MCMuonDigitInfo equal to the first 
    m_phChInfo.setDigitInfo(((*m_phChannel.begin())->phChInfo()).DigitInfo());
    
    for(iterpc=m_phChannel.begin();iterpc!=m_phChannel.end(); iterpc++){
      if(iterpc==m_phChannel.begin())continue;
      LHCb::MCMuonDigitInfo other= (*iterpc)->phChInfo();
      if( other.isInDeadTime()) m_phChInfo.setDeadtimeDigit(1);
      if( other.isDeadForChamberInefficiency()) m_phChInfo.setChamberInefficiencyDigit(1);
      if( other.isDeadByTimeJitter()) m_phChInfo.setTimeJitteredDigit(1);
      if( other.isDeadByTimeAdjustment()) m_phChInfo.setTimeAdjDigit(1);
   
      if( other.isDeadByGeometry()) m_phChInfo.setGeometryInefficiency(1);
      if( other.isInDialogDeadTime()) m_phChInfo.setDialogDeadtimeDigit(1);          
    }
     m_phChInfo.setAliveDigit(0);
  }
  //now the retrieve of the firing info and setting of it
  
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

     m_phChInfo.setAliveDigit(1);

}
  /*void MuonCardiacChannelOutput::setChInfoNotFiringDueDeadtime(MuonCardiacTraceBack pointer)
{
  //this hit has fired the electronics ...
  //but the dialog is in deadtime
  MuonPhysicalChannelOutput* phchannel=getPhCh(pointer);
  m_phChInfo=phchannel->phChInfo();
  m_phChInfo.setDialogDeadtimeDigit(1);
  m_phChInfo.setAliveDigit(0);
  

  }*/
   /*void MuonCardiacChannelOutput::setChInfoNotFiring()
{
  //there is no hit that has fired the electronics ...
  //so get the first ch and copy such info
//std::cout<<(*(m_phChannel.begin()))->phChInfo().isAlive()<<std::endl;
  m_phChInfo=(*(m_phChannel.begin()))->phChInfo();
               m_phChInfo.setAliveDigit(0);
               }*/
  

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




