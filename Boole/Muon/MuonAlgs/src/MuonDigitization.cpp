#ifndef MUONALGS_MUONDIGITIZATION_CPP
#define MUONALGS_MUONDIGITIZATION_CPP 1

#include "GaudiKernel/AlgFactory.h"  
#include "GaudiKernel/IToolSvc.h"  
#include "GaudiKernel/IAlgManager.h"
#include "MuonAlgs/MuonDigitization.h"
#include "GaudiKernel/LinkManager.h" 
#include "MuonAlgs/ComparePC.h"
#include "GaudiKernel/IChronoStatSvc.h"

#include <iostream>


static const AlgFactory<MuonDigitization>  Factory;
const IAlgFactory& MuonDigitizationFactory = Factory;


const std::string MuonDigitization::spill[5] = 
           {"","/Prev","/PrevPrev","/Next","/NextNext"};
const std::string MuonDigitization::numreg[4] = 
           {"1","2","3","4"};
const std::string MuonDigitization::numsta[5]= 
           {"1","2","3","4","5"};

const std::string MuonDigitization::TESPathOfHitsContainer[4]=
           {"Hits","ChamberNoiseHits","FlatSpilloverHits","BackgroundHits"};
const int MuonDigitization::MuonDigitization::OriginOfHitsContainer[4]=
           {MuonOriginFlag::GEANT,MuonOriginFlag::CHAMBERNOISE,
					  MuonOriginFlag::FLATSPILLOVER,MuonOriginFlag::BACKGROUND};

MuonDigitization::MuonDigitization(const std::string& name,
                                   ISvcLocator* pSvcLocator)
:  Algorithm(name, pSvcLocator)
{
  //declareProperty("NmbOfSpilloverEvents" , m_numberOfSpilloverEvents=3) ;
declareProperty("BXTime" , m_BXTime=25) ;
declareProperty("VerboseDebug" , m_verboseDebug=false) ;

}



StatusCode MuonDigitization::initialize()
{
  MsgStream log(msgSvc(), name()); 

  // Get the number of spillover events from the SpillOverAlg
  IAlgManager* algmgr;
  StatusCode sc = service( "ApplicationMgr", algmgr );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Failed to locate algManager i/f of AppMgr" << endmsg;
    return sc;
  }
  IAlgorithm*  spillAlg;
  sc = algmgr->getAlgorithm( "SpillOverAlg", spillAlg );
  if( !sc.isSuccess() ) {
    log << MSG::WARNING << "SpillOverAlg not found" << endmsg;
    m_numberOfSpilloverEvents = 0;
  }
  else {
    IProperty* spillProp;
    spillAlg->queryInterface( IID_IProperty, (void**)&spillProp );
    IntegerProperty numPrev = 0;
    IntegerProperty numNext = 0;
    numPrev.assign( spillProp->getProperty("SpillOverPrev") );
    numNext.assign( spillProp->getProperty("SpillOverNext") );
    m_numberOfSpilloverEvents = numPrev + numNext;
  }
  log << MSG::INFO << "number of spillover events read from aux stream "
      << m_numberOfSpilloverEvents << endmsg;
  m_numberOfEvents=m_numberOfSpilloverEvents+1;

 sc = toolSvc()->retrieveTool( "MuonTileIDXYZ", m_pMuonTileXYZ );
 if( !sc.isSuccess() ) {
   log << MSG::ERROR << "Failed to retrieve MuonTileIDXYZ tool" << endmsg;
   return sc;
 }
 MuonDigitizationParameters::Parameters usefull( toolSvc(), detSvc(), msgSvc());
 log<<MSG::DEBUG<<usefull.getChamberPerRegion(0);
 m_flatDist.initialize( randSvc(), Rndm::Flat(0.0,1.0));	 
 detectorResponse.initialize( randSvc(), detSvc(), msgSvc());
 
 return StatusCode::SUCCESS;
}
 
 					
StatusCode MuonDigitization::execute()
{
//	IChronoStatSvc* svc = chronoSvc();	
 
//  IChronoStatSvc::ChronoTime time;	 
  MsgStream log(msgSvc(), name());
 
 	StatusCode sc=StatusCode::SUCCESS ;

  log<<MSG::DEBUG<<"starting the Muon Digitization algorithm "<<endreq;
  SmartDataPtr<MCMuonHits> hitPointer(eventSvc(),"/Event/MC/Muon/M2/R1/Hits");
 	 

  if(hitPointer!=0){
     MCMuonHits::const_iterator i;
     for (i=(hitPointer)->begin();i<(hitPointer)->end();i++){                          
        if(m_verboseDebug){
		      log<<MSG::DEBUG<<"muon x , y, z , exit  "<< (*i)->exit().x() <<" " <<
 			      (*i)->exit().y() << "  " <<                          
				    (*i)->exit().z() << endreq ;																		
          log<<MSG::DEBUG<<"muon x , y, z entry ,  "<< (*i)->entry().x() <<" " <<
            (*i)->entry().y() << "  " <<(*i)->entry().z() << endreq ;   
          log<<MSG::DEBUG<<"time of flight ,  "<< (*i)->timeOfFlight()  << endreq ;   
		      log<<MSG::DEBUG<<" chamber and gap ID	"<<
            (*i)->chamberID()<<" "<<(*i)->gapID()<<endreq;
	        MCParticle* origparticle=(*i)->mcParticle();
          if(origparticle)  {
		        log<<MSG::DEBUG<<"Particle from which it originates "<<origparticle->helicity()<<endreq;
 		      }
				  else{
				     log<<MSG::WARNING<<"Particle from which it originates is not defined "<< endreq;
				  }
				}	
     }
	}	
	else{
    log<<MSG::ERROR<<"unable to retrieve the hit container"<<endreq; 
  } 
    

	
	sc=addChamberNoise();
	if(sc.isFailure())return StatusCode::FAILURE;
	MuonDigitizationData<MuonPhyChannelInput> PhyChaInput("MuPI",&log,eventSvc(),"MuPI") ;
	sc=createInput(PhyChaInput);
	if(sc.isFailure())return StatusCode::FAILURE;	
 	MuonDigitizationData<MuonPhysicalChannel> PhyChaOutput("MuPO",&log,eventSvc(),"MuPO") ;
  sc=elaborateMuonPhyChannelInputs(PhyChaInput,PhyChaOutput);	
	if(sc.isFailure())return StatusCode::FAILURE;	 	
 	sc=addElectronicNoise(PhyChaOutput);
	if(sc.isFailure())return StatusCode::FAILURE;	
  sc=applyPhysicalEffects(PhyChaOutput);
	if(sc.isFailure())return StatusCode::FAILURE;	
 	 		
	MuonDigitizationData<MuonPhysicalChannelOutput> PhysicalChannelOutput("MULC",&log,eventSvc(),"MULC") ;
	sc=fillPhysicalChannel(PhyChaOutput,PhysicalChannelOutput);
	if(sc.isFailure())return StatusCode::FAILURE;	 			
  if(m_verboseDebug){	
    for(int i=0; i<20; i++){
	    log<<MSG::DEBUG<<"  last print container number "<<i<<endreq;
      MuonPhysicalChannelOutputs::const_iterator iter=PhysicalChannelOutput.getPartition(i)->begin();
		  for(iter=PhysicalChannelOutput.getPartition(i)->begin();iter<PhysicalChannelOutput.getPartition(i)->end();iter++){		    
		     log<<MSG::DEBUG<<"FE ID "<<(*iter)->phChID()->getID()<<endreq;
			   log<<MSG::DEBUG<<"Station "<<(*iter)->phChID()->getStation()<<endreq;
			   log<<MSG::DEBUG<<"Region "<<(*iter)->phChID()->getRegion()<<endreq;
			   log<<MSG::DEBUG<<"Quadrant "<<(*iter)->phChID()->getQuadrant()<<endreq;
 			   log<<MSG::DEBUG<<"Chamber "<<(*iter)->phChID()->getChamber()<<endreq;
			   log<<MSG::DEBUG<<" ch X "<<(*iter)->phChID()->getPhChIDX()<<endreq;
			   log<<MSG::DEBUG<<" ch Y "<<(*iter)->phChID()->getPhChIDY()<<endreq;
			   log<<MSG::DEBUG<<" frontend "<<(*iter)->phChID()->getFrontEnd()<<endreq;
 			   std::vector<MuonHitTraceBack> vector_traceBack=(*iter)->hitsTraceBack();
			   std::vector<MuonHitTraceBack>::iterator iterTraceBack=(vector_traceBack).begin(); 
			   for(iterTraceBack=(vector_traceBack).begin();iterTraceBack<(vector_traceBack).end();iterTraceBack++){
            log<<MSG::DEBUG<<"hit time "<<(*iterTraceBack).hitArrivalTime() <<endreq; 
 			   }
		  }
    }
	}

 
  MCMuonDigits* mcDigitContainer= new MCMuonDigits;
  sc=createLogicalChannel(PhysicalChannelOutput, *mcDigitContainer);
	if(sc.isFailure())return StatusCode::FAILURE;	 			
	eventSvc()->registerObject(MCMuonDigitLocation::MCMuonDigit,mcDigitContainer);
  MuonDigits* digitContainer= new MuonDigits;	
	sc=createRAWFormat(*mcDigitContainer, *digitContainer);
	if(sc.isFailure())return StatusCode::FAILURE;	 				
	eventSvc()->registerObject(MuonDigitLocation::MuonDigit,digitContainer);

 	log<<MSG::DEBUG<<"End of the Muon Digitization"<<endreq;
  return StatusCode::SUCCESS;

}

