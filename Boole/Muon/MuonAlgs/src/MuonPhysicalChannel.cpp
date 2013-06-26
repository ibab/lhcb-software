#ifndef MUONALGS_MUONPHYSICALCHANNEL_CPP
#define MUONALGS_MUONPHYSICALCHANNEL_CPP 1 

#include "MuonPhysicalChannel.h"
#include "SortHitsInTime.h"


double MuonPhysicalChannel::m_timeGate;
double MuonPhysicalChannel::m_timeBX;

void MuonPhysicalChannel::applyTimeJitter(){
  std::vector<MuonHitTraceBack>::iterator iter ;
  // 		double deltaT=		response->extractTimeJitter();
  //		cout<<" time jitter"<<deltaT<<" "<<phChID()->getStation()<<
  //" "<<phChID()->getRegion()<<endl;
  for (iter= m_Hits.begin();iter< m_Hits.end();iter++){
    if((!(*iter).getMCMuonHitOrigin().getXTalkNature())&&
       (!(*iter).getMCMuonHitOrigin().getElectronicNoiseNature())){
      double originalTime=(*iter).hitArrivalTime();
      double deltaT=		response->extractTimeJitter();	
      //				 	cout<<" time jitter"<<deltaT<<" "<<phChID()->getFETile()
      //<<" "<<originalTime<<endl;		 
      (*iter).setHitArrivalTime(originalTime+deltaT);	
      //					if(m_ID.getStation()==1&&m_ID.getRegion()==2)
      //					cout<<" time jitter"<<*(phChID())<< " "<<deltaT<<" "
      //<<" "<<originalTime<<" "<<(*iter).hitArrivalTime()<<endl;
      if((*iter).getMCMuonHitOrigin().getBX()==LHCb::MuonBXFlag::CURRENT){
        if(originalTime>0.0&&originalTime<m_timeGate){
          if((*iter).hitArrivalTime()<0.||(*iter).hitArrivalTime()>m_timeGate){
            (*iter).getMCMuonHistory().setHitOutByTimeJitter((unsigned int)1);
          }
        }
      }	 
    }			 	 
  }	 	 		
}

void MuonPhysicalChannel::applyGeoInefficiency(){
  /* MC 2013-06-26: commenting out, since this code has no effect.
   * See Coverity defect 17443
  std::vector<MuonHitTraceBack>::iterator iter ;
  bool drop=false;
  for (iter= m_Hits.begin();iter< m_Hits.end();iter++){
    if((!(*iter).getMCMuonHitOrigin().getXTalkNature())&&
       (!(*iter).getMCMuonHitOrigin().getElectronicNoiseNature())){
      if(drop){
        (*iter).getMCMuonHistory().
          setGeometricalNotAcceptedHitHistory((unsigned int)1);
      }
    }	 
  }
  */
}
	
void MuonPhysicalChannel::applyChamberInefficiency(){
  std::vector<MuonHitTraceBack>::iterator iter ;
  bool keep=true;
  for (iter= m_Hits.begin();iter< m_Hits.end();iter++){
    if((!(*iter).getMCMuonHitOrigin().getXTalkNature())&&
       (!(*iter).getMCMuonHitOrigin().getElectronicNoiseNature())&&
       (!(*iter).getMCMuonHitOrigin().getChamberNoiseNature())){
      keep=response->surviveInefficiency();
      if(!keep){
        (*iter).getMCMuonHistory().
          setKilledByChamberInefficiencyHit((unsigned int)1);
      }	 
    }	  			 
  };
}


void MuonPhysicalChannel::createXTalkChannel(MuonHitTraceBack* iter,
                                             unsigned int newChannel,
                                             int axis, 
                                             MuonPhysicalChannel* newPhCh){
  double	  time=iter->hitArrivalTime();
  if(axis==0){
    newPhCh->	phChID()->setPhChIDX(newChannel);	
  }else if (axis==1){
    newPhCh->	phChID()->setPhChIDY(newChannel);	
  }
  MuonHitTraceBack* pointerToHit=new MuonHitTraceBack;
  pointerToHit->setHitArrivalTime(time);							
  pointerToHit->
    getMCMuonHitOrigin().setHitNature(LHCb::MuonOriginFlag::XTALK);
  pointerToHit->getMCMuonHitOrigin().
    setBX((*iter).getMCMuonHitOrigin().getBX());	
  pointerToHit->getMCMuonHistory().
    setBXOfHit((*iter).getMCMuonHistory().BX());
  pointerToHit->getMCMuonHistory().
    setNatureOfHit(LHCb::MuonOriginFlag::XTALK);
  
  newPhCh->hitsTraceBack().push_back(*pointerToHit);
  delete pointerToHit;														
}

