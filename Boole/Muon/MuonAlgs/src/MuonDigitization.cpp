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

//reserve space for static variable
std::string MuonDigitization::spill[6] = 
{"/LHCBackground","","/Prev","/PrevPrev","/Next","/NextNext"};
std::string MuonDigitization::numreg[4] = 
{"1","2","3","4"};
std::string MuonDigitization::numsta[5]= 
{"1","2","3","4","5"};
 std::string MuonDigitization::TESPathOfHitsContainer[4]=
{"Hits","ChamberNoiseHits","FlatSpilloverHits",
"BackgroundHits"};
const int MuonDigitization::OriginOfHitsContainer[5]=
{MuonOriginFlag::GEANT,MuonOriginFlag::CHAMBERNOISE,
 MuonOriginFlag::FLATSPILLOVER,MuonOriginFlag::BACKGROUND, 
MuonOriginFlag::LHCBACKGROUND};

MuonDigitization::MuonDigitization(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  :  Algorithm(name, pSvcLocator)
{
  //declareProperty("NmbOfSpilloverEvents" , m_numberOfSpilloverEvents=3) ;
  declareProperty("BXTime" , m_BXTime=25.0) ;
  declareProperty("TimeGate" , m_gate=20.0) ;
  declareProperty("VerboseDebug" , m_verboseDebug=false) ;
  declareProperty("ApplyTimeJitter" , m_applyTimeJitter=true) ;
  declareProperty("ApplyChamberNoise" , m_applyChamberNoise=true) ;
  declareProperty("ApplyElectronicNoise" , m_applyElectronicNoise=true) ;
  declareProperty("ApplyXTalk" , m_applyXTalk=true) ;
  declareProperty("ApplyEfficiency" , m_applyEfficiency=true) ;
  declareProperty("ApplyDeadtime" , m_applyDeadtime=true) ;
  declareProperty("ApplyTimeAdjustment",m_applyTimeAdjustment=true);
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
    // Release the interfaces no longer needed
    spillAlg->release();
  }
  algmgr->release();
  log << MSG::INFO << "number of spillover events read from aux stream "
      << m_numberOfSpilloverEvents << endmsg;
  m_numberOfEvents=m_numberOfSpilloverEvents+1;
  m_numberOfEventsNeed=5;	
  // m_numberOfEvents=5;
  sc = toolSvc()->retrieveTool( "MuonTileIDXYZ", m_pMuonTileXYZ );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Failed to retrieve MuonTileIDXYZ tool" << endmsg;
    return sc;
  }
  // initialize some basic geometrical information
  MuonBasicGeometry basegeometry(detSvc(),msgSvc()); 
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();
  int i=0;  
  while(i<m_stationNumber){
    numsta[i]=basegeometry.getStationName(i);    
    log<<MSG::DEBUG<<" station "<<i<<" "<<numsta[i]<<endreq;
    i++;    
  }
  m_partition=basegeometry.getPartitions();    


  sc=toolSvc()->retrieveTool("MuonGetInfoTool",m_pGetInfo);
  if(sc.isFailure())return StatusCode::FAILURE;
  

  //  MuonGeometryStore::Parameters usefull( toolSvc(), 
  //                                                detSvc(), msgSvc());
  log<<MSG::DEBUG<<m_pGetInfo->getChamberPerRegion(0)<<endreq;
  m_flatDist.initialize( randSvc(), Rndm::Flat(0.0,1.0));	 
  detectorResponse.initialize( toolSvc(),randSvc(), detSvc(), msgSvc());
  m_spill=6;
  m_container=4;
  
  return StatusCode::SUCCESS;

}
 
 					
StatusCode MuonDigitization::execute()
{
  //	IChronoStatSvc* svc = chronoSvc();	
  
  //  IChronoStatSvc::ChronoTime time;	 
  MsgStream log(msgSvc(), name());
  
 	StatusCode sc=StatusCode::SUCCESS ;
  
  log<<MSG::DEBUG<<"starting the Muon Digitization algorithm "<<endreq;
  SmartDataPtr<MCMuonHits> hitPointer(eventSvc(),"/Event/MC/Muon/M3/R1/Hits");
  // 	 cout<<" alessia testa il generatore flat "<<m_flatDist()<<endl;
  //log<<MSG::INFO<<"alessia "<<hitPointer<<endreq;
  
  if(hitPointer!=0){
    MCMuonHits::const_iterator i;
    for (i=(hitPointer)->begin();i<(hitPointer)->end();i++){         
      if(m_verboseDebug){
        log<<MSG::INFO<<"muon x , y, z , exit  "<< (*i)->exit().x() <<" " <<
          (*i)->exit().y() << "  " <<                          
          (*i)->exit().z() << endreq ;																		
        log<<MSG::DEBUG<<"muon x , y, z entry ,  "<< (*i)->entry().x() <<" " <<
          (*i)->entry().y() << "  " <<(*i)->entry().z() << endreq ;   
        log<<MSG::DEBUG<<"time of flight ,  "<< (*i)->timeOfFlight()<< endreq; 
        log<<MSG::DEBUG<<" chamber and gap ID	"<<
          (*i)->chamberID()<<" "<<(*i)->gapID()<<endreq;
        MCParticle* origparticle=(*i)->mcParticle();
        if(origparticle)  {
          log<<MSG::DEBUG<<"Particle from which it originates (PDG code)"<<
            origparticle->particleID().abspid()<<endreq;
        }
        else{
          log<<MSG::WARNING<<
            "Particle from which it originates is not defined "<< endreq;
        }
      }	
    }
	}	
	else{
    log<<MSG::ERROR<<"unable to retrieve the hit container"<<endreq; 
  } 
    

	
	if(m_applyChamberNoise) sc=addChamberNoise();
	if(sc.isFailure())return StatusCode::FAILURE;
	
	MuonDigitizationData<MuonPhyChannelInput> PhyChaInput("MuPI",&log,
                                                        eventSvc(),"MuPI") ;
	sc=createInput(PhyChaInput);
	if(sc.isFailure())return StatusCode::FAILURE;	
 	MuonDigitizationData<MuonPhysicalChannel> PhyChaOutput("MuPO",&log,
                                                         eventSvc(),"MuPO") ;
  sc=elaborateMuonPhyChannelInputs(PhyChaInput,PhyChaOutput);	
	if(sc.isFailure())return StatusCode::FAILURE;	 	
	if(m_applyElectronicNoise)	sc=addElectronicNoise(PhyChaOutput);
	if(sc.isFailure())return StatusCode::FAILURE;	
  
  
  sc=applyPhysicalEffects(PhyChaOutput);
	if(sc.isFailure())return StatusCode::FAILURE;	
  if(m_verboseDebug){	
    for(int i=0; i<m_partition; i++){
	    log<<MSG::INFO<<"  last print container number "<<i<<endreq;
      MuonPhysicalChannels::const_iterator iter=
        PhyChaOutput.getPartition(i)->begin();
		  for(iter=PhyChaOutput.getPartition(i)->begin();iter<
            PhyChaOutput.getPartition(i)->end();iter++){		    
        log<<MSG::INFO<<"#FE ID "<<(*iter)->phChID()->getID()<<endreq;
        log<<MSG::INFO<<"#Station "<<(*iter)->phChID()->getStation()<<endreq;
        log<<MSG::INFO<<"#Region "<<(*iter)->phChID()->getRegion()<<endreq;
        log<<MSG::INFO<<"#Quadrant "<<(*iter)->phChID()->getQuadrant()<<endreq;
        log<<MSG::INFO<<"#Chamber "<<(*iter)->phChID()->getChamber()<<endreq;
        log<<MSG::INFO<<"# ch X "<<(*iter)->phChID()->getPhChIDX()<<endreq;
        log<<MSG::INFO<<"# ch Y "<<(*iter)->phChID()->getPhChIDY()<<endreq;
        log<<MSG::INFO<<"#frontend "<<(*iter)->phChID()->getFrontEnd()<<endreq;
      }
    }
    
  }
  
  
	MuonDigitizationData<MuonPhysicalChannelOutput> 
    PhysicalChannelOutput("MULC",&log,eventSvc(),"MULC") ;
	sc=fillPhysicalChannel(PhyChaOutput,PhysicalChannelOutput);
	if(sc.isFailure())return StatusCode::FAILURE;	 			
  if(m_verboseDebug){	
    for(int i=0; i<m_partition; i++){
	    log<<MSG::INFO<<"  last print container number "<<i<<endreq;
      MuonPhysicalChannelOutputs::const_iterator iter=
        PhysicalChannelOutput.getPartition(i)->begin();
		  for(iter=PhysicalChannelOutput.getPartition(i)->begin();iter<
            PhysicalChannelOutput.getPartition(i)->end();iter++){		    
        log<<MSG::INFO<<"FE ID "<<(*iter)->phChID()->getID()<<endreq;
        log<<MSG::INFO<<"Station "<<(*iter)->phChID()->getStation()<<endreq;
        log<<MSG::INFO<<"Region "<<(*iter)->phChID()->getRegion()<<endreq;
        log<<MSG::INFO<<"Quadrant "<<(*iter)->phChID()->getQuadrant()<<endreq;
        log<<MSG::INFO<<"Chamber "<<(*iter)->phChID()->getChamber()<<endreq;
        log<<MSG::INFO<<" ch X "<<(*iter)->phChID()->getPhChIDX()<<endreq;
        log<<MSG::INFO<<" ch Y "<<(*iter)->phChID()->getPhChIDY()<<endreq;
        log<<MSG::INFO<<" frontend "<<(*iter)->phChID()->getFrontEnd()<<endreq;
        log<<MSG::INFO<<" fired "<<(*iter)->phChInfo().isAlive()<<endreq;
        log<<MSG::INFO<<" nature "<<(*iter)->phChInfo().natureOfHit()<<endreq;
        std::vector<MuonHitTraceBack> vector_traceBack=(*iter)->
          hitsTraceBack();
        std::vector<MuonHitTraceBack>::iterator iterTraceBack=
          (vector_traceBack).begin(); 
        for(iterTraceBack=(vector_traceBack).begin();iterTraceBack<
              (vector_traceBack).end();iterTraceBack++){
          log<<MSG::INFO<<"hit time "<<(*iterTraceBack).hitArrivalTime() 
             <<endreq; 
        }
		  }
    }
	}

 
  MCMuonDigits* mcDigitContainer= new MCMuonDigits;
  sc=createLogicalChannel(PhysicalChannelOutput, *mcDigitContainer);
	if(sc.isFailure())return StatusCode::FAILURE;	 			
	eventSvc()->registerObject(MCMuonDigitLocation::MCMuonDigit,
                             mcDigitContainer);
  MuonDigits* digitContainer= new MuonDigits;	
	sc=createRAWFormat(*mcDigitContainer, *digitContainer);
	if(sc.isFailure())return StatusCode::FAILURE;	 				
	eventSvc()->registerObject(MuonDigitLocation::MuonDigit,digitContainer);
  
 	log<<MSG::DEBUG<<"End of the Muon Digitization"<<endreq;
  return StatusCode::SUCCESS;
  
}