StatusCode MuonDigitization::finalize()
{

  return StatusCode::SUCCESS;
}

StatusCode
MuonDigitization::addChamberNoise(){
  MuonDigitizationParameters::Parameters usefull;
	MsgStream log(msgSvc(), name()); 
  int container=1;
	for (int ispill=0;ispill<m_numberOfEvents;ispill++){
	  int chamberTillNow=0;
		double shiftOfTOF=-m_BXTime*ispill;
    for(int k=0;k<5;k++){ 
	    for(int s=0;s<4;s++){
			   KeyedContainer<MCMuonHit>* hitsContainer = new KeyedContainer<MCMuonHit>();
 			   std::string path="/Event"+spill[ispill]+"/MC/Muon/M"+numsta[k]+
			       "/R"+numreg[s]+"/"+TESPathOfHitsContainer[container];
		     int partitionNumber=k*4+s;
//			   int readoutNumber=usefull.getReadoutNumber(partitionNumber);	
			   int chamberInRegion=usefull.getChamberPerRegion(partitionNumber);
				 int gapPerChamber=usefull.getGapPerRegion(partitionNumber);
			   for (int chamber=0;chamber<chamberInRegion;chamber++){			   
			     int numberOfNoiseHit=(detectorResponse.getChamberResponse(partitionNumber))->extractNoise();
// the readout number is essentially meaningless... it is chamber noise.....					 
				   int readout=0;
					 double startPosZ=usefull.getStartPositionFirstGapZ(chamber+ chamberTillNow);
				   for (int hit=1;hit<=numberOfNoiseHit;hit++){
//define position of hit 
//first gap number
              int gap=(int)((m_flatDist()*gapPerChamber)); 
						  if(gap>3)gap=3;
							int frontEnd=(int)(gap/usefull.getGapPerFE(partitionNumber));
							int gapFE=usefull.getGapPerFE(partitionNumber);
							double middlePosZ=startPosZ+(usefull.getStopGapZ((frontEnd+1)*gapFE-1 ,partitionNumber)-
								usefull.getStartGapZ(frontEnd*gapFE ,partitionNumber))/2+usefull.getStartGapZ(frontEnd*gapFE ,partitionNumber);
//then x&y	
              float x= m_flatDist()* usefull.getPhChannelSizeX(readout,partitionNumber)*usefull.getPhChannelNX(readout,partitionNumber)+
						          usefull. getStartChamberPositionX(chamber+chamberTillNow);
						  float y= m_flatDist()* usefull.getPhChannelSizeY(readout,partitionNumber)*usefull.getPhChannelNY(readout,partitionNumber)+
						          usefull. getStartChamberPositionY(chamber+chamberTillNow);
						  float time=		m_flatDist()*m_BXTime;	
						  MCMuonHit* pHit = new MCMuonHit();
						  pHit->setEntry(HepPoint3D(x,y,usefull.getStartPositionFirstGapZ(chamber+chamberTillNow)+usefull.getStartGapZ(gap,partitionNumber)));
						  pHit->setExit(HepPoint3D(x,y,usefull.getStartPositionFirstGapZ(chamber+chamberTillNow)+usefull.getStopGapZ(gap,partitionNumber)));
 							double tofOfLight=(sqrt(x*x+ y*y+(middlePosZ)*(middlePosZ)))/333.3;
    			    pHit->setTimeOfFlight(time+shiftOfTOF+tofOfLight); 
							pHit->setVolumeID(chamber+1,gap+1);
							hitsContainer->insert(pHit);
							if(m_verboseDebug){	
							  log<<MSG::DEBUG<<"adding chamber noise hit "<<ispill<<" "<<k<<" "<<s<<endreq;
							}	
				   }
  			 }
			  chamberTillNow=chamberTillNow+chamberInRegion;
				eventSvc()->registerObject(path,hitsContainer);
			 
	    }
	  }
	}	 
  return StatusCode::SUCCESS;
}