void MuonPhysicalChannel::fireXTalkChannels(int axis, int numberOfXTalkHits,
                                            MuonHitTraceBack* iter,unsigned int
                                            indexOfChannel, int direction, 
                                            int maxChannel, 
                                            std::vector<MuonPhysicalChannel*>&
                                            hitDueToXTalk){
  unsigned int FETile=phChID()->getID();
  int newChannel;			
  //		double	  time=iter->hitArrivalTime();
  
  if(numberOfXTalkHits!=0){
    if(numberOfXTalkHits%2==0){
      // even										 
      for(int i=-numberOfXTalkHits/2; i<=numberOfXTalkHits/2;i++){  	
        if(i!=0){					   
          newChannel=indexOfChannel+i;					  				
          if(newChannel>=0&&newChannel<maxChannel){
            MuonPhysicalChannel* newPhCh = new 
              MuonPhysicalChannel(FETile,m_timeGate,m_timeBX); 
            createXTalkChannel(iter,(unsigned int)newChannel,axis,newPhCh);
            hitDueToXTalk.push_back(newPhCh); 	
          }
        }
      }
    }		
    else{
      /// odd
      newChannel=indexOfChannel +direction; 
      if(newChannel>=0&&newChannel<maxChannel){
        MuonPhysicalChannel* newPhCh = new 
          MuonPhysicalChannel(FETile,m_timeGate,m_timeBX); 
        createXTalkChannel(iter,(unsigned int)newChannel,axis,newPhCh);
        hitDueToXTalk.push_back(newPhCh); 	
      }	
      for(int i=-(numberOfXTalkHits-1)/2; i<=(numberOfXTalkHits-1)/2;i++){  	
        if(i!=0){					   
          newChannel=indexOfChannel+i;					  				
          if(newChannel>=0&&newChannel<maxChannel){
            MuonPhysicalChannel* newPhCh = new 
              MuonPhysicalChannel(FETile,m_timeGate,m_timeBX); 
            createXTalkChannel(iter,(unsigned int)newChannel,axis,newPhCh);
            hitDueToXTalk.push_back(newPhCh); 	
          }
        }
      }
    }						
  } 
}

void MuonPhysicalChannel::applyXTalk(int phChInX[2], int phChInY[2], 
                                     std::vector<MuonPhysicalChannel*>&
                                     hitDueToXTalk){			 
  int numberOfXTalkHitsX=0;
  int numberOfXTalkHitsY=0;
  std::vector<MuonHitTraceBack>::iterator iter;	
  double coord[4];	
  double	xLeft;
  double	xRight ;
  double	yUp ;
  double	yDown ;
  double x,y;
  
  unsigned int nx=	phChID()->getPhChIDX()	;
  unsigned int ny=	phChID()->getPhChIDY()	;
  int readoutType= phChID()->getReadout();
  int directionx,directiony;
  
  for (iter= m_Hits.begin();iter< m_Hits.end();iter++){
    if((!(*iter).getMCMuonHitOrigin().getXTalkNature())&&
         (!(*iter).getMCMuonHitOrigin().getElectronicNoiseNature())){
      // reject hit that has not produced any signal by its own	
      // double time=iter->hitArrivalTime(); // never used
      if(!(*iter).getMCMuonHistory().isKilledByChamberInefficiency()&&
         !(*iter).getMCMuonHistory().isHitOutGeoAccemtance()){			
        coord[0]=*((*iter).getCordinate());
        coord[1]=*((*iter).getCordinate()+1);
        coord[2]=*((*iter).getCordinate()+2);
        coord[3]=*((*iter).getCordinate()+3);
        xLeft= coord[0];
        xRight=coord[2] ;
        yUp=coord[3] ;
        yDown=coord[1] ;
        if(xLeft<xRight){
          x=xLeft ;
          directionx =-1;
        }
        else {
          x=xRight ; 
          directionx =1;
        }
        if(yDown<yUp){
          y=yDown ;						
          directiony =-1;
        }
        else {
          y=yUp ;
              directiony =1;
        }
						
        /// here the XTalk probability is evaluated so numberOfXTalkHits 
        //modified
        //						if(m_ID.getStation()==1&&m_ID.getRegion()==0)cout
        //<<"readoutType "<<m_ID.getReadout()<<endl;
        numberOfXTalkHitsX=response->extractXTalkX(x)-1;
        numberOfXTalkHitsY=response->extractXTalkY(y)-1;
        //int azzerax=hitDueToXTalk.size();
        //						if(m_ID.getStation()==1&&m_ID.getRegion()==0)cout
        //<<"arci "<<numberOfXTalkHitsX<<" "<<numberOfXTalkHitsY<<endl;
        //						if(m_ID.getStation()==0&&m_ID.getRegion()==0)
        //						cout<<"direction "<<directionx<<" "<<directiony<<" "<<
        //						 x<<" "<<y<<endl;
        if(numberOfXTalkHitsX)fireXTalkChannels(0,numberOfXTalkHitsX,
                                                &(*iter),
                                                nx, directionx, 
                                                phChInX[readoutType],  
                                                hitDueToXTalk) ;        
        //int yu=hitDueToXTalk.size();
        //						if(m_ID.getStation()==0&&m_ID.getRegion()==0)cout
        //<<"size in X "<<hitDueToXTalk.size()-azzerax<<endl;
        
        if(numberOfXTalkHitsY)fireXTalkChannels(1,numberOfXTalkHitsY,
                                                 &(*iter),
                                                ny, directiony, 
                                                phChInY[readoutType],  
                                                hitDueToXTalk) ;
        //					if(m_ID.getStation()==0&&m_ID.getRegion()==0)cout
        //<<"size in Y "<<hitDueToXTalk.size()-yu<<endl;
			}
		}
	}			
}