StatusCode MuonDigitization::finalize()
{
  // Release the tools
  if( m_pMuonTileXYZ ) toolSvc()->releaseTool( m_pMuonTileXYZ );
  if( m_pGetInfo ) toolSvc()->releaseTool( m_pGetInfo );
 
  return StatusCode::SUCCESS;
}

StatusCode
MuonDigitization::addChamberNoise(){

	MsgStream log(msgSvc(), name()); 
  int container=1;
	for (int ispill=1;ispill<m_numberOfEventsNeed+1;ispill++){
	  int chamberTillNow=0;
		double shiftOfTOF=-m_BXTime*ispill;
    for(int k=0;k<m_stationNumber;k++){ 
	    for(int s=0;s<m_regionNumber;s++){
        KeyedContainer<MCMuonHit>* hitsContainer = new 
          KeyedContainer<MCMuonHit>();
        std::string path="/Event"+spill[ispill]+"/MC/Muon/"+numsta[k]+
          "/R"+numreg[s]+"/"+TESPathOfHitsContainer[container];
        int partitionNumber=k*m_regionNumber+s;
        int chamberInRegion=m_pGetInfo->getChamberPerRegion(partitionNumber);
        int gapPerChamber=m_pGetInfo->getGapPerRegion(partitionNumber);
        for (int chamber=0;chamber<chamberInRegion;chamber++){			   
          int numberOfNoiseHit=(detectorResponse.getChamberResponse
                                (partitionNumber))->extractNoise();
          
          // the readout number is essentially meaningless... 
          //it is chamber noise.....					 
          int readout=0;
          double startPosZ=m_pGetInfo->getStartPositionFirstGapZ
            (chamber+ chamberTillNow);
          for (int hit=1;hit<=numberOfNoiseHit;hit++){
            //define position of hit 
            //first gap number
            int gap=(int)((m_flatDist()*gapPerChamber)); 
            if(gap>3)gap=3;
            int frontEnd=(int)(gap/m_pGetInfo->getGapPerFE(partitionNumber));
            int gapFE=m_pGetInfo->getGapPerFE(partitionNumber);
            double middlePosZ=startPosZ+(m_pGetInfo->getStopGapZ
                                         ((frontEnd+1)*gapFE-1 ,
                                          partitionNumber)-
                                         m_pGetInfo->getStartGapZ
                                         (frontEnd*gapFE ,
                                          partitionNumber))/2+
              m_pGetInfo->getStartGapZ(frontEnd*gapFE ,partitionNumber);
            //then x&y	
            float x= m_flatDist()* m_pGetInfo->
              getPhChannelSizeX(readout,partitionNumber)*
              m_pGetInfo->getPhChannelNX(readout,partitionNumber)+
              m_pGetInfo-> getStartChamberPositionX(chamber+chamberTillNow);
            float y= m_flatDist()* 
              m_pGetInfo->getPhChannelSizeY(readout,partitionNumber)*
              m_pGetInfo->getPhChannelNY(readout,partitionNumber)+
              m_pGetInfo-> getStartChamberPositionY(chamber+chamberTillNow);
            float time=		m_flatDist()*m_BXTime;	
            MCMuonHit* pHit = new MCMuonHit();
            pHit->setEntry(HepPoint3D(
                                      x,y,m_pGetInfo->
                                      getStartPositionFirstGapZ
                                      (chamber+chamberTillNow)+
                                      m_pGetInfo->getStartGapZ
                                      (gap,partitionNumber)));
            pHit->setExit(HepPoint3D(x,y,m_pGetInfo->
                                     getStartPositionFirstGapZ
                                     (chamber+chamberTillNow)+
                                     m_pGetInfo->
                                     getStopGapZ(gap,partitionNumber)));
            double tofOfLight=(sqrt(x*x+ y*y+(middlePosZ)*
                                    (middlePosZ)))/300.0;
            pHit->setTimeOfFlight(time+shiftOfTOF+tofOfLight); 
            pHit->setVolumeID(chamber+1,gap+1);
            hitsContainer->insert(pHit);
            if(m_verboseDebug){	
              log<<MSG::INFO<<"adding chamber noise hit "<<
                  ispill<<" "<<k<<" "<<s<<numberOfNoiseHit<<endreq;
            }	
          }
          if(m_verboseDebug){	
            if(numberOfNoiseHit>0)
              log<<MSG::INFO<<"adding chamber noise hit "<<ispill<<" "<<
                k<<" "<<s<<" chamber "<<chamber <<" "<<
                numberOfNoiseHit<<endreq;
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
                              MuonDigitizationData<MuonPhyChannelInput>& 
                              PhyChaInput)
{
  MsgStream log(msgSvc(), name()); 
  unsigned int chamberTillNow=0;		
  //loop over the containers
  for(int iterRegion=0;iterRegion<m_partition;iterRegion++){    
    std::vector<MuonPhPreInput> keepTemporary ;		 
    int station=iterRegion/m_regionNumber+1;
    int region=iterRegion%m_regionNumber+1;	 
    int readoutNumber=m_pGetInfo->getReadoutNumber(iterRegion);		 
    for (int ispill=0; ispill<m_numberOfEventsNeed+1;ispill++){
      for(int container=0; container<m_container;container++){				
        std::string path="/Event"+spill[ispill]+"/MC/Muon/"+
          numsta[station-1]+"/R"+numreg[region-1]+"/"+
          TESPathOfHitsContainer[container];
        if(m_verboseDebug) {log<<MSG::INFO<<"hit container path "<<
                              path<<endreq;}
       
        SmartDataPtr<MCMuonHits> hitPointer(eventSvc(),path);
        MCMuonHits::const_iterator iter;	 
        if(hitPointer!=0){
          for (iter=(hitPointer)->begin();iter<(hitPointer)->end();iter++)
            {           
              // evaluate slop and intercept in the transverse plane .. 
              //useful later
              
              bool parallelFlag=false ; 
              float slopeY;
              float intercept;
              if((*iter)->exit().x()!=(*iter)->entry().x()){
                slopeY=((*iter)->exit().y()-(*iter)->entry().y())/
                  ((*iter)->exit().x()-(*iter)->entry().x());
                intercept=	(*iter)->exit().y()	- slopeY*(*iter)->exit().x() ;
              }else{
                parallelFlag=true ; 
              }
              for(int ireadout=0;ireadout<=readoutNumber;ireadout++){
                float phChSizeX= m_pGetInfo->getPhChannelSizeX( ireadout, 
                                                            iterRegion);
                float phChSizeY= m_pGetInfo->getPhChannelSizeY( ireadout, 
                                                            iterRegion);
                unsigned int readoutType=  m_pGetInfo->
                  getReadoutType(ireadout,iterRegion);
                // how many p.c. are crossed by the hit???
                
                int chamber= (int)chamberTillNow + (*iter)->chamberID()-1 ;
                int gap=(*iter)->gapID()-1 ;
                
                int frontEnd = gap/m_pGetInfo->getGapPerFE(iterRegion);
                int gapFE=m_pGetInfo->getGapPerFE(iterRegion);
                
                double startPosX=m_pGetInfo->
                  getStartChamberPositionX(chamber) ;
                double startPosY= m_pGetInfo->
                  getStartChamberPositionY(chamber)	;	
                double middlePosZ=m_pGetInfo->
                  getStartPositionFirstGapZ(chamber)+
                  (m_pGetInfo->getStopGapZ((frontEnd+1)*gapFE-1 ,iterRegion)-
                 m_pGetInfo->getStartGapZ(frontEnd*gapFE ,iterRegion))/2
                  +m_pGetInfo->getStartGapZ(frontEnd*gapFE ,iterRegion);
                unsigned int nxChaEntry ;
                unsigned int nxChaExit  ;
                int tmpNxChaEntry=(int) (((*iter)->entry().x()-
                                          startPosX)/phChSizeX);
                int tmpNxChaExit=(int) (((*iter)->exit().x()-
                                         startPosX)/phChSizeX);
                double xstart,xstop;
                double ystart,ystop;
                if(m_verboseDebug) {
                  log<<MSG::INFO<<"processing hit  "<<
                  ireadout<<" "<<chamber<<" "<<gap<<endreq;
                  log<<MSG::INFO<<" hit position in x "<< 
                    (*iter)->entry().x()<<" "<<(*iter)->exit().x()<<endreq;
                  log<<MSG::INFO<<" hit position in y "<< 
                    (*iter)->entry().y()<<" "<<(*iter)->exit().y()<<endreq;		 
                  log<<MSG::INFO<<"x--direction start stop  "<<
                    tmpNxChaEntry<<" "<<tmpNxChaExit<<endreq;
                  log<<MSG::INFO<<"x start stop  "<<startPosX<<" "<<
                    startPosY<<endreq;
                  
                }
                
                if(tmpNxChaEntry<=tmpNxChaExit){
                  if( tmpNxChaEntry<0)tmpNxChaEntry=0;
                  if( tmpNxChaExit>=(int)m_pGetInfo->getPhChannelNX(ireadout, 
                                                                iterRegion))
                    tmpNxChaExit=(int)m_pGetInfo->getPhChannelNX(ireadout, 
                                                             iterRegion)-1 ;	
									xstart=	(*iter)->entry().x()			;
									xstop=	(*iter)->exit().x()			;			
						      nxChaEntry = (unsigned int) tmpNxChaEntry;
						      nxChaExit  = (unsigned int) tmpNxChaExit;										
                  
                }
                // normal order								
                else{
                  // inverse order					
                  if( tmpNxChaExit<0)tmpNxChaExit=0;
                  if( tmpNxChaEntry>=(int)m_pGetInfo->getPhChannelNX(ireadout, 
                                                                 iterRegion))
                    tmpNxChaEntry=(int)m_pGetInfo->getPhChannelNX(ireadout, 
                                                              iterRegion)-1 ;	
                  xstop=	(*iter)->entry().x()			;
                xstart=	(*iter)->exit().x()			;				
                nxChaEntry = (unsigned int) tmpNxChaExit;
                nxChaExit  = (unsigned int) tmpNxChaEntry;										
                }   
                unsigned int quarter=m_pGetInfo->
                  getQuadrantChamber(chamber);		
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
                  int tmpYBegin=static_cast<int> ((yBegin-startPosY)/
                                                  (phChSizeY));
                  int tmpYEnd=static_cast<int> ((yEnd-startPosY)/(phChSizeY));
                  if(tmpYBegin<=tmpYEnd){
                    if( tmpYBegin<0)tmpYBegin=0;
                    if( tmpYEnd>=(int)m_pGetInfo->getPhChannelNY(ireadout, 
                                                             iterRegion))
                      tmpYEnd=(int)m_pGetInfo->getPhChannelNY(ireadout, 
                                                          iterRegion)-1 ;	
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
                  if( tmpYBegin>=(int)m_pGetInfo->getPhChannelNY
                      (ireadout, iterRegion))
                    tmpYBegin=(int)m_pGetInfo->getPhChannelNY(ireadout, 
                                                          iterRegion)-1 ;
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
                    /* if((xi-((Xloop)*phChSizeX+startPosX))<
                       (xf-((Xloop)*phChSizeX+startPosX))){
                       distanceFromBoundary[0]=xi-((Xloop)*phChSizeX
                       +startPosX);}
                       else{
                       distanceFromBoundary[0]=xf-((Xloop)*phChSizeX+
                       startPosX);}
                       if((yi-((Yloop)*phChSizeY+startPosY))<
                       (yf-((Yloop)*phChSizeY+startPosY))){	
                       distanceFromBoundary[1]=yi-((Yloop)*phChSizeY+
                       startPosY);}
                       else{
                       distanceFromBoundary[1]=yf-((Yloop)*phChSizeY+
                       startPosY);}
                       if((((Xloop+1)*phChSizeX+startPosX)-xi)<
                       (((Xloop+1)*phChSizeX+startPosX)-xf)){	
                       distanceFromBoundary[2]=((Xloop+1)*phChSizeX+
                       startPosX)-xi;}
                       else{
                       distanceFromBoundary[2]=	((Xloop+1)*phChSizeX+
                       startPosX)-xf;}	
                       if((((Yloop+1)*phChSizeY+startPosY)-yi)<
                       (((Yloop+1)*phChSizeY+startPosY)-yf)){						
                       distanceFromBoundary[3]= ((Yloop+1)*phChSizeY+
                       startPosY)-yi;}
                       else{	
                       distanceFromBoundary[3]= ((Yloop+1)*phChSizeY+
                       startPosY)-yf;}
                    */
                  distanceFromBoundary[0]=(xi+xf)/2-
                    ((Xloop)*phChSizeX+startPosX);
                  distanceFromBoundary[1]=(yi+yf)/2-
                    ((Yloop)*phChSizeY+startPosY);		
                  distanceFromBoundary[2]=((Xloop+1)*
                                           phChSizeX+startPosX)-(xi+xf)/2 ;
                  distanceFromBoundary[3]= ((Yloop+1)*
                                            phChSizeY+startPosY)-(yi+yf)/2 ;
                  MuonPhPreInput* inputPointer = new  MuonPhPreInput  ;				
                  inputPointer->phChID()->
                    setStation((unsigned int)(station-1));
                  inputPointer->phChID()->setRegion((unsigned int)(region-1));
                  inputPointer->phChID()->setQuadrant(quarter);
                  inputPointer->phChID()->setChamber((*iter)->chamberID()-1 );
                  inputPointer->phChID()->setPhChIDX(Xloop);
                  inputPointer->phChID()->setPhChIDY(Yloop);								
                  inputPointer->phChID()->setFrontEnd((unsigned int) frontEnd);
                  inputPointer->phChID()->setReadout(readoutType);
                  inputPointer->phChID()->setGap(gap);
                  inputPointer->getHitTraceBack()->setMCMuonHit(*iter); 
                  
                  //correct for the  tof .... i.e. subtract the tof that 
                  //a lightparticle impacting the center of the pc. has....
                  double tofOfLight=(sqrt(((Xloop+0.5)*
                                           phChSizeX+startPosX)*
                                          ((Xloop+0.5)*phChSizeX+startPosX)+
                                          ((Yloop+0.5)*phChSizeY+startPosY)
                                          *((Yloop+0.5)*phChSizeY+startPosY)
                                          +(middlePosZ)*(middlePosZ)))/300.0;
																				
                  inputPointer->getHitTraceBack()
                    ->setHitArrivalTime((*iter)->timeOfFlight()-tofOfLight);
                  inputPointer->getHitTraceBack()
                    ->setCordinate(distanceFromBoundary);
                  if(ispill>0){
                    inputPointer->getHitTraceBack()->getMCMuonHitOrigin().
                      setBX(ispill-1);
                     inputPointer->getHitTraceBack()
                    ->getMCMuonHitOrigin().setHitNature
                    (OriginOfHitsContainer[container]);	 
                     inputPointer->getHitTraceBack()
                    ->getMCMuonHistory().setBXOfHit(ispill-1);
                     inputPointer->getHitTraceBack()
                       ->getMCMuonHistory().setNatureOfHit
                       (OriginOfHitsContainer[container]);			
                  }
                  //patch for machine background
                  if(ispill==0){
                    inputPointer->getHitTraceBack()->getMCMuonHitOrigin().
                      setBX(ispill); 
                    inputPointer->getHitTraceBack()
                    ->getMCMuonHitOrigin().setHitNature
                      (OriginOfHitsContainer[4]);	 
                    inputPointer->getHitTraceBack()
                      ->getMCMuonHistory().setBXOfHit(ispill);
                    inputPointer->getHitTraceBack()
                    ->getMCMuonHistory().setNatureOfHit
                      (OriginOfHitsContainer[4]);			
                  }
                  
                 /*  if(	OriginOfHitsContainer[container]>1){
                      inputPointer->getHitTraceBack()->
                      getMCMuonHistory().setNatureOfHit
                      (OriginOfHitsContainer[container]);			
                      
										  log<<MSG::INFO<<"bug  "<<	
                      OriginOfHitsContainer[container]<<"			"
                      <<inputPointer->getHitTraceBack()
                      ->getMCMuonHistory().natureOfHit()<<endreq;
                      }	 	*/		
                  inputPointer->getHitTraceBack()->setCordinate
                    (distanceFromBoundary);
                  if(m_verboseDebug){	
                    int code=0;
                    if((*iter)->mcParticle())code=(*iter)
                                               ->mcParticle()->particleID().
                                               abspid();                    
                    log<<MSG::INFO<<"hit processing "<<station<<" "<< 
                      region<<" " << quarter<<" "<< chamber
                       <<" "<<gap<<" "<<frontEnd<<" "<<ireadout<<" "<<
                      " "<<Xloop<<" "<<Yloop<<" "<<
                      m_pGetInfo->getPhChannelNX(ireadout, iterRegion)<<
                      " "<<m_pGetInfo->getPhChannelNY(ireadout, iterRegion)  
                       <<" "<<tofOfLight<<" "<< 
                      OriginOfHitsContainer[container]<<" "<<
                      ispill<<" "<< code<<endreq;		
                    log<<MSG::INFO<<" ph ch ID "<<
                      *(inputPointer->phChID())<<" id "<<
                      inputPointer->phChID()->getID()<<endreq;
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
		for(iterPost=keepTemporary.rbegin();iterPost<keepTemporary.rend();
        ++iterPost){
      if(m_verboseDebug){
        /*           log<<MSG::DEBUG<<"ID  Post "<< 
                     (iterPost)->phChID()->getID()<<
                     " hit origin "<< (iterPost)->getHitTraceBack()
                     ->getMCMuonHitOrigin().getFlatSpilloverNature() 
                     <<endreq;	*/	
      }
      MuonPhyChannelInput* phChPointer = 
				new MuonPhyChannelInput((iterPost)->phChID()->getID(),
                                *((iterPost)->getHitTraceBack())) ;	
      PhyChaInput.addMuonObject(iterRegion,phChPointer);		 	 
      keepTemporary.pop_back(); 	
    } 	  
    //!!!!!!!!!!!!!  left it as the last instruction of the for loop    !!!! 	
    chamberTillNow=chamberTillNow+m_pGetInfo->
      getChamberPerRegion(iterRegion);		 
	}   
  return StatusCode::SUCCESS;
}

StatusCode MuonDigitization::
elaborateMuonPhyChannelInputs(
                              MuonDigitizationData<MuonPhyChannelInput> & 
                              PhyChaInput,
                              MuonDigitizationData<MuonPhysicalChannel>& 
                              PhysicalChannel){
  MsgStream log(msgSvc(), name()); 					 
  for (int i=0; i<m_partition;i++){
    if(!PhyChaInput.isEmpty(i)){	  
      MuonPhyChannelInputs::const_iterator inputIter ;
      MuonPhyChannelInputs::const_iterator inputIterStart=
        PhyChaInput.getPartition(i)->begin();
      /* 		  int station=i/4+1;
              int region=i%4+1;	 
              std::string text="/Event/MC/Muon/"+numsta[station-1]+
              "/R"+numreg[region-1]+"/Hits";
              DataObject* pPart;
              StatusCode sc=eventSvc()->retrieveObject(text, 
              (DataObject*&)pPart);		*/
      unsigned int prevFE=(*inputIterStart)->phChID()->getFETile();
      unsigned int lastFE;
      //			unsigned int prevID=(*inputIterStart)->phChID()->getID();
      //			unsigned int lastID;
      MuonHitTraceBack* pointerToHitTraceBack=(*inputIterStart)
        ->getHitTraceBack();
      MuonPhysicalChannel* outputPointer = new MuonPhysicalChannel
        (prevFE,m_gate,m_BXTime);
      //if(i==4)cout<<" ciao "<<(*inputIterStart)
      //->phChID()->getReadout()<<endl;
      
      outputPointer->setResponse
        (detectorResponse.getResponse(*((*inputIterStart)->phChID())));
      if(!detectorResponse.getResponse(*((*inputIterStart)->phChID()))){
        log<<MSG::ERROR<<"unable to retrieve the response of  ph. channel"
           <<endreq;
      }
      outputPointer->hitsTraceBack().push_back(*pointerToHitTraceBack);
      PhysicalChannel.addMuonObject(i,outputPointer);
      for (inputIter=++inputIterStart;inputIter
             <PhyChaInput.getPartition(i)->end();inputIter++){
        lastFE=(*inputIter)->phChID()->getFETile();
        pointerToHitTraceBack=(*inputIter)->getHitTraceBack();
        if(lastFE==prevFE){
          outputPointer->hitsTraceBack().push_back(*pointerToHitTraceBack) ;
        }else{
          outputPointer = new MuonPhysicalChannel(lastFE,m_gate,m_BXTime);
          outputPointer->setResponse
            (detectorResponse.getResponse(*((*inputIter)->phChID())));
        
          
          outputPointer->hitsTraceBack().push_back(*pointerToHitTraceBack) ;
          prevFE=lastFE;
          PhysicalChannel.addMuonObject(i,outputPointer);
        }			
      }						
    }
  }
    return StatusCode::SUCCESS;	
}

StatusCode  MuonDigitization::
fillPhysicalChannel(MuonDigitizationData<MuonPhysicalChannel>& 
                    PhysicalChannel,
                    MuonDigitizationData<MuonPhysicalChannelOutput>& 
                    PhysicalChannelOutput){
  MuonPhysicalChannels::iterator iterInput;
  //		  MuonPhysicalChannelOutputs::iterator iterOutput;
  MsgStream log(msgSvc(), name()); 					 
  
  for (int i=0; i<m_partition;i++){
    if(!PhysicalChannel.isEmpty(i)){				
      for(iterInput=PhysicalChannel.getPartition(i)->begin();
          iterInput<PhysicalChannel.getPartition(i)->end();iterInput++){
        MuonPhysicalChannelOutput* objToAdd=new 
          MuonPhysicalChannelOutput(*(*iterInput));
        bool fired=false;
        bool interestingHit=false;
        double timeOfFiring;
        std::vector<MuonHitTraceBack>::iterator iterInHits ;
        if(m_verboseDebug)log<<MSG::INFO<<objToAdd->phChID()->
                            getID()<<" "<<*(objToAdd->phChID())<<endreq;
        // if(i==0)	cout<<" start nuovo mupnphysicalchannel "<<endl;
        for(iterInHits=objToAdd->hitsTraceBack().begin();
            iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){
          if(m_verboseDebug)log<<MSG::INFO<<" geo acce "<<
                              (*iterInHits).getMCMuonHistory().
                              isHitOutGeoAccemtance()<<
                              " cha ineff "<<
                              (*iterInHits).getMCMuonHistory().
                                isKilledByChamberInefficiency()<<
                              " in deadtime "<<
                              (*iterInHits).getMCMuonHistory().
                              isHitInDeadtime()<<
                              " time of firing "<< 
                              (*iterInHits).hitArrivalTime()<<endreq;				 
          if(!fired){
            if(!(*iterInHits).getMCMuonHistory().isHitOutGeoAccemtance()&&
               !(*iterInHits).getMCMuonHistory().
               isKilledByChamberInefficiency()&&
               !(*iterInHits).getMCMuonHistory().isHitInDeadtime()){
              timeOfFiring=(*iterInHits).hitArrivalTime();
              if(timeOfFiring>0&&timeOfFiring<m_gate){
                // just test
                //										 if(timeOfFiring>0&&timeOfFiring<20){
                
                fired=true;
                (*iterInHits).getMCMuonHistory().setFiredFlag((unsigned int)1);
                objToAdd->setFiringTime(timeOfFiring);		
                objToAdd->phChInfo().setAliveDigit(1);
                objToAdd->phChInfo().setBXIDFlagHit((*iterInHits).
                                                    getMCMuonHistory().BX());
                objToAdd->phChInfo().setNatureHit((*iterInHits).
                                                  getMCMuonHitOrigin().
                                                  getNature());
                if((*iterInHits).getMCMuonHistory().
                   isHitOriginatedInCurrentEvent()){
                  if((*iterInHits).getMCMuonHistory().
                     isInForTimeAdjustment()){
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
        if(m_verboseDebug)log<<MSG::INFO<<" fired "<<fired<<endreq;
        if(!fired){
          for(iterInHits=objToAdd->hitsTraceBack().begin();
              iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){
            if((*iterInHits).getMCMuonHistory().
               isHitOriginatedInCurrentEvent()){
              interestingHit=true;
              
              if((*iterInHits).getMCMuonHistory().hasTimeJittered()){
                if(!((*iterInHits).getMCMuonHistory().
                     isInForTimeAdjustment())){
                  // first source of dead time jitter                  
                  objToAdd->phChInfo().setTimeJitteredDigit(1);						
                }else if ((*iterInHits).getMCMuonHistory().
                          isHitInDeadtime()){
                  // remember to check that the time adjustment do not 
                  // put back the hit int he gate....=> 
                  // only deadtime can kill this digit 											 
                  objToAdd->phChInfo().setDeadtimeDigit(1);
                }	 
              }
              if((*iterInHits).getMCMuonHistory().
                 isKilledByChamberInefficiency()){
                // hit is killed by chamber inefficiency
                objToAdd->phChInfo().setChamberInefficiencyDigit(1);
              }else   if((*iterInHits).getMCMuonHistory().
                         isHitOutGeoAccemtance()){
                // hit is killed by geomatrial acceptance 
                objToAdd->phChInfo().setGeometryInefficiency(1);
              }else if ((*iterInHits).getMCMuonHistory().isHitInDeadtime()){
                // hit in deadtime
                objToAdd->phChInfo().setDeadtimeDigit(1);
              }else   if((*iterInHits).getMCMuonHistory().
                         isOutForTimeAdjustment()){
                // hit is killed by time adjustment
                objToAdd->phChInfo().setTimeAdjDigit(1);								
              }  
            }
          }
        }
        
        // debug printout
        
        bool muon=false;
        for(iterInHits=objToAdd->hitsTraceBack().begin();
            iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){				
 // search the muon first...
          if((iterInHits)->getMCMuonHit()){
            MCParticle* particle=(iterInHits)->getMCMuonHit()->mcParticle();
            if(particle){
              int pid= particle->particleID().abspid();
              if(pid==13||pid==-13){
                log<<MSG::DEBUG<<"moun hit   time   ??????"<<
                    (iterInHits)-> hitArrivalTime()	<<endreq;	
                muon=true;
              }
            }
          }							 
        }
        if(muon){
          if(m_verboseDebug)
              log<<MSG::INFO<<"**** start new pc****   station  and region "
                 <<i <<" fired "<<fired<<endreq;	
          for(iterInHits=objToAdd->hitsTraceBack().begin();
              iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){	
              
            if(m_verboseDebug)log<<MSG::INFO<<"time"<<(iterInHits)-> 
                                hitArrivalTime()	<<" tile "<<
                                  objToAdd->phChID()->getFETile()<<endreq;	
            if(m_verboseDebug)log<<MSG::INFO<<	" deadtime "<<
                                (*iterInHits).getMCMuonHistory().
                                isHitInDeadtime()<<" time jitter "<<
                                (*iterInHits).getMCMuonHistory().
                                  hasTimeJittered() <<" efficiency  "<<
                                (*iterInHits).getMCMuonHistory().
                                isKilledByChamberInefficiency()<<endreq;
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



 

StatusCode MuonDigitization::
applyPhysicalEffects(MuonDigitizationData<MuonPhysicalChannel>&
                     PhysicalChannel){
	MsgStream log(msgSvc(), name()); 	
  
  
  //loop over the 20 containers 
  
  for (int i=0; i<m_partition;i++){	
	  if(!PhysicalChannel.isEmpty(i)){	  
		  std::vector<MuonPhysicalChannel*>  XTalkPhysicalChannel;
			std::vector<MuonPhysicalChannel*>::iterator iterOnSTD;
		  MuonPhysicalChannels::iterator iter ;
			std::vector<MuonPhysicalChannel*> channelsDueToXTalk;	   
			std::vector<MuonPhysicalChannel*>::iterator iterXTalk;
			MuonPhysicalChannel* pFound;
			int phChInX[2] ;
			int phChInY[2] ;
      for (int iloop=0; iloop<=m_pGetInfo->getReadoutNumber(i);iloop++){
			 	phChInX[(int)m_pGetInfo->getReadoutType(iloop,i)]	=
          m_pGetInfo->getPhChannelNX( iloop, i);
			 	phChInY[(int)m_pGetInfo->getReadoutType(iloop,i)]	=
          m_pGetInfo->getPhChannelNY( iloop, i);
      }
      for(iter=PhysicalChannel.getPartition(i)->begin();
			    iter<PhysicalChannel.getPartition(i)->end();iter++){
        //					if(i==0)cout<<" alessia diventa scema "<<endl;
        // apply per pc the time jitter on each hit	
        if( m_applyTimeJitter)(*iter)->applyTimeJitter();
        // apply per pc the geometry inefficiency 	on each hit			
        if(m_applyEfficiency)(*iter)->applyGeoInefficiency();					 
        
        // apply per pc the chamber inefficiency 	on each hit				
        if(m_applyEfficiency)(*iter)->applyChamberInefficiency();	
        
        // apply per pc the X Talk on each hit				
        // start Xtalk   
        
        if(m_applyXTalk)(*iter)->
                          applyXTalk(phChInX, phChInY,channelsDueToXTalk);
        for(iterXTalk=channelsDueToXTalk.begin();
            iterXTalk<channelsDueToXTalk.end();iterXTalk++){
          pFound=0;
          bool already=PhysicalChannel.isObjectIn(i,*iterXTalk,
                                                  pFound, ComparePC);
          if(!already){
            (*iterXTalk)->setResponse(detectorResponse.
                                      getResponse(*((*iterXTalk)->phChID())));
 							 XTalkPhysicalChannel.push_back(*iterXTalk);
          }else{
            log<<MSG::DEBUG<<"xtalk hit test  molto dopo "<<
              (*iter)<<" "<<*iterXTalk<<" "<<pFound<<endreq;
            if(pFound==(*iter))log<<MSG::ERROR<<"alessia gran casino"<<endreq;
            pFound->addToPC(*iterXTalk);
            delete *iterXTalk;
          }
        }
        //end Xtalk
        // empty the Xtalked PC container		
        while(!channelsDueToXTalk.empty()){
          channelsDueToXTalk.pop_back();
        }
      }
      // add the xtalk hit to the main container
      
      for(iterOnSTD=XTalkPhysicalChannel.begin();
			    iterOnSTD<XTalkPhysicalChannel.end();iterOnSTD++){
        pFound=0;
        bool already=PhysicalChannel.
          isObjectIn(i,*iterOnSTD,pFound, ComparePC);
        if(already){
          pFound->addToPC(*iterOnSTD); 
          delete *iterOnSTD;
          //						 if(i==0)cout<<" giammai already found "<<endl;
        }
        else{
          PhysicalChannel.addMuonObject(i,*iterOnSTD);
          //						 if(i==0)cout<<" real new  "<<endl;
          std::vector<MuonHitTraceBack>::iterator hji ;
          if(i==0){
            for(hji=(*iterOnSTD)->hitsTraceBack().begin();
                hji<(*iterOnSTD)->hitsTraceBack().end();hji++){
           }
          }
        }        
      } 
      // start deadtime
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
          }
        }			  
        //apply time adjustment	
        if(m_applyTimeAdjustment)(*iter)->applyTimeAdjustment();	
        //apply deadtime
        if(m_applyDeadtime)(*iter)-> applyDeadtime(m_numberOfEventsNeed) ;
			}	
      //end deadtime			 			 			  									 			
	  }				
  }
  return StatusCode::SUCCESS ;	
}






StatusCode MuonDigitization::
createLogicalChannel(MuonDigitizationData<MuonPhysicalChannelOutput>&
                     PhyChaOutput, MCMuonDigits& mcDigitContainer){
	MsgStream log(msgSvc(), name()); 
  //  int flag=0;
  int countDigits=0;
  for(int i=0; i<m_partition; i++){
    MuonPhysicalChannelOutputs::const_iterator iter =
      PhyChaOutput.getPartition(i)->begin();
    MCMuonDigits::iterator iterDigit;        
    for(iter=PhyChaOutput.getPartition(i)->begin();iter<
          PhyChaOutput.getPartition(i)->end();iter++){	
      if(m_verboseDebug){
        log<<MSG::DEBUG<<"FE ID "<<(*iter)->phChID()->getID()<<endreq;}
      MuonTileID phChTileID[2];
      int numberOfTileID;  
      if(m_verboseDebug)log<<MSG::INFO<<"FE ID "<<
                          (*iter)->phChID()->getID()<<endreq;
      (*iter)->calculateTileID(numberOfTileID,phChTileID,m_pGetInfo);
      if( m_verboseDebug)log<<MSG::INFO<<" after tile calculation " 
                            << numberOfTileID<<" "<<endreq;
      if( m_verboseDebug)log<<MSG::INFO<<" tile  " << 
                                phChTileID[0]<< phChTileID[1]<<" "<<endreq;
      
      //
      // loop over possible phchtileID (1 or 2 if double mapping)
      //
      for(int iTile=0;iTile<numberOfTileID;iTile++){
        bool found=false; 
        //
        // loop over already created Digits
        //           
        if(m_verboseDebug){
          log<<MSG::DEBUG<<" Loop on mappings "<<
            iTile<<" "<<numberOfTileID<<endreq;}             
        for(iterDigit=mcDigitContainer.begin(); 
            iterDigit<mcDigitContainer.end()&&!found; iterDigit++){
          MuonTileID tile=(*iterDigit)->key();
          //
          // tile is the key of the existing Digit, phChTileID[] 
          // is the just created ID of the ph.ch.
          //                
               
          if(tile==phChTileID[iTile]){
            if( m_verboseDebug)log<<MSG::INFO<<
                                 " Loop on mappings found already "
                                  <<tile<<" "<<endreq;
            if( m_verboseDebug) log<<MSG::INFO<<"  "<<
                                  (*iterDigit)->DigitInfo().isAlive()
                                        <<" "<<(*iter)->phChInfo().isAlive()
                                   <<endreq;
            found=true;
                 // Digit already exists, update bits and links
            std::vector<MuonHitTraceBack>::iterator iterOnHits;
            std::vector<MCMuonHitHistory>::iterator iterOnHitsInDigit;
            if((*iterDigit)->DigitInfo().isAlive()&&
               (*iter)->phChInfo().isAlive()){
              //both fired
              if((*iterDigit)->firingTime()<(*iter)->firingTime()){
                for(iterOnHits=(*iter)->hitsTraceBack().begin();
                    iterOnHits<(*iter)->hitsTraceBack().end();
                         iterOnHits++){
                  (*iterOnHits).getMCMuonHistory().setFiredFlag(0);			 
                }
              }else{
                (*iterDigit)->setFiringTime((*iter)->firingTime());
                (*iterDigit)->DigitInfo().setNatureHit((*iter)
                                                       ->phChInfo().
                                                       natureOfHit());
                (*iterDigit)->DigitInfo().setBXIDFlagHit((*iter)
                                                              ->phChInfo().
                                                         BX());
                for(iterOnHitsInDigit=(*iterDigit)->HitsHistory().begin();
                    iterOnHitsInDigit<(*iterDigit)->HitsHistory().end();
                    iterOnHitsInDigit++){
                       (*iterOnHitsInDigit).setFiredFlag(0);						 
                }
              }
            }
            
            if((*iterDigit)->DigitInfo().isAlive()&&!((*iter)
                                                           ->phChInfo().
                                                      isAlive())){
              // only one is fired	
              
            }
            if(!((*iterDigit)->DigitInfo().isAlive())&&
               ((*iter)->phChInfo().isAlive())){
              // only one is fired	
              (*iterDigit)->setFiringTime((*iter)->firingTime());
              (*iterDigit)->DigitInfo().setNatureHit((*iter)
                                                     ->phChInfo().
                                                     natureOfHit());
              (*iterDigit)->DigitInfo().setBXIDFlagHit((*iter)
                                                       ->phChInfo().
                                                       BX());  
              (*iterDigit)->DigitInfo().setSecondPart(0);   
              (*iterDigit)->DigitInfo().setAliveDigit(1);
                   
              if( m_verboseDebug)log<<MSG::INFO<<" importante "<<
                                   (*iterDigit)->DigitInfo().isAlive()  
                                    <<endreq;
            }
            if(!((*iterDigit)->DigitInfo().isAlive())&&
               !((*iter)->phChInfo().isAlive())){
              if( m_verboseDebug)log<<MSG::INFO<<
                                   " molto importante "<<
                                   (*iterDigit)->DigitInfo().isAlive()  
                                    <<endreq;                   
              // both not fired
              if((*iterDigit)->DigitInfo().isInDeadTime()||	
                 (*iter)->phChInfo().isInDeadTime()){
                (*iterDigit)->DigitInfo().setDeadtimeDigit(1);
              }					
              if((*iterDigit)->DigitInfo().isDeadForChamberInefficiency()
                 ||	(*iter)->phChInfo().isDeadForChamberInefficiency()){
                (*iterDigit)->DigitInfo().setChamberInefficiencyDigit(1);
              }					
              if((*iterDigit)->DigitInfo().isDeadByTimeJitter() ||	
                 (*iter)->phChInfo().isDeadByTimeJitter() ){
                (*iterDigit)->DigitInfo().setTimeJitteredDigit(1);
              }					
              if((*iterDigit)->DigitInfo().isDeadByTimeAdjustment() 
                 ||	(*iter)->phChInfo().isDeadByTimeAdjustment() ){
                (*iterDigit)->DigitInfo().setTimeAdjDigit(1) ;
              }					
              if((*iterDigit)->DigitInfo().isAliveByTimeAdjustment() 
                 ||	(*iter)->phChInfo().isAliveByTimeAdjustment() ){
                (*iterDigit)->DigitInfo().setAliveTimeAdjDigit(1);
              }					
              if((*iterDigit)->DigitInfo().isDeadByGeometry() 
                 ||	(*iter)->phChInfo().isDeadByGeometry() ){
                (*iterDigit)->DigitInfo().setGeometryInefficiency(1);
              }		
            }
            // add links to the hits 										
            for(iterOnHits=(*iter)->hitsTraceBack().begin();
                     iterOnHits<(*iter)->hitsTraceBack().end();iterOnHits++){
              (*iterDigit)->HitsHistory().push_back((*iterOnHits).
                                                    getMCMuonHistory());
                   (*iterDigit)->addToMCMuonHits((*iterOnHits).getMCMuonHit());
            }                 
          }
        }
        if(!found){
          if( m_verboseDebug)log<<MSG::INFO<<
                               " create new Digit with tile "<<
                               phChTileID[iTile]<<" "<<iTile<<endreq;
          std::vector<MuonHitTraceBack>::iterator iterOnHits;
          MCMuonDigit* newMCDigit=new MCMuonDigit(phChTileID[iTile]);
          for(iterOnHits=(*iter)->hitsTraceBack().begin();
              iterOnHits<(*iter)->hitsTraceBack().end();iterOnHits++){
              (newMCDigit)->HitsHistory().push_back((*iterOnHits).
                                                    getMCMuonHistory());
            
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

StatusCode MuonDigitization::
createRAWFormat(MCMuonDigits& mcDigitContainer, MuonDigits& digitContainer){
	MsgStream log(msgSvc(), name()); 
 	MCMuonDigits::iterator iterMCDigit; 
 	for( iterMCDigit = mcDigitContainer.begin();iterMCDigit < 
         mcDigitContainer.end() ; iterMCDigit ++){
	  if((*iterMCDigit)->DigitInfo().isAlive()){		  
      MuonDigit* muonDigit= new MuonDigit((*iterMCDigit)->key());
			unsigned int time=(unsigned	int)(((*iterMCDigit)->firingTime())
                                       /(m_gate/8.0));
      //			if(time>7)time=7;
			muonDigit->setTimeStamp(time);
			digitContainer.insert(muonDigit);
			log<<MSG::DEBUG<<"new daq word "<<(*iterMCDigit)->key()<<
        " "<<time<<endreq;
 		}
	}
  return StatusCode::SUCCESS;	 
}

StatusCode MuonDigitization::
addElectronicNoise(MuonDigitizationData
                   <MuonPhysicalChannel>& PhysicalChannel){
  
	MsgStream log(msgSvc(), name()); 
	MuonPhysicalChannel* pFound;
  for(int ispill=1;ispill<m_numberOfEventsNeed;ispill++){
    int chamberTillNow=0;
    double shiftOfTOF=-m_BXTime*ispill;
    for(int i=0;i<m_stationNumber;i++){
      for(int k=0;k<m_regionNumber;k++){
        int partitionNumber=i*m_regionNumber+k;
        int chamberInRegion=m_pGetInfo->getChamberPerRegion(partitionNumber);
        //					 int gapFE=m_pGetInfo->getGapPerFE(partitionNumber);
        for(int chamber=0;chamber<chamberInRegion;chamber++){		
          for(int frontEnd=0;frontEnd<(int)(m_pGetInfo->
                                            getGapPerRegion(partitionNumber)/
                                            m_pGetInfo->getGapPerFE
                                            (partitionNumber));frontEnd++){
            for(int readout=0;readout<=m_pGetInfo->
                  getReadoutNumber(partitionNumber);readout++){
              int phChInX=m_pGetInfo->
                getPhChannelNX( readout, partitionNumber);
              int phChInY=m_pGetInfo->
                getPhChannelNY( readout, partitionNumber);	
              int noiseChannels=detectorResponse.getResponse
                (partitionNumber,readout)->
                electronicNoise();
              unsigned int readoutType=  m_pGetInfo->getReadoutType
                (readout,   partitionNumber);
              for(int hitNoise=0;hitNoise<noiseChannels;hitNoise++){
                int chX=(int)(m_flatDist()*phChInX);
                int chY=(int)(m_flatDist()*phChInY);
                if(chX==phChInX)chX=phChInX-1;
                if(chY==phChInY)chY=phChInY-1;
               double time=m_flatDist()*m_BXTime+shiftOfTOF ;
                //											 +tofOfLight;
                MuonPhChID ID;
                ID.setStation(i);
                ID.setRegion(k);
                ID.setQuadrant(m_pGetInfo->getQuadrantChamber
                               (chamber+chamberTillNow));
                ID.setChamber(chamber);
                ID.setPhChIDX(chX);
                ID.setPhChIDY(chY);
                ID.setFrontEnd(frontEnd);
                ID.setReadout(readoutType);					 
                MuonPhysicalChannel* newPhysicalChannel=new 
                  MuonPhysicalChannel(ID.getFETile(),m_gate,m_BXTime);
                MuonHitTraceBack* pointerToHit=new MuonHitTraceBack;
                pointerToHit->setHitArrivalTime(time);							
                pointerToHit->getMCMuonHitOrigin().
                  setHitNature(MuonOriginFlag::ELECTRONICNOISE);
                pointerToHit->getMCMuonHitOrigin().setBX(ispill-1);	
                pointerToHit->getMCMuonHistory().setBXOfHit(ispill-1);
                pointerToHit->getMCMuonHistory().
                  setNatureOfHit(MuonOriginFlag::ELECTRONICNOISE);
                newPhysicalChannel->hitsTraceBack().push_back(*pointerToHit);
                delete pointerToHit;				
                pFound=0;						
                bool already=PhysicalChannel.
                  isObjectIn(partitionNumber,newPhysicalChannel,
                             pFound, ComparePC);
                if(already){											   
                  pFound->addToPC(newPhysicalChannel);
                  delete newPhysicalChannel;
                }
                else{
                  newPhysicalChannel->setResponse
                    (detectorResponse.getResponse
                     (*(newPhysicalChannel->phChID())));
                  //if(!newPhysicalChannel->getResponse()){
                    //log<<MSG::INFO<<"addio p"<<partitionNumber<<endreq;
                    //log<<MSG::INFO<<"addio oip"<<detectorResponse.
                    //  getResponse(*(newPhysicalChannel->phChID()))<<endreq; 
                    //log<<MSG::INFO<<"addio"<<endreq;}
                  PhysicalChannel.addMuonObject(partitionNumber,
                                                newPhysicalChannel);	
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

 