StatusCode
MuonDigitization::createInput(
       MuonDigitizationData<MuonPhyChannelInput>& PhyChaInput)
{
  MsgStream log(msgSvc(), name()); 
  MuonDigitizationParameters::Parameters usefull ;
  unsigned int chamberTillNow=0;		
//loop over the containers
  for(int iterRegion=0;iterRegion<20;iterRegion++){
 
 		 std::vector<MuonPhPreInput> keepTemporary ;		 
		 int station=iterRegion/4+1;
	   int region=iterRegion%4+1;	 
	   int readoutNumber=usefull.getReadoutNumber(iterRegion);		 
     for (int ispill=0; ispill<m_numberOfEvents;ispill++){
        for(int container=0; container<4;container++){				
		      std::string path="/Event"+spill[ispill]+"/MC/Muon/M"+numsta[station-1]+"/R"+numreg[region-1]+"/"+TESPathOfHitsContainer[container];
					if(m_verboseDebug) {log<<MSG::DEBUG<<"hit container path "<<path<<endreq;}
          SmartDataPtr<MCMuonHits> hitPointer(eventSvc(),path);
 	        MCMuonHits::const_iterator iter;	 
	        if(hitPointer!=0){
            for (iter=(hitPointer)->begin();iter<(hitPointer)->end();iter++){  

// evaluate slop and intercept in the transverse plane .. useful later

              bool parallelFlag=false ; 
				      float slopeY;
				      float intercept;
              if((*iter)->exit().x()!=(*iter)->entry().x()){
                slopeY=((*iter)->exit().y()-(*iter)->entry().y())/((*iter)->exit().x()-(*iter)->entry().x());
				        intercept=	(*iter)->exit().y()	- slopeY*(*iter)->exit().x() ;
				      }else{
				        parallelFlag=true ; 
				      }
 		          for(int ireadout=0;ireadout<=readoutNumber;ireadout++){
 				        float phChSizeX= usefull.getPhChannelSizeX( ireadout, iterRegion);
				   	    float phChSizeY= usefull.getPhChannelSizeY( ireadout, iterRegion);
                unsigned int readoutType=  usefull.getReadoutType(ireadout, iterRegion);
// how many p.c. are crossed by the hit???

                int chamber= (int)chamberTillNow + (*iter)->chamberID()-1 ;
   					    int gap=(*iter)->gapID()-1 ;
					
    				    int frontEnd = gap/usefull.getGapPerFE(iterRegion);
								int gapFE=usefull.getGapPerFE(iterRegion);
					
   					    double startPosX=usefull.getStartChamberPositionX(chamber) ;
   					    double startPosY= usefull.getStartChamberPositionY(chamber) 		;			
								double middlePosZ=usefull.getStartPositionFirstGapZ(chamber)+(usefull.getStopGapZ((frontEnd+1)*gapFE-1 ,iterRegion)-
								usefull.getStartGapZ(frontEnd*gapFE ,iterRegion))/2+usefull.getStartGapZ(frontEnd*gapFE ,iterRegion);
   					    unsigned int nxChaEntry ;
   					    unsigned int nxChaExit  ;
   					    int tmpNxChaEntry=(int) (((*iter)->entry().x()-startPosX)/phChSizeX);
						    int tmpNxChaExit=(int) (((*iter)->exit().x()-startPosX)/phChSizeX);
								double xstart,xstop;
								double ystart,ystop;
                if(tmpNxChaEntry<=tmpNxChaExit){
						      if( tmpNxChaEntry<0)tmpNxChaEntry=0;
						      if( tmpNxChaExit>=(int)usefull.getPhChannelNX(ireadout, iterRegion))
									     tmpNxChaExit=(int)usefull.getPhChannelNX(ireadout, iterRegion)-1 ;		
									xstart=	(*iter)->entry().x()			;
									xstop=	(*iter)->exit().x()			;			
						      nxChaEntry = (unsigned int) tmpNxChaEntry;
						      nxChaExit  = (unsigned int) tmpNxChaExit;										
											 								 		  					  
								}
// normal order								
    						else{
// inverse order					
						      if( tmpNxChaExit<0)tmpNxChaExit=0;
						      if( tmpNxChaEntry>=(int)usefull.getPhChannelNX(ireadout, iterRegion))
									     tmpNxChaEntry=(int)usefull.getPhChannelNX(ireadout, iterRegion)-1 ;		
									xstop=	(*iter)->entry().x()			;
									xstart=	(*iter)->exit().x()			;											 		  					  											 						  					  
						      nxChaEntry = (unsigned int) tmpNxChaExit;
						      nxChaExit  = (unsigned int) tmpNxChaEntry;										
                }   
  					    unsigned int quarter=usefull.getQuadrantChamber(chamber);					
						    float xBegin;
						    float xEnd;
						    double distanceFromBoundary[4];
						    unsigned int nyBegin ;
						    unsigned int nyEnd ;					
						    for (unsigned int Xloop=nxChaEntry;Xloop<=nxChaExit;Xloop++){
					  	    if(Xloop==nxChaEntry){
						 	     xBegin= xstart;
							    }else{
						  	    xBegin=Xloop*phChSizeX+startPosX ;
							    }
					  	    if(Xloop==nxChaExit){						 
							       xEnd= xstop;
						      }else{
						        xEnd=(Xloop+1)*phChSizeX+startPosX ;
						      }	
						      float yBegin;
						      float yEnd;
						      if(parallelFlag){
						        yBegin=(*iter)->entry().y();
						        yEnd= (*iter)->exit().y();
						      }
						      else{					
						        yBegin=intercept+slopeY*xBegin ;
						        yEnd=intercept+slopeY*xEnd ;
						      }
						      double xinit,yinit,xend,yend;
						      int tmpYBegin=static_cast<int> ((yBegin-startPosY)/(phChSizeY));
						      int tmpYEnd=static_cast<int> ((yEnd-startPosY)/(phChSizeY))	;	
									if(tmpYBegin<=tmpYEnd){
					          if( tmpYBegin<0)tmpYBegin=0;
					          if( tmpYEnd>=(int)usefull.getPhChannelNY(ireadout, iterRegion))
										tmpYEnd=(int)usefull.getPhChannelNY(ireadout, iterRegion)-1 ;	
										ystart=yBegin;
										ystop=yEnd;			
										nyBegin=tmpYBegin ;		
										nyEnd=  tmpYEnd	;			
										xinit=xBegin;
										yinit=ystart;
										xend=xEnd;
										yend=ystop;
											  
									}else{
					          if( tmpYEnd<0)tmpYEnd=0;
					          if( tmpYBegin>=(int)usefull.getPhChannelNY(ireadout, iterRegion))
										tmpYBegin=(int)usefull.getPhChannelNY(ireadout, iterRegion)-1 ;
										ystart=yEnd;
										ystop=yBegin;	
										nyBegin=tmpYEnd ;		
										nyEnd=  tmpYBegin	;				  																	  									
										xinit=xEnd;
										yinit=ystart;
										xend=xBegin;
										yend=ystop;
                  }
 						      double xi,xf,yi,yf;
 						    
 						      for (unsigned int Yloop=nyBegin;Yloop<=nyEnd;Yloop++){
									  if(nyBegin==nyEnd){
										   xi=xinit;
											 yi=yinit;
											 xf=xend;
											 yf=yend;
 										}else if(Yloop==nyBegin&&Yloop!=nyEnd){
										   xi=xinit;
											 yi=yinit;
											 xf=(((Yloop+1)*phChSizeY+startPosY-intercept)/slopeY );
											 yf=((Yloop+1)*phChSizeY+startPosY) ;										   
										}else if(Yloop!=nyBegin&&Yloop==nyEnd){
										   xi=(((Yloop)*phChSizeY+startPosY-intercept)/slopeY );
											 yi=((Yloop)*phChSizeY+startPosY) ;
											 xf=xend;
											 yf=yend;										   
										}else{
										   xi=(((Yloop)*phChSizeY+startPosY-intercept)/slopeY );
											 yi=((Yloop)*phChSizeY+startPosY) ;
											 xf=(((Yloop+1)*phChSizeY+startPosY-intercept)/slopeY );
											 yf=((Yloop+1)*phChSizeY+startPosY) ;										  
										}
										if((xi-((Xloop)*phChSizeX+startPosX))<(xf-((Xloop)*phChSizeX+startPosX))){
										  distanceFromBoundary[0]=xi-((Xloop)*phChSizeX+startPosX);}
										else{
										  distanceFromBoundary[0]=xf-((Xloop)*phChSizeX+startPosX);}
										if((yi-((Yloop)*phChSizeY+startPosY))<(yf-((Yloop)*phChSizeY+startPosY))){	
										  distanceFromBoundary[1]=yi-((Yloop)*phChSizeY+startPosY);}			
										else{
										  distanceFromBoundary[1]=yf-((Yloop)*phChSizeY+startPosY);}
										if((((Xloop+1)*phChSizeX+startPosX)-xi)<(((Xloop+1)*phChSizeX+startPosX)-xf)){	
										  distanceFromBoundary[2]=((Xloop+1)*phChSizeX+startPosX)-xi;}
										else{
										  distanceFromBoundary[2]=	((Xloop+1)*phChSizeX+startPosX)-xf;}	
										if((((Yloop+1)*phChSizeY+startPosY)-yi)<(((Yloop+1)*phChSizeY+startPosY)-yf)){						
 										  distanceFromBoundary[3]= ((Yloop+1)*phChSizeY+startPosY)-yi;}
										else{	
										  distanceFromBoundary[3]= ((Yloop+1)*phChSizeY+startPosY)-yf;}
										
 						        MuonPhPreInput* inputPointer = new  MuonPhPreInput  ;				
				            inputPointer->phChID()->setStation((unsigned int)(station-1));
				            inputPointer->phChID()->setRegion((unsigned int)(region-1));					    			
							      inputPointer->phChID()->setQuadrant(quarter);
							      inputPointer->phChID()->setChamber((*iter)->chamberID()-1 );
							      inputPointer->phChID()->setPhChIDX(Xloop);
							      inputPointer->phChID()->setPhChIDY(Yloop);													
							      inputPointer->phChID()->setFrontEnd((unsigned int) frontEnd);							
							      inputPointer->phChID()->setReadout(readoutType);
							      inputPointer->phChID()->setGap(gap);
							      inputPointer->getHitTraceBack()->setMCMuonHit(*iter); 

/// correct for the  tof .... i.e. subtract the tof that a lightparticle impacting the center of the pc. has....
                    double tofOfLight=(sqrt(((Xloop+0.5)*phChSizeX+startPosX)*((Xloop+0.5)*phChSizeX+startPosX)+
										      ((Yloop+0.5)*phChSizeY+startPosY)*((Yloop+0.5)*phChSizeY+startPosY)+(middlePosZ)*(middlePosZ)))/333.3;
																				
							      inputPointer->getHitTraceBack()->setHitArrivalTime((*iter)->timeOfFlight()-tofOfLight);
							      inputPointer->getHitTraceBack()->setCordinate(distanceFromBoundary);
		                inputPointer->getHitTraceBack()->getMCMuonHitOrigin().setBX(ispill);
										inputPointer->getHitTraceBack()->getMCMuonHitOrigin().setHitNature(OriginOfHitsContainer[container]);										
    							  inputPointer->getHitTraceBack()->getMCMuonHistory().setBXOfHit(ispill);									
									  inputPointer->getHitTraceBack()->setCordinate(distanceFromBoundary);
                    if(m_verboseDebug){	
                      log<<MSG::DEBUG<<"hit processing "<<station<<" "<< region<<" " << quarter<<" "<< chamber
							           <<" "<<gap<<" "<<frontEnd<<" "<<ireadout<<" "<<
							           " "<<Xloop<<" "<<Yloop<<" "<<
												 usefull.getPhChannelNX(ireadout, iterRegion)<<usefull.getPhChannelNY(ireadout, iterRegion)  
												 <<" "<<tofOfLight<<" "<< OriginOfHitsContainer[container]<<" "<<
												 ispill<< endreq;
										}	 
                    keepTemporary.push_back(*inputPointer);
							      delete inputPointer;
  					      }
								}		
					   }
           }				              
         }
	     }	
 		
 	  }
		if(m_verboseDebug){
		log<<MSG::DEBUG<<"pre-sorted vector"<<endreq;}
//		std::vector<MuonPhPreInput>::reverse_iterator iterPre;		
    std::stable_sort(keepTemporary.begin(),keepTemporary.end(),SortPhChID());
		if(m_verboseDebug){
		log<<MSG::DEBUG<<"sorted vector"<<endreq;}
		std::vector<MuonPhPreInput>::reverse_iterator iterPost;	
 
		for(iterPost=keepTemporary.rbegin();iterPost<keepTemporary.rend();++iterPost){
		    if(m_verboseDebug){
           log<<MSG::DEBUG<<"ID  Post "<< (iterPost)->phChID()->getID()<<
				    " hit origin "<< (iterPost)->getHitTraceBack()->getMCMuonHitOrigin().getFlatSpilloverNature() 
						 <<endreq;		}
   		  MuonPhyChannelInput* phChPointer = 
				new MuonPhyChannelInput((iterPost)->phChID()->getID(),*((iterPost)->getHitTraceBack())) ;	
  		  PhyChaInput.addMuonObject(iterRegion,phChPointer);		 	 
				keepTemporary.pop_back(); 	
 		 }

 	  
//!!!!!!!!!!!!!  left it as the last instruction of the for loop         !!!!		
	   chamberTillNow=chamberTillNow+usefull.getChamberPerRegion(iterRegion);	   	  
		 
	}   
  return StatusCode::SUCCESS;
}



 
 
  StatusCode MuonDigitization::elaborateMuonPhyChannelInputs(MuonDigitizationData<MuonPhyChannelInput> & PhyChaInput,
	           MuonDigitizationData<MuonPhysicalChannel>& PhysicalChannel){
	MsgStream log(msgSvc(), name()); 					 
  for (int i=0; i<20;i++){
	  if(!PhyChaInput.isEmpty(i)){	  
		  MuonPhyChannelInputs::const_iterator inputIter ;
		  MuonPhyChannelInputs::const_iterator inputIterStart=PhyChaInput.getPartition(i)->begin();
/* 		  int station=i/4+1;
	    int region=i%4+1;	 
 		  std::string text="/Event/MC/Muon/M"+numsta[station-1]+"/R"+numreg[region-1]+"/Hits";
		  DataObject* pPart;
      StatusCode sc=eventSvc()->retrieveObject(text, (DataObject*&)pPart);		*/
 		  unsigned int prevFE=(*inputIterStart)->phChID()->getFETile();
		  unsigned int lastFE;
//			unsigned int prevID=(*inputIterStart)->phChID()->getID();
//			unsigned int lastID;
		  MuonHitTraceBack* pointerToHitTraceBack=(*inputIterStart)->getHitTraceBack();
		  MuonPhysicalChannel* outputPointer = new MuonPhysicalChannel(prevFE);
			 
			outputPointer->setResponse(detectorResponse.getResponse(*((*inputIterStart)->phChID())));
			if(!detectorResponse.getResponse(*((*inputIterStart)->phChID()))){
			   log<<MSG::ERROR<<"unable to retrieve the response of the physical channel"<<endreq;
			}
		  outputPointer->hitsTraceBack().push_back(*pointerToHitTraceBack);
			PhysicalChannel.addMuonObject(i,outputPointer);
 		  for (inputIter=++inputIterStart;inputIter<PhyChaInput.getPartition(i)->end();inputIter++){
		    lastFE=(*inputIter)->phChID()->getFETile();
 		    pointerToHitTraceBack=(*inputIter)->getHitTraceBack();
 				if(lastFE==prevFE){
           outputPointer->hitsTraceBack().push_back(*pointerToHitTraceBack) ;
 				}else{
				  outputPointer = new MuonPhysicalChannel(lastFE);
					outputPointer->setResponse(detectorResponse.getResponse(*((*inputIterStart)->phChID())));
 					outputPointer->hitsTraceBack().push_back(*pointerToHitTraceBack) ;
					prevFE=lastFE;
					PhysicalChannel.addMuonObject(i,outputPointer);
				}			
		 }						
	 }
	}
  return StatusCode::SUCCESS;	
} 



  StatusCode  MuonDigitization::fillPhysicalChannel(MuonDigitizationData<MuonPhysicalChannel>& PhysicalChannel,
	            MuonDigitizationData<MuonPhysicalChannelOutput>& PhysicalChannelOutput){
		  MuonPhysicalChannels::iterator iterInput;
//		  MuonPhysicalChannelOutputs::iterator iterOutput;
   
      for (int i=0; i<20;i++){
	      if(!PhysicalChannel.isEmpty(i)){				
			    for(iterInput=PhysicalChannel.getPartition(i)->begin();iterInput<PhysicalChannel.getPartition(i)->end();iterInput++){
			      MuonPhysicalChannelOutput* objToAdd=new MuonPhysicalChannelOutput(*(*iterInput));
						bool fired=false;
						bool interestingHit=false;
						double timeOfFiring;
						std::vector<MuonHitTraceBack>::iterator iterInHits ;
						for(iterInHits=objToAdd->hitsTraceBack().begin();iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){
						   if(!fired){
                 if(!(*iterInHits).getMCMuonHistory().isHitOutGeoAccemtance()&&
							      !(*iterInHits).getMCMuonHistory().isKilledByChamberInefficiency()&&
									  !(*iterInHits).getMCMuonHistory().isHitInDeadtime()){
									   timeOfFiring=(*iterInHits).hitArrivalTime();
										 if(timeOfFiring>0&&timeOfFiring<m_BXTime){
										   fired=true;
											 (*iterInHits).getMCMuonHistory().setFiredFlag((unsigned int) 1);
											 objToAdd->setFiringTime(timeOfFiring);		
											 objToAdd->phChInfo().setAliveDigit(1);
											 objToAdd->phChInfo().setBXIDFlagHit((*iterInHits).getMCMuonHistory().BX());
											 objToAdd->phChInfo().setNatureHit((*iterInHits).getMCMuonHitOrigin().getNature());
											 if((*iterInHits).getMCMuonHistory().isHitOriginatedInCurrentEvent()){
											   if((*iterInHits).getMCMuonHistory().isInForTimeAdjustment()){
												   objToAdd->phChInfo().setAliveTimeAdjDigit(1);
													 if((*iterInHits).getMCMuonHistory().hasTimeJittered()){
													   objToAdd->phChInfo().setTimeJitteredDigit(1);
													 }
												 }
											 }
										 }
								 }
							 }
 						}
 						if(!fired){
						  for(iterInHits=objToAdd->hitsTraceBack().begin();iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){
							  if((*iterInHits).getMCMuonHistory().isHitOriginatedInCurrentEvent()){
								  interestingHit=true;
								  if(!(*iterInHits).getMCMuonHitOrigin().getXTalkNature()&&!(*iterInHits).getMCMuonHitOrigin().getElectronicNoiseNature()){
									     if((*iterInHits).getMCMuonHit()->timeOfFlight()>0&&(*iterInHits).getMCMuonHit()->timeOfFlight()<m_BXTime){
									
                       if((*iterInHits).getMCMuonHistory().hasTimeJittered()){
										     if(!((*iterInHits).getMCMuonHistory().isOutForTimeAdjustment())){
// first source of dead time jitter                  
                           objToAdd->phChInfo().setTimeJitteredDigit(1);  									
											  }else if ((*iterInHits).getMCMuonHistory().isHitInDeadtime()){
// remember to check that the time adjustment do not put back the hit int he gate....=> only deadtime can kill this digit 											 
											   objToAdd->phChInfo().setDeadtimeDigit(1);
											  }	 
										  }else  if((*iterInHits).getMCMuonHistory().isKilledByChamberInefficiency()){
// hit is killed by chamber inefficiency
										    objToAdd->phChInfo().setChamberInefficiencyDigit(1);
										  }else   if((*iterInHits).getMCMuonHistory().isHitOutGeoAccemtance()){
// hit is killed by geomatrial acceptance 
										    objToAdd->phChInfo().setGeometryInefficiency(1);
										  }else   if((*iterInHits).getMCMuonHistory().isOutForTimeAdjustment()){
// hit is killed by time adjustment
										    objToAdd->phChInfo().setTimeAdjDigit(1);												
										  }else if ((*iterInHits).getMCMuonHistory().isHitInDeadtime()){
// hit in deadtime
                       objToAdd->phChInfo().setDeadtimeDigit(1);
											} 
                    }  										
                  }
  							}
						  }
						}
 				    if(fired||interestingHit){
						  PhysicalChannelOutput.addMuonObject(i,objToAdd);
						}
						else {
						  delete 	objToAdd;
						}
 		 	    }
			  }
			}	
  return StatusCode::SUCCESS;					   
	}