void MuonPhysicalChannel:: addToPC(MuonPhysicalChannel* pointerOfPCToAdd){
  std::vector<MuonHitTraceBack>::iterator iter;	
  for (iter= (pointerOfPCToAdd->hitsTraceBack()).begin();iter<
         (pointerOfPCToAdd->hitsTraceBack()).end();iter++){
    m_Hits.push_back(*iter);
  }	
}

void MuonPhysicalChannel::sortInTimeHits(){  
  std::stable_sort(m_Hits.begin(),m_Hits.end(),SortHitsInTime());
}

void MuonPhysicalChannel::applyTimeAdjustment(){

  // extract time adjustment deltaT      			
  
  double deltaT=response->extractTimeAdjustmentImprecision();
  std::vector<MuonHitTraceBack>::iterator iter;
		for(iter=m_Hits.begin();iter<m_Hits.end();iter++){
      double originalTime=(*iter).hitArrivalTime();
      // modify the time of the hit    			
      (*iter).setHitArrivalTime(originalTime+deltaT);	
      
      // set the bit flag if needed
       
if((*iter).getMCMuonHitOrigin().getBX()==LHCb::MuonBXFlag::CURRENT){
         if((*iter).hitArrivalTime()>0&&(*iter).hitArrivalTime()<m_timeGate){
				   if((*iter).getMCMuonHistory().hasTimeJittered()){
					   (*iter).getMCMuonHistory().
               setHitInByTimeAdjustment((unsigned int)1);
					 }
           //alessia correggere aggiungendo una parola che rappresenta 
           //il tempo -toflight vero pre time adjustmanet per hit ed usare 
           //quella					 
           /*					 if(!(*iter).getMCMuonHitOrigin().getXTalkNature()){
                       if((*iter).getMCMuonHit()->timeOfFlight()>25.0){
                       (*iter).getMCMuonHistory().
                       //setHitInByTimeAdjustment((unsigned int)1);
                       }		
                       }*/  
				 }
			   if(originalTime>0.0&&originalTime<m_timeGate){
				   if((*iter).hitArrivalTime()<0.||(*iter).hitArrivalTime()>
              m_timeGate){
					   (*iter).getMCMuonHistory().
               setHitOutByTimeAdjustment((unsigned int)1);
					 }
				 }
			 }       
		}
}

void MuonPhysicalChannel::applyDeadtime(int BX){
  // extract the start conditions
  bool dead=false;
  double lenghtOfDead=0; 
  double timeOfStart=0;  
  //extract wether the channel is in deadtime at the very beginning	 
  if(dead){
    //	   lenghtOfDead=; 
    //	   timeOfStart=; //relative to range 0-lenghtOfDead
  }
  // corrected for the BX(negative)
  timeOfStart=-timeOfStart-BX*(m_timeBX);
  timeOfStart=-500;  

  //	 if(m_ID.getStation()==1&&m_ID.getRegion()==2)cout<<"bx "<<BX
  //<<" "<<m_timeBX<<endl;
  std::vector<MuonHitTraceBack>::iterator iterOnHits;
  double timeOfStartOfDeadtime=timeOfStart;;
  double lenghtOfDeadtime=lenghtOfDead;;
  
  for(iterOnHits=m_Hits.begin();iterOnHits<m_Hits.end();iterOnHits++){
    // check that he hit is able to fire the channel 
    if(!(*iterOnHits).getMCMuonHistory().isHitOutGeoAccemtance()&&
       !(*iterOnHits).getMCMuonHistory().isKilledByChamberInefficiency()){	 
      double timeOfHit=(*iterOnHits).hitArrivalTime();
      //				if(m_ID.getStation()==1&&m_ID.getRegion()==2)
      //					cout<<" deadtime time "<<*(phChID())<< " "
      //<<timeOfHit<<" "<<timeOfStartOfDeadtime<<" "<<lenghtOfDeadtime<<endl;
      if(timeOfHit<timeOfStartOfDeadtime+lenghtOfDeadtime){
        //        hit in deadtime
        (*iterOnHits).getMCMuonHistory().setHitInDeadtime(1);
      }else{
        //		     hit start the deadtime
        timeOfStartOfDeadtime=timeOfHit;
        response->extractDeadtime(lenghtOfDeadtime);
        //         extract the value of lenghtOfDeadtime	
      }	 
    }		 
  }	  	
}
#endif