StatusCode MuonDigitization::applyPhysicalEffects(MuonDigitizationData<MuonPhysicalChannel>&
	PhysicalChannel){
	MsgStream log(msgSvc(), name()); 						 
  MuonDigitizationParameters::Parameters usefull ;

//loop over the 20 containers 

  for (int i=0; i<20;i++){	
	  if(!PhysicalChannel.isEmpty(i)){	  
		  std::vector<MuonPhysicalChannel*>  XTalkPhysicalChannel;
			std::vector<MuonPhysicalChannel*>::iterator iterOnSTD;
		  MuonPhysicalChannels::iterator iter ;
			std::vector<MuonPhysicalChannel*> channelsDueToXTalk;	   
			std::vector<MuonPhysicalChannel*>::iterator iterXTalk;
			MuonPhysicalChannel* pFound;
			int phChInX[2] ;
			int phChInY[2] ;
      for (int iloop=0; iloop<=usefull.getReadoutNumber(i);iloop++){
			 	phChInX[(int)usefull.getReadoutType(iloop,i)]	=usefull.getPhChannelNX( iloop, i);
			 	phChInY[(int)usefull.getReadoutType(iloop,i)]	=usefull.getPhChannelNY( iloop, i);
			 }
      for(iter=PhysicalChannel.getPartition(i)->begin();
			    iter<PhysicalChannel.getPartition(i)->end();iter++){
 // apply per pc the time jitter on each hit	
          if(m_verboseDebug){				
 					log<<MSG::DEBUG<<"applying physics effects"<<*iter<<" "<<*iter+1<<endreq;
  				log<<MSG::DEBUG<<"applying physics effects"<<(*iter)->phChID()->getFETile()<<endreq;}
					(*iter)->applyTimeJitter();
 // apply per pc the geometry inefficiency 	on each hit			
					(*iter)->applyGeoInefficiency();					 
					
// apply per pc the chamber inefficiency 	on each hit				
					(*iter)->applyChamberInefficiency();	
 					
// apply per pc the X Talk 	on each hit				
 	        (*iter)->applyXTalk(phChInX, phChInY,channelsDueToXTalk);
 				  for(iterXTalk=channelsDueToXTalk.begin();iterXTalk<channelsDueToXTalk.end();iterXTalk++){
					   pFound=0;
					   bool already=PhysicalChannel.isObjectIn(i,*iterXTalk,pFound, ComparePC);
 						 if(!already){
						   (*iterXTalk)->setResponse(detectorResponse.getResponse(*((*iterXTalk)->phChID())));
 							 XTalkPhysicalChannel.push_back(*iterXTalk);
  						 }else{
						  log<<MSG::DEBUG<<"xtalk hit test  molto dopo "<<(*iter)<<" "<<*iterXTalk<<" "<<pFound<<endreq;
							if(pFound==(*iter))log<<MSG::ERROR<<"alessia gran casino"<<endreq;
					     pFound->addToPC(*iterXTalk);
 							 delete *iterXTalk;
						 }
					}
					if(m_verboseDebug){
					  log<<MSG::DEBUG<<" after applying physics effects"<<*iter<<" "<<*iter+1<<endreq;}
// empty the Xtalked PC container		
          while(!channelsDueToXTalk.empty()){
					 channelsDueToXTalk.pop_back();
					}
 				}
// add the xtalk hit to the main container
        
			 for(iterOnSTD=XTalkPhysicalChannel.begin();
			    iterOnSTD<XTalkPhysicalChannel.end();iterOnSTD++){
					pFound=0;
					bool already=PhysicalChannel.isObjectIn(i,*iterOnSTD,pFound, ComparePC);
					if(already){
					  pFound->addToPC(*iterOnSTD); 
						delete *iterOnSTD;
					}
				  else{
					  PhysicalChannel.addMuonObject(i,*iterOnSTD);
					}	
					
			 }	

			 for(iter=PhysicalChannel.getPartition(i)->begin();
			    iter<PhysicalChannel.getPartition(i)->end();iter++){
 // sort in time the hit of each pc
         std::vector<MuonHitTraceBack> hits=(*iter)->hitsTraceBack();
		     std::vector<MuonHitTraceBack>::iterator iterTest;
         (*iter)->sortInTimeHits();		
// printout for testing
         hits=(*iter)->hitsTraceBack(); 
				 if(m_verboseDebug){
 		       for (iterTest=hits.begin();iterTest<hits.end();iterTest++){
				   
		        log<<MSG::DEBUG<<"print hits time after ordering  "<<
					  (*iterTest).hitArrivalTime()<<endreq;}
			  	}			  
//apply time adjustment	
         (*iter)->applyTimeAdjustment();	
//apply deadtime
         (*iter)-> applyDeadtime(m_numberOfEvents) ;  		
			}	 			 			  									 			
	  }				
  }
  return StatusCode::SUCCESS ;	
}






StatusCode MuonDigitization::createLogicalChannel(MuonDigitizationData<MuonPhysicalChannelOutput>&
	PhyChaOutput, MCMuonDigits& mcDigitContainer){
	MsgStream log(msgSvc(), name()); 
//  int flag=0;
  int countDigits=0;
  for(int i=0; i<20; i++){
     MuonPhysicalChannelOutputs::const_iterator iter =PhyChaOutput.getPartition(i)->begin();
 	   MCMuonDigits::iterator iterDigit;        
        for(iter=PhyChaOutput.getPartition(i)->begin();iter<PhyChaOutput.getPartition(i)->end();iter++){	
           if(m_verboseDebug){
					   log<<MSG::DEBUG<<"FE ID "<<(*iter)->phChID()->getID()<<endreq;}
           MuonTileID phChTileID[2];
           int numberOfTileID;
           (*iter)->calculateTileID(numberOfTileID,phChTileID);
//
// loop over possible phchtileID (1 or 2 if double mapping)
//
            for(int iTile=0;iTile<numberOfTileID;iTile++){
             bool found=false; 
//
// loop over already created Digits
//           
             if(m_verboseDebug){
               log<<MSG::DEBUG<<" Loop on mappings "<<iTile<<" "<<numberOfTileID<<endreq;}

	           for(iterDigit=mcDigitContainer.begin(); iterDigit<mcDigitContainer.end()&&!found; iterDigit++){
	              MuonTileID tile=(*iterDigit)->key();
//
// tile is the key of the existing Digit, phChTileID[] is the just created ID of the ph.ch.
//                

                if(tile==phChTileID[iTile]){
                  found=true;
 // Digit already exists, update bits and links
								  std::vector<MuonHitTraceBack>::iterator iterOnHits;
                  if((*iterDigit)->DigitInfo().isAlive()&&(*iter)->phChInfo().isAlive()){
//both fired
									  if((*iterDigit)->firingTime()<(*iter)->firingTime()){
										    
										}else{
										  (*iterDigit)->setFiringTime((*iter)->firingTime());
											(*iterDigit)->DigitInfo().setNatureHit((*iter)->phChInfo().natureOfHit());
											(*iterDigit)->DigitInfo().setBXIDFlagHit((*iter)->phChInfo().BX());
										}
									}
									
                  if((*iterDigit)->DigitInfo().isAlive()&&!((*iter)->phChInfo().isAlive())){
// only one is fired	
                      								
									}
                  if(!((*iterDigit)->DigitInfo().isAlive())&&((*iter)->phChInfo().isAlive())){
// only one is fired	
										  (*iterDigit)->setFiringTime((*iter)->firingTime());
											(*iterDigit)->DigitInfo().setNatureHit((*iter)->phChInfo().natureOfHit());
											(*iterDigit)->DigitInfo().setBXIDFlagHit((*iter)->phChInfo().BX());  
											(*iterDigit)->DigitInfo().setSecondPart(0);                    																											
									}
                  if(!((*iterDigit)->DigitInfo().isAlive())&&!((*iter)->phChInfo().isAlive())){
// both not fired
                      if((*iterDigit)->DigitInfo().isInDeadTime()||	(*iter)->phChInfo().isInDeadTime()){
											  (*iterDigit)->DigitInfo().setDeadtimeDigit(1);
											}					
                      if((*iterDigit)->DigitInfo().isDeadForChamberInefficiency()||	(*iter)->phChInfo().isDeadForChamberInefficiency()){
											  (*iterDigit)->DigitInfo().setChamberInefficiencyDigit(1);
											}					
                      if((*iterDigit)->DigitInfo().isDeadByTimeJitter() ||	(*iter)->phChInfo().isDeadByTimeJitter() ){
											  (*iterDigit)->DigitInfo().setTimeJitteredDigit(1);
											}					
                      if((*iterDigit)->DigitInfo().isDeadByTimeAdjustment() ||	(*iter)->phChInfo().isDeadByTimeAdjustment() ){
											  (*iterDigit)->DigitInfo().setTimeAdjDigit(1) ;
											}					
                      if((*iterDigit)->DigitInfo().isAliveByTimeAdjustment() ||	(*iter)->phChInfo().isAliveByTimeAdjustment() ){
											  (*iterDigit)->DigitInfo().setAliveTimeAdjDigit(1);
											}					
                      if((*iterDigit)->DigitInfo().isDeadByGeometry() ||	(*iter)->phChInfo().isDeadByGeometry() ){
											  (*iterDigit)->DigitInfo().setGeometryInefficiency(1);
											}					
											
									}
// add links to the hits 										
  								for(iterOnHits=(*iter)->hitsTraceBack().begin();iterOnHits<(*iter)->hitsTraceBack().end();iterOnHits++){
                    (*iterDigit)->HitsHistory().push_back((*iterOnHits).getMCMuonHistory());
 		  							  (*iterDigit)->addToMCMuonHits((*iterOnHits).getMCMuonHit());
//			  					  }
									}
                }
             }
 	           if(!found){
                log<<MSG::DEBUG<<" create new Digit with tile "<<phChTileID[iTile]<<" "<<iTile<<endreq;
								std::vector<MuonHitTraceBack>::iterator iterOnHits;
                MCMuonDigit* newMCDigit=new MCMuonDigit(phChTileID[iTile]);
								for(iterOnHits=(*iter)->hitsTraceBack().begin();iterOnHits<(*iter)->hitsTraceBack().end();iterOnHits++){
								   newMCDigit->HitsHistory().push_back((*iterOnHits).getMCMuonHistory());
                      newMCDigit->addToMCMuonHits((*iterOnHits).getMCMuonHit());
 								}
								newMCDigit->setDigitInfo((*iter)->phChInfo());
								newMCDigit->setFiringTime((*iter)->firingTime());
                mcDigitContainer.insert(newMCDigit); 
                ++countDigits;
 	           }
	        }	
	    }
   }
  log<<MSG::DEBUG<<" MC Digits created "<<countDigits<<endreq; 
  return StatusCode::SUCCESS;
}




 
	StatusCode MuonDigitization::createRAWFormat(MCMuonDigits& mcDigitContainer, MuonDigits& digitContainer){
	MsgStream log(msgSvc(), name()); 
 	MCMuonDigits::iterator iterMCDigit; 
 	for( iterMCDigit = mcDigitContainer.begin();iterMCDigit < mcDigitContainer.end() ; iterMCDigit ++){
	  if((*iterMCDigit)->DigitInfo().isAlive()){		  
      MuonDigit* muonDigit= new MuonDigit((*iterMCDigit)->key());
			digitContainer.insert(muonDigit);
			log<<MSG::DEBUG<<"new daq word "<<(*iterMCDigit)->key()<<endreq;
 		}
	}
   return StatusCode::SUCCESS;	 
}

StatusCode MuonDigitization::
           addElectronicNoise(MuonDigitizationData
					 <MuonPhysicalChannel>& PhysicalChannel){
  MuonDigitizationParameters::Parameters usefull;
	MsgStream log(msgSvc(), name()); 
	MuonPhysicalChannel* pFound;
  for(int ispill=0;ispill<m_numberOfEvents;ispill++){
	   int chamberTillNow=0;
		 double shiftOfTOF=-m_BXTime*ispill;
	   for(int i=0;i<5;i++){
		    for(int k=0;k<4;k++){
				   int partitionNumber=i*4+k;
			     int chamberInRegion=usefull.getChamberPerRegion(partitionNumber);
           //					 int gapFE=usefull.getGapPerFE(partitionNumber);
					 for(int chamber=0;chamber<chamberInRegion;chamber++){		
	            for(int frontEnd=0;frontEnd<(int)(usefull.getGapPerRegion(partitionNumber)/
					                           usefull.getGapPerFE(partitionNumber));frontEnd++){
                //					       double startPosZ=usefull.getStartPositionFirstGapZ(chamber+ chamberTillNow);
                 //							   double middlePosZ=startPosZ+(usefull.getStopGapZ((frontEnd+1)*gapFE-1 ,partitionNumber)-
                 //								                   usefull.getStartGapZ(frontEnd*gapFE ,partitionNumber))/2
                 //																	 +usefull.getStartGapZ(frontEnd*gapFE ,partitionNumber);																		 
	               for(int readout=0;readout<=usefull.getReadoutNumber(partitionNumber);readout++){
 				            int phChInX=usefull.getPhChannelNX( readout, partitionNumber);
				            int phChInY=usefull.getPhChannelNY( readout, partitionNumber);		 
                    int noiseChannels=detectorResponse.getResponse(partitionNumber,readout)->
                             electronicNoise();
										unsigned int readoutType=  usefull.getReadoutType(readout, partitionNumber);
				            for(int hitNoise=0;hitNoise<noiseChannels;hitNoise++){
				               int chX=(int)(m_flatDist()*phChInX);
				               int chY=(int)(m_flatDist()*phChInY);
					             if(chX==phChInX)chX=phChInX-1;
					             if(chY==phChInY)chY=phChInY-1;
                       //											 float x= (chX+0.5)* usefull.getPhChannelSizeX(readout,partitionNumber) +
                       //						                    usefull. getStartChamberPositionX(chamber+chamberTillNow);
                       //                       float y= (chY+0.5)* usefull.getPhChannelSizeY(readout,partitionNumber) +
                       //						                    usefull. getStartChamberPositionY(chamber+chamberTillNow);
                       //											 double tofOfLight=(sqrt(x*x+ y*y+(middlePosZ)*(middlePosZ)))/333.3;
 					             double time=m_flatDist()*m_BXTime+shiftOfTOF ;
//											 +tofOfLight;
											 MuonPhChID ID;
											 ID.setStation(i);
											 ID.setRegion(k);
        							 ID.setQuadrant(usefull.getQuadrantChamber(chamber+chamberTillNow));
											 ID.setChamber(chamber);
											 ID.setPhChIDX(chX);
											 ID.setPhChIDY(chY);
											 ID.setFrontEnd(frontEnd);
											 ID.setReadout(readoutType);											 											 
											 MuonPhysicalChannel* newPhysicalChannel=new MuonPhysicalChannel(ID.getFETile());
											 MuonHitTraceBack* pointerToHit=new MuonHitTraceBack;
				               pointerToHit->setHitArrivalTime(time);							
                       pointerToHit->getMCMuonHitOrigin().setHitNature(MuonOriginFlag::ELECTRONICNOISE);
				               pointerToHit->getMCMuonHitOrigin().setBX(ispill);	
				               pointerToHit->getMCMuonHistory().setBXOfHit(ispill);
 				               newPhysicalChannel->hitsTraceBack().push_back(*pointerToHit);
		               		 delete pointerToHit;				
											 pFound=0;						
					             bool already=PhysicalChannel.isObjectIn(partitionNumber,newPhysicalChannel,pFound, ComparePC);
											 if(already){											   
												 pFound->addToPC(newPhysicalChannel);
 							           delete newPhysicalChannel;
											 }
											 else{
                         newPhysicalChannel->setResponse(detectorResponse.getResponse(*(newPhysicalChannel->phChID())));
												 if(!newPhysicalChannel->getResponse()){
												   log<<MSG::INFO<<"addio"<<endreq;}
 											   PhysicalChannel.addMuonObject(partitionNumber,newPhysicalChannel);	
											 }
 										}	 
								}	 
							}
					 }
					 chamberTillNow=chamberTillNow+chamberInRegion;	
				}									 							 		
		 }			 
	}		
  return StatusCode::SUCCESS;	 
		 
}					 

#endif
							
 
