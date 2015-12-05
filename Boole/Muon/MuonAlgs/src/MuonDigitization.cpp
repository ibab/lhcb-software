#include <algorithm>
#include <vector>
#include <cmath>

#include "GaudiKernel/AlgFactory.h"

#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/DeMuonDetector.h"
#include "DetDesc/IGeometryInfo.h"

#include "MuonDigitization.h"
#include "MuonChamberResponse.h" 
#include "MuonPhPreInput.h"
#include "SortPhChID.h" 
#include "ComparePC.h"

#include <boost/utility/string_ref.hpp>

DECLARE_ALGORITHM_FACTORY( MuonDigitization )

namespace {
       double getGlobalTimeOffset( boost::string_ref rootInTES ) {
           if ( rootInTES.starts_with("Prev") && rootInTES.ends_with("/") ) {
              rootInTES.remove_prefix(4);
              rootInTES.remove_suffix(1);
              return - std::stoi( rootInTES.data() )*25*Gaudi::Units::ns;
           }
           if ( rootInTES.starts_with("Next") && rootInTES.ends_with("/") ) {
              rootInTES.remove_prefix(4);
              rootInTES.remove_suffix(1);
              return std::stoi( rootInTES.data() )*25*Gaudi::Units::ns;
           }
           return 0.;
       }
}

//reserve space for static variable
std::string MuonDigitization::spill[6] = 
{"/LHCBackground","","/Prev","/PrevPrev","/Next","/NextNext"};
const int MuonDigitization::spillTimeOffset[6] =
{0, 0, -1,-2,1,2};
std::string MuonDigitization::numreg[4] = 
{"1","2","3","4"};
std::string MuonDigitization::numsta[5]= 
{"1","2","3","4","5"};
 std::string MuonDigitization::TESPathOfHitsContainer[4]=
{"Hits","ChamberNoiseHits","FlatSpilloverHits",
"BackgroundHits"};
const int MuonDigitization::OriginOfHitsContainer[5]=
{LHCb::MuonOriginFlag::GEANT,LHCb::MuonOriginFlag::CHAMBERNOISE,
 LHCb::MuonOriginFlag::FLATSPILLOVER,LHCb::MuonOriginFlag::BACKGROUND, 
LHCb::MuonOriginFlag::LHCBACKGROUND};

MuonDigitization::MuonDigitization(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  :  GaudiAlgorithm(name, pSvcLocator)
  , m_muonDetector(NULL)
{
  //declareProperty("SpilloverPathsSize", m_numberOfSpilloverEvents = 0) ;
  declareProperty("BXTime" , m_BXTime=25.0) ;
  declareProperty("TimeGate" , m_gate=25.0) ;
  declareProperty("TimeBits" , m_TimeBits=4) ;
  declareProperty("VerboseDebug" , m_verboseDebug=false) ;
  declareProperty("ApplyTimeJitter" , m_applyTimeJitter=true) ;
  declareProperty("ApplyChamberNoise" , m_applyChamberNoise=false) ;
  declareProperty("ApplyElectronicNoise" , m_applyElectronicNoise=true) ;
  declareProperty("ApplyXTalk" , m_applyXTalk=true) ;
  declareProperty("ApplyEfficiency" , m_applyEfficiency=true) ;
  declareProperty("ApplyDeadtime" , m_applyDeadtime=true) ;  
  declareProperty("ApplyDialogDeadtime" , m_applyDialogDeadtime=true) ;
  declareProperty("ApplyTimeAdjustment",m_applyTimeAdjustment=true);
  declareProperty("DialogLength",m_dialogLength=28);
  declareProperty("RegisterPhysicalChannelOutput",
                   m_registerPhysicalChannelOutput=false);
  m_hitNotInGap=0;
}

StatusCode MuonDigitization::initialize()
{  
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by  GaudiAlgorithm	

  m_numberOfEvents = 4+1;
  m_numberOfEventsNeed=5;	


  //  info()<<"Initializing muon digitization"<<endmsg;


  // initialize some basic geometrical information
  MuonBasicGeometry basegeometry(detSvc(),msgSvc()); 
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();
  int i=0;  
  while(i<m_stationNumber){
    numsta[i]=basegeometry.getStationName(i);    
    if(msgLevel(MSG::DEBUG)) debug()<<" station "<<i<<" "<<numsta[i]<<endmsg;
    i++;    
  }
  m_partition=basegeometry.getPartitions();
  sc=m_flatDist.initialize( randSvc(), Rndm::Flat(0.0,1.0));	 
  if(sc.isFailure())warning()<<" error in flat ini"<<endmsg;
  detectorResponse.initialize( toolSvc(),randSvc(), detSvc(), msgSvc());
  if(msgLevel(MSG::DEBUG)) debug()<<" detectorResponseInitialized "<<endmsg;
  m_spill=6;
  m_container=4;
  unsigned int count=1;
  for (i=0;i<4;i++){
    count=count*2;    
  }
  m_timeBin=m_BXTime/(count);  
  m_muonDetector=getDet<DeMuonDetector>(DeMuonLocation::Default);
  return StatusCode::SUCCESS;
}
 
 					
StatusCode MuonDigitization::execute()
{

MsgStream log(msgSvc(), name());
 	StatusCode sc=StatusCode::SUCCESS ;
  
  if(msgLevel(MSG::DEBUG)) 
    debug()<<"starting the Muon Digitization algorithm "<<endmsg;

  //  info()<<"starting the Muon Digitization algorithm "<<endmsg;


  SmartDataPtr<LHCb::MCHits> 
    hitPointer(eventSvc(),LHCb::MCHitLocation::Muon);
  
  
  if(hitPointer!=0){
    LHCb::MCHits::const_iterator i;
    for (i=(hitPointer)->begin();i<(hitPointer)->end();i++){         
      if(m_verboseDebug){
        info()<<"muon x , y, z , exit  "<< (*i)->exit().x() <<" " <<
          (*i)->exit().y() << "  " <<                          
          (*i)->exit().z() << endmsg ;																		
        if(msgLevel(MSG::DEBUG)) {          
          debug()<<"muon x , y, z entry ,  "<< (*i)->entry().x() <<" " <<
            (*i)->entry().y() << "  " <<(*i)->entry().z() << endmsg ;   
          debug()<<"time of flight ,  "<< (*i)->time()<< endmsg;
          int det=(*i)->sensDetID(); 
          debug()<<" chamber and gap ID	"<<
            m_muonDetector->chamberID(det)<<" "<<
            m_muonDetector->gapID(det)<<endmsg;
        }
        const LHCb::MCParticle* origparticle=(*i)->mcParticle();
        if(origparticle)  {
            if(msgLevel(MSG::DEBUG)) 
              debug()<<"Particle from which it originates (PDG code)"<<
                origparticle->particleID().abspid()<<endmsg;
        }
        else{
          warning()<<
            "Particle from which it originates is not defined "<< endmsg;
        }
      }	
    }
  }	
  else{
    Error("unable to retrieve the hit container").ignore(); 
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
  //  if(1){	
    for(int i=0; i<m_partition; i++){
      MuonPhysicalChannels::const_iterator iter=
        PhyChaOutput.getPartition(i)->begin();
      for(iter=PhyChaOutput.getPartition(i)->begin();iter<
            PhyChaOutput.getPartition(i)->end();iter++){		    
	//      info()<<"Hits after last processing (phys effects) partition: "<<i
	//   <<" channel ID: "<<*((*iter)->phChID())<<endmsg;
      /*
        info()<<"#FE ID "<<(*iter)->phChID()->getID()<<endmsg;
        info()<<"#Station "<<(*iter)->phChID()->getStation()<<endmsg;
        info()<<"#Region "<<(*iter)->phChID()->getRegion()<<endmsg;
        info()<<"#Quadrant "<<(*iter)->phChID()->getQuadrant()<<endmsg;
        info()<<"#Chamber "<<(*iter)->phChID()->getChamber()<<endmsg;
        info()<<"# ch X "<<(*iter)->phChID()->getPhChIDX()<<endmsg;
        info()<<"# ch Y "<<(*iter)->phChID()->getPhChIDY()<<endmsg;
        info()<<"#frontend "<<(*iter)->phChID()->getFrontEnd()<<endmsg;
      */
      }
    }    
  }
  
  if(m_registerPhysicalChannelOutput){
    MuonBasicGeometry basegeometry(detSvc(),msgSvc()); 
    PhyChaOutput.registerPartitions(&basegeometry);
  }  
 
  MuonDigitizationData<MuonPhysicalChannelOutput> PhysicalChannelOutput("MULC",&log,eventSvc(),"MULC") ;
  sc=fillPhysicalChannel(PhyChaOutput,PhysicalChannelOutput);
  if(sc.isFailure())return StatusCode::FAILURE;
		
  if(m_verboseDebug){	
    //  if(1){
    for(int i=0; i<m_partition; i++){
      //      info()<<"Creating MC digits for partition: "<<i<<endmsg;
      MuonPhysicalChannelOutputs::const_iterator iter=
        PhysicalChannelOutput.getPartition(i)->begin();
      for(iter=PhysicalChannelOutput.getPartition(i)->begin();iter<
            PhysicalChannelOutput.getPartition(i)->end();iter++){		    
        info()<<"FE ID "<<(*iter)->phChID()->getID()<<endmsg;
        info()<<"Station "<<(*iter)->phChID()->getStation()<<endmsg;
        info()<<"Region "<<(*iter)->phChID()->getRegion()<<endmsg;
        //info()<<"Quadrant "<<(*iter)->phChID()->getQuadrant()<<endmsg;
        info()<<"Chamber "<<(*iter)->phChID()->getChamber()<<endmsg;
        info()<<" ch X "<<(*iter)->phChID()->getPhChIDX()<<endmsg;
        info()<<" ch Y "<<(*iter)->phChID()->getPhChIDY()<<endmsg;
        info()<<" frontend "<<(*iter)->phChID()->getFrontEnd()<<endmsg;
        info()<<" fired "<<(*iter)->phChInfo().isAlive()<<endmsg;
        info()<<" nature "<<(*iter)->phChInfo().natureOfHit()<<endmsg;
        std::vector<MuonHitTraceBack> vector_traceBack=(*iter)->
          hitsTraceBack();
        std::vector<MuonHitTraceBack>::iterator iterTraceBack=
          (vector_traceBack).begin(); 
        for(iterTraceBack=(vector_traceBack).begin();iterTraceBack<
              (vector_traceBack).end();iterTraceBack++){
          info()<<"hit time "<<(*iterTraceBack).hitArrivalTime() 
		<<endmsg; 
        }
      }
    }
  }
  
  MuonDigitizationData<MuonCardiacChannelOutput> 
    CardiacChannelOutput("MUCC",&log,eventSvc(),"MUCC") ;
  //debug()<<"pappa hk "<<endmsg; 
  sc=fillCardiacChannel(PhysicalChannelOutput,CardiacChannelOutput);
  if(sc.isFailure())return StatusCode::FAILURE;	 	
  LHCb::MCMuonDigits* mcDigitContainer= new LHCb::MCMuonDigits;
  //bool test=true;
  // debug()<<"pappa rrr"<<endmsg; 
  if(m_applyDialogDeadtime){
    
    
    
    sc=createLogicalChannel(CardiacChannelOutput, *mcDigitContainer);
    
    
  }
  else{
    sc=createLogicalChannel(PhysicalChannelOutput, *mcDigitContainer);
  }
  
  if(sc.isFailure())return StatusCode::FAILURE;	 			
  put( mcDigitContainer, LHCb::MCMuonDigitLocation::MCMuonDigit );

  LHCb::MuonDigits* digitContainer= new LHCb::MuonDigits;	

  sc=createRAWFormat(*mcDigitContainer, *digitContainer);
  if(sc.isFailure())return StatusCode::FAILURE;	 				
  put( digitContainer, LHCb::MuonDigitLocation::MuonDigit );
  
  if(msgLevel(MSG::DEBUG)) debug()<<"End of the Muon Digitization"<<endmsg;
  return StatusCode::SUCCESS;
  
}

StatusCode MuonDigitization::finalize()
{
  // Release the tools
  //if( m_pMuonTileXYZ ) toolSvc()->releaseTool( m_pMuonTileXYZ );
  //if( m_pGetInfo ) toolSvc()->releaseTool( m_pGetInfo );
  //info()<<" the hit which were not found inside a gap were "<<m_hitNotInGap<<endmsg;;

  detectorResponse.finalize();
  
  return GaudiAlgorithm::finalize();
}

StatusCode MuonDigitization::addChamberNoise(){

  MsgStream log(msgSvc(), name()); 
  int container=1;
  for (int ispill=1;ispill<=m_numberOfEvents;ispill++){
		//double shiftOfTOF=-m_BXTime*ispill;
    ObjectVector<LHCb::MCHit>* hitsContainer = new 
      ObjectVector<LHCb::MCHit>();
    std::string path="/Event"+spill[ispill]+"/MC/Muon/"+"/"
      +TESPathOfHitsContainer[container];
    for(int k=0;k<m_stationNumber;k++){ 
      for(int s=0;s<m_regionNumber;s++){
        
        int partitionNumber=k*m_regionNumber+s;
        int chamberInRegion=m_muonDetector->chamberInRegion(k,s);
        int gapPerChamber=m_muonDetector->gapsInRegion(k,s);
        for (int chamber=0;chamber<chamberInRegion;chamber++){			   
          int numberOfNoiseHit=(detectorResponse.getChamberResponse
                                (partitionNumber))->extractNoise();          
          // the readout number is essentially meaningless... 
          //it is chamber noise.....					 
          if(numberOfNoiseHit==0)continue;
          DeMuonChamber* p_chamber=m_muonDetector->getChmbPtr(k, s,chamber);
          for (int hit=1;hit<=numberOfNoiseHit;hit++){
            //define position of hit 
            //first gap number
            int gap=(int)((m_flatDist()*gapPerChamber)); 
            int gapMax=m_muonDetector->gapsInRegion(k,s);
            if(gap>gapMax-1)gap=gapMax-1;
            IPVolume* gasGap=(p_chamber)->getGasGap(gap);
            IPVolume* gasGapLayer=(p_chamber)->getGasGapLayer(gap);


            double zhalfgap= 1.0;
            
            //  if(p_Gap!=NULL){
            const ISolid* gasGapSolid=gasGap->lvolume()
              ->solid();
            const SolidBox* gapBox = 
                    dynamic_cast<const SolidBox *>(gasGapSolid);
            zhalfgap= gapBox->zHalfLength() ; 
            //}
            
            //then x&y	
            double x = m_flatDist()* 
              m_muonDetector->getSensAreaX(k,s);
            double y = m_flatDist()*  
              m_muonDetector->getSensAreaY(k,s);
            double time = m_flatDist()*m_BXTime;	
            LHCb::MCHit* pHit = new LHCb::MCHit();
            //traslate xyz e local to global..
            Gaudi::XYZPoint pointInLayer1=gasGap
              ->toMother(Gaudi::XYZPoint(x,y,-1*zhalfgap+0.1));
            Gaudi::XYZPoint pointInLayer2=gasGap
              ->toMother(Gaudi::XYZPoint(x,y,zhalfgap-0.1));

            Gaudi::XYZPoint pointInCh1=gasGapLayer
              ->toMother(pointInLayer1);
            Gaudi::XYZPoint pointInCh2=gasGapLayer
              ->toMother(pointInLayer2);
            Gaudi::XYZPoint point1=(p_chamber->geometry())->
              toGlobal(pointInCh1);
            Gaudi::XYZPoint point2=(p_chamber->geometry())->
              toGlobal(pointInCh2);
            double z=(point1.z()+point2.z())/2.0;
            
            //              p_chamber->
            
            pHit->setEntry(point1);
            pHit->setDisplacement(point2-point1);
            double tofOfLight=(sqrt(x*x+ y*y+z*z))/300.0;
            pHit->setTime(time+tofOfLight);
            Gaudi::XYZPoint hitMid=
              Gaudi::XYZPoint((point1.x()+point2.x())/2.0F,
                              (point1.y()+point2.y())/2.0F,
                              (point1.z()+point2.z())/2.0F);
            
            int sen=m_muonDetector->sensitiveVolumeID(hitMid);
            pHit->setSensDetID(sen);
            hitsContainer->push_back(pHit);
            if(m_verboseDebug){	
              info()<<"adding chamber noise hit "<<
                ispill<<" "<<k<<" "<<s<<numberOfNoiseHit<<endmsg;
            }
            //        continue;              
            //}            
                //countGap++;            
                // }
          }
          
        
          
          
          if(m_verboseDebug){	
            if(numberOfNoiseHit>0)
              info()<<"adding chamber noise hit "<<ispill<<" "<<
                k<<" "<<s<<" chamber "<<chamber <<" "<<
                numberOfNoiseHit<<endmsg;
          }	
          
        }         
      }
    }
    eventSvc()->registerObject(path,hitsContainer);
    
  }
  
  return StatusCode::SUCCESS;
}





StatusCode
MuonDigitization::createInput(
                              MuonDigitizationData<MuonPhyChannelInput>& 
                              PhyChaInput)
{
  MsgStream log(msgSvc(), name()); 
  
  //loop over the containers
  std::vector<MuonPhPreInput> keepTemporary[20] ;	

  double globalTimeOffset = getGlobalTimeOffset( rootInTES() );
  
  //  info()<<"quanti eventi "<<m_numberOfEvents<<endmsg;
  
  for (int ispill=0; ispill<=m_numberOfEvents;ispill++){ // loop over event types
    long spillTime=0;     
    spillTime=spillTimeOffset[ispill]*m_BXTime;
    
    for(int container=0; container<m_container;container++){// loop over containers
      std::string path="/Event"+spill[ispill]+"/MC/Muon/"+
        TESPathOfHitsContainer[container];
      if(m_verboseDebug) {info()<<"createInput: hit container path "<<container
	  //      if(1) {info()<<"createInput: hit container path "<<container
				<<" "<<path<<endmsg;}
      
      SmartDataPtr<LHCb::MCHits> hitPointer(eventSvc(),path);
      LHCb::MCHits::const_iterator iter;	 
      if(hitPointer!=0){

        for (iter=(hitPointer)->begin();iter<(hitPointer)->end();iter++){ // loop over hits
          std::vector< std::pair<MuonFrontEndID, std::vector<float> > > 
            listph;
      	  int det=(*iter)-> sensDetID();
          if(det<0)continue;
          
          unsigned int hitStation = m_muonDetector->stationID(det);
          unsigned int hitRegion  = m_muonDetector->regionID(det);          
          unsigned int hitChamber = m_muonDetector->chamberID(det);
          unsigned int hitGap     = m_muonDetector->gapID(det);
          unsigned int hitQuarter = m_muonDetector->quadrantID(det);

	  //          if(m_verboseDebug) verbose()<<" ga hit "<<hitStation<<" "<<hitRegion
	  //                       <<" "<<hitChamber<<" "<<hitGap<<" "<<hitQuarter<<endmsg;
	  /*
          info()<<"CreateInput(): gap hit, x,y,z,s,r,c,g,q "
		<<(*iter)->entry().x()<<" "
		<<(*iter)->entry().y()<<" "
		<<(*iter)->entry().z()<<" "
		<<hitStation<<" "
		<<hitRegion<<" "
		<<hitChamber<<" "
		<<hitGap<<" "
		<<hitQuarter<<endmsg;
	  */

          listph= m_muonDetector->listOfPhysChannels((*iter)->entry(),
                                                     (*iter)->exit(), 
                                                     hitRegion,hitChamber);
          if(listph.size()==0) m_hitNotInGap++;
          std::vector< std::pair<MuonFrontEndID, std::vector<float> > >::
            iterator itPh;
          
          for(itPh=listph.begin();itPh<listph.end();itPh++){
            MuonFrontEndID fe=(*itPh).first;
            std::vector<float> dist=(*itPh).second;    
            double distanceFromBoundary[4];
            distanceFromBoundary[0]=dist[0];
            distanceFromBoundary[1]=dist[1];
            distanceFromBoundary[2]=dist[2];
            distanceFromBoundary[3]=dist[3];
            
            MuonPhPreInput* inputPointer = new  MuonPhPreInput;           
	    //            if(m_verboseDebug) debug()<<" adding pch "<<hitStation<<" "<<
	    //            info()<<"adding pch: s/r/c/q readout "<<hitStation<<" "<<
	    //hitRegion<<" "<<hitChamber<<" "<<hitQuarter<<"  "<<fe.getReadout()
	    //  <<" ID "<<fe<<endmsg;
            
            inputPointer->phChID()->setStation(hitStation);
            inputPointer->phChID()->setRegion(hitRegion);
            inputPointer->phChID()->setQuadrant(hitQuarter);
            inputPointer->phChID()->setChamber(hitChamber);
            
            inputPointer->phChID()->setPhChIDX(fe.getFEIDX());
            inputPointer->phChID()->setPhChIDY(fe.getFEIDY());		
            inputPointer->phChID()->setFrontEnd(fe.getLayer());
            inputPointer->phChID()->setReadout(fe.getReadout());
            inputPointer->phChID()->setGap(hitGap);
            inputPointer->getHitTraceBack()->setMCHit(*iter); 
            
            //correct for the  tof .... i.e. subtract the tof that 
            //a lightparticle impacting the center of the pc. has....
            
            double xcenter, ycenter,zcenter;
            
            
            StatusCode sc=m_muonDetector->
                          getPCCenter(fe,hitChamber,hitStation, hitRegion ,
                                        xcenter, ycenter,zcenter);
            if(sc.isFailure())warning()<<" getpc ch error"<<endmsg;
            double tofOfLight=sqrt(xcenter*xcenter+ycenter*ycenter+
                                   zcenter*zcenter)/300.0;
            //info()<<" tof "<<tofOfLight<<" "<<hitStation<<" "<<hitRegion<<" "<<zcenter<<endmsg;
            inputPointer->getHitTraceBack()
              ->setHitArrivalTime((*iter)->time()+globalTimeOffset+
                                  +spillTime-tofOfLight+0.5);
	    //
            inputPointer->getHitTraceBack()
              ->setCordinate(distanceFromBoundary);
	    //

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
            
	    // ma non e' gia' fatto sopra ? in questo modo sovrascrive...     
            inputPointer->getHitTraceBack()->setCordinate
              (distanceFromBoundary);
	    //


	    if(m_verboseDebug){	
	    //            if(1){	
	      int code=0;
	      if((*iter)->mcParticle())
		code=(*iter)->mcParticle()->particleID().abspid();

	      info()<<"hit processing "<<hitStation<<" "
		    <<hitRegion<<" "
		    <<hitQuarter<<" "
		    <<hitChamber<<" "
		    <<hitGap<<" "
		    <<fe.getLayer()<<" "
		    <<fe.getReadout()<<" "
		    <<tofOfLight<<" "
		    <<OriginOfHitsContainer[container]<<" "
		    <<ispill<<" "
		    <<code<<endmsg;		
	      info()<<" ph ch ID "
		    <<*(inputPointer->phChID())
		    <<" id "<<inputPointer->phChID()->getID()<<endmsg;
            }	 	

            keepTemporary[hitStation*4+hitRegion].push_back(*inputPointer);
            delete inputPointer;

          } // end of loop over phisical channels
        }// end of loop over MC hits
      } // endif MC hit exists
    } // end of loop over containers
  } // end of loop over event types
  
  for(int iterRegion=0;iterRegion<m_partition;iterRegion++){// loop over partitions
    //    info()<<"Sorted hits for partition: "<<iterRegion<<endmsg;

    // sort the temporary list of physical channels
    std::stable_sort(keepTemporary[iterRegion].begin(),
                     keepTemporary[iterRegion].end(),SortPhChID());

    std::vector<MuonPhPreInput>::reverse_iterator iterPost;    

    for(iterPost=keepTemporary[iterRegion].rbegin();
        iterPost<keepTemporary[iterRegion].rend();
        ++iterPost){
      if(m_verboseDebug){
      //      if(1){
	info()<<"Sorted hits s/r/q/c/idx/idy/FE/RO/g "
	      <<*((iterPost)->phChID())<<endmsg;
      }
      MuonPhyChannelInput* phChPointer = 
        new MuonPhyChannelInput((iterPost)->phChID()->getID(),*((iterPost)->getHitTraceBack())) ;	

      StatusCode asc=PhyChaInput.addMuonObject(iterRegion,phChPointer);

      //      if(asc.isFailure())debug()<<" not able to add requested obj "<<endmsg;
      if(asc.isFailure())info()<<" not able to add requested obj "<<endmsg;
      keepTemporary[iterRegion].pop_back(); 	
    } 	  
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
      MuonPhyChannelInputs::const_iterator inputIterStart=PhyChaInput.getPartition(i)->begin();

      unsigned int prevFE=(*inputIterStart)->phChID()->getFETile();

      //      info()<<"First tile for partition: "<<i<<" "<<*((*inputIterStart)->phChID())<<endmsg;


      unsigned int lastFE;

      MuonHitTraceBack* pointerToHitTraceBack=(*inputIterStart)->getHitTraceBack();
      MuonPhysicalChannel* outputPointer = new MuonPhysicalChannel(prevFE,m_gate,m_BXTime);
      
      outputPointer->setResponse
        (detectorResponse.getResponse(*((*inputIterStart)->phChID())));

      /*

      if(!detectorResponse.getResponse(*((*inputIterStart)->phChID()))){
        err()<<"unable to retrieve the response of  ph. channel"
           <<endmsg;
      }

      */

      outputPointer->hitsTraceBack().push_back(*pointerToHitTraceBack);
      StatusCode asc=PhysicalChannel.addMuonObject(i,outputPointer);
      if(asc.isFailure()){
        warning()<<" error in inserting obj "<<endmsg;
      }
      for (inputIter=++inputIterStart;inputIter
             <PhyChaInput.getPartition(i)->end();inputIter++){
        lastFE=(*inputIter)->phChID()->getFETile();

	//	info()<<"Next tile for partition: "<<i<<" "<<*((*inputIter)->phChID())<<endmsg;

        pointerToHitTraceBack=(*inputIter)->getHitTraceBack();
        if(lastFE==prevFE){ // phys channels with same FE are not recreated
          outputPointer->hitsTraceBack().push_back(*pointerToHitTraceBack);
        }else{
          outputPointer = new MuonPhysicalChannel(lastFE,m_gate,m_BXTime);
          outputPointer->setResponse
            (detectorResponse.getResponse(*((*inputIter)->phChID())));
        
          
          outputPointer->hitsTraceBack().push_back(*pointerToHitTraceBack) ;
          prevFE=lastFE;
          StatusCode _asc=PhysicalChannel.addMuonObject(i,outputPointer);
          if(_asc.isFailure() && msgLevel(MSG::DEBUG))
            debug()<<"error in adding obj "<<endmsg;        
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
	if(m_verboseDebug)info()<<objToAdd->phChID()->
	//        if(1)info()<<"FillPhysChan: object to add: "<<objToAdd->phChID()->
                            getID()<<" "<<*(objToAdd->phChID())<<endmsg;
        for(iterInHits=objToAdd->hitsTraceBack().begin();
            iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){
          if(m_verboseDebug)info()<<" geo acce "<<
                              (*iterInHits).getMCMuonHistory().
                              isHitOutGeoAccemtance()<<
                              " cha ineff "<<
                              (*iterInHits).getMCMuonHistory().
                                isKilledByChamberInefficiency()<<
                              " in deadtime "<<
                              (*iterInHits).getMCMuonHistory().
                              isHitInDeadtime()<<
                              " time of firing "<< 
                              (*iterInHits).hitArrivalTime()<<endmsg;				 
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
        if(m_verboseDebug)info()<<" fired "<<fired<<endmsg;
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
        if(!fired &&! interestingHit){
          iterInHits=objToAdd->hitsTraceBack().begin();
          objToAdd->phChInfo().setAliveDigit(0);
          objToAdd->phChInfo().setBXIDFlagHit((*iterInHits).
                                              getMCMuonHistory().BX());
          objToAdd->phChInfo().setNatureHit((*iterInHits).
                                                  getMCMuonHitOrigin().
                                                  getNature());
          // objToAdd->phChInfo().setSecondPart((*iterInHits).secondPart());
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


   


     
        // debug printout
        
        bool muon=false;
        for(iterInHits=objToAdd->hitsTraceBack().begin();
            iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){				
          // search the muon first...
          if((iterInHits)->getMCHit()){
            const LHCb::MCParticle* particle=
              (iterInHits)->getMCHit()->mcParticle();
            if(particle){
              int pid= particle->particleID().abspid();
              if(pid==13||pid==-13){
                if(msgLevel(MSG::DEBUG)) debug()<<"moun hit   time   ??????"<<
                  (iterInHits)-> hitArrivalTime()	<<endmsg;	
                muon=true;
              }
            }
          }							 
        }
        if(muon){
          if(m_verboseDebug)
            info()<<"**** start new pc****   station  and region "
                  <<i <<" fired "<<fired<<endmsg;	
          for(iterInHits=objToAdd->hitsTraceBack().begin();
              iterInHits<objToAdd->hitsTraceBack().end();iterInHits++){	
            
            if(m_verboseDebug)info()<<"time"<<(iterInHits)-> 
                                hitArrivalTime()	<<" tile "<<
                                objToAdd->phChID()->getFETile()<<endmsg;	
            if(m_verboseDebug)info()<<	" deadtime "<<
                                (*iterInHits).getMCMuonHistory().
                                isHitInDeadtime()<<" time jitter "<<
                                (*iterInHits).getMCMuonHistory().
                                hasTimeJittered() <<" efficiency  "<<
                                (*iterInHits).getMCMuonHistory().
                                isKilledByChamberInefficiency()<<endmsg;
          }					  
        }
        // if(fired||interestingHit){  
        objToAdd->fillTimeList();
        StatusCode asc=PhysicalChannelOutput.addMuonObject(i,objToAdd);
        if(asc.isFailure() && msgLevel(MSG::DEBUG))
          debug()<<" problem in adding obj "<<endmsg;
        
        //}
        //else {
        //  delete 	objToAdd;
        //}        
      }
    }
  }	
  return StatusCode::SUCCESS;					   
}


StatusCode  MuonDigitization::
fillCardiacChannel(MuonDigitizationData<MuonPhysicalChannelOutput>& 
                    PhysicalChannelOutput,
                    MuonDigitizationData<MuonCardiacChannelOutput>& 
                    CardiacChannelOutput){
  MuonPhysicalChannelOutputs::iterator iterInput;
  MuonCardiacChannelOutputs::iterator iterOutput;
  //		  MuonPhysicalChannelOutputs::iterator iterOutput;
  //  MsgStream log(msgSvc(), name()); 					 

  
  for (int i=0; i<m_partition;i++){
    if(!PhysicalChannelOutput.isEmpty(i)){				
      for(iterInput=PhysicalChannelOutput.getPartition(i)->begin();
          iterInput<PhysicalChannelOutput.getPartition(i)->end();iterInput++){


        LHCb::MuonTileID phChTileID[2];
        int numberOfTileID;

        (*iterInput)->calculateTileID(numberOfTileID,phChTileID,m_muonDetector);

        //        info()<<"ii "<<i<<" "<< cardiacChTileID<<endmsg;

        for(int iTile=0;iTile<numberOfTileID;iTile++){

          bool alreadyExist=false;
          MuonPhysicalChannelOutput* jout=
            static_cast< MuonPhysicalChannelOutput* >(*(iterInput));
          //info()<<((*iterInput)->phChInfo()).isAlive()<<endmsg;
          
          if(!CardiacChannelOutput.isEmpty(i)){
            for(iterOutput=CardiacChannelOutput.getPartition(i)->begin();
                iterOutput<CardiacChannelOutput.getPartition(i)->end();
                iterOutput++){
              if(((*iterOutput)->chID())==phChTileID[iTile]){
                (*iterOutput)->addPhyChannel(jout);
                alreadyExist=true;
              }
              
            }
            
          }
          //info()<<" alreadyExist= "<<alreadyExist<<endmsg;
        
        
          if(!alreadyExist){
            
            MuonCardiacChannelOutput* objToAdd=new 
              MuonCardiacChannelOutput(phChTileID[iTile]);
            objToAdd->addPhyChannel(jout);
            StatusCode asc=CardiacChannelOutput.addMuonObject(i,objToAdd);
            if(asc.isFailure() && msgLevel(MSG::DEBUG))
              debug()<<"unable to add chardiac channel "<<endmsg;
          }
          
        }
        
      }
    }
    
    
    
    if(!CardiacChannelOutput.isEmpty(i)){				
      for(iterOutput=CardiacChannelOutput.getPartition(i)->begin();
          iterOutput<CardiacChannelOutput.getPartition(i)->end();iterOutput++){
        (*iterOutput)-> processForDeadTime(m_dialogLength, (float)m_gate);
        //        (*iterOutput)-> setFiringTime(); 
        //  info()<<" dopo il deadtime "<< (*iterOutput)->chInfo().isAlive()<<endmsg;
        
      }
    }
    
  }
  
  
  //info()<<" pre new ch  size "<<endmsg;
  
 
  return StatusCode::SUCCESS;
}


 



StatusCode MuonDigitization::
applyPhysicalEffects(MuonDigitizationData<MuonPhysicalChannel>&
                     PhysicalChannel){
  MsgStream log(msgSvc(), name()); 	
  
  //loop over the 20 containers 
  
  for (int i=0; i<m_partition;i++){	
    //    log<<MSG::INFO<<" part "<<i<<endmsg;
    int station=i/4;
    int region=i%4;	 
    if(!PhysicalChannel.isEmpty(i)){
      //log<<MSG::INFO<<" non empty "<<i<<endmsg;
      
      std::vector<MuonPhysicalChannel*>  XTalkPhysicalChannel;
      std::vector<MuonPhysicalChannel*>::iterator iterOnSTD;
      MuonPhysicalChannels::iterator iter ;
      std::vector<MuonPhysicalChannel*> channelsDueToXTalk;	   
      std::vector<MuonPhysicalChannel*>::iterator iterXTalk;
      MuonPhysicalChannel* pFound;
      int phChInX[2] ; phChInX[0]=-1; phChInX[1]=-1;
      int phChInY[2] ; phChInY[0]=-1; phChInY[1]=-1;
      for (int iloop=0; iloop<m_muonDetector->readoutInRegion(station,region);
           iloop++){
	phChInX[(int)m_muonDetector->getReadoutType(iloop,station,region)]	=
          m_muonDetector->getPhChannelNX( iloop,station,region);        
	phChInY[m_muonDetector->getReadoutType(iloop,station,region)]	=
          m_muonDetector->getPhChannelNY( iloop,station,region);
      }
      //log<<MSG::INFO<<" start iter "<<endmsg;
      
      for(iter=PhysicalChannel.getPartition(i)->begin();
	  iter<PhysicalChannel.getPartition(i)->end();iter++){
        // apply per pc the time jitter on each hit	
        //  log<<MSG::INFO<<" pre jitter "<<endmsg;
        if( m_applyTimeJitter)(*iter)->applyTimeJitter();
        //log<<MSG::INFO<<" jitter "<<endmsg;
        // apply per pc the geometry inefficiency 	on each hit			
        if(m_applyEfficiency)(*iter)->applyGeoInefficiency();					 
        //log<<MSG::INFO<<" eff "<<endmsg;
        
        // apply per pc the chamber inefficiency 	on each hit				
        if(m_applyEfficiency)(*iter)->applyChamberInefficiency();	
        //log<<MSG::INFO<<" effdue "<<endmsg;
        
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
            if(msgLevel(MSG::DEBUG)) debug()<<"xtalk hit test  molto dopo "<<
              (*iter)<<" "<<*iterXTalk<<" "<<pFound<<endmsg;
            if(pFound==(*iter))err()<<"alessia gran casino"<<endmsg;
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
        }else{
          StatusCode asc=PhysicalChannel.addMuonObject(i,*iterOnSTD);
          if(asc.isFailure() && msgLevel(MSG::DEBUG))
            debug()<<" not able to add xt "<<endmsg;
          
          std::vector<MuonHitTraceBack>::iterator hji ;
          if(i==0){
            for(hji=(*iterOnSTD)->hitsTraceBack().begin();
                hji<(*iterOnSTD)->hitsTraceBack().end();hji++){
            }
          }
        }        
      } 
      // start deadtime
      //log<<MSG::INFO<<" deadtime "<<endmsg;
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
        //log<<MSG::INFO<<" deadtime 2"<<endmsg;
        
        //apply time adjustment	
        if(m_applyTimeAdjustment)(*iter)->applyTimeAdjustment();	
        //apply deadtime
        if(m_applyDeadtime)(*iter)-> applyDeadtime(m_numberOfEventsNeed) ;
        //log<<MSG::INFO<<" deadtime 3"<<endmsg;
      }	
      //end deadtime			 			 			  									 			
    }				
  }
  
  //log<<MSG::INFO<<" end of all "<<endmsg;
  
  return StatusCode::SUCCESS ;	
}






StatusCode MuonDigitization::
createLogicalChannel(MuonDigitizationData<MuonPhysicalChannelOutput>&
                     PhyChaOutput, LHCb::MCMuonDigits& mcDigitContainer){
	MsgStream log(msgSvc(), name()); 
  //  int flag=0;
  int countDigits=0;
  for(int i=0; i<m_partition; i++){

    MuonPhysicalChannelOutputs::const_iterator iter = PhyChaOutput.getPartition(i)->begin();
    LHCb::MCMuonDigits::iterator iterDigit;        

    for(iter=PhyChaOutput.getPartition(i)->begin();
	iter != PhyChaOutput.getPartition(i)->end();iter++){	
      if(m_verboseDebug){
      //      if(1){
        if(msgLevel(MSG::DEBUG)) 
          debug()<<"Create logical channel from FE ID "<<*((*iter)->phChID())<<endmsg;
      }
      LHCb::MuonTileID phChTileID[2];
      int numberOfTileID;  

      (*iter)->calculateTileID(numberOfTileID,phChTileID,m_muonDetector);

      if( m_verboseDebug)info()<<" after tile calculation " 
			       << numberOfTileID<<" "<<endmsg;
      if( m_verboseDebug)info()<<" tile  " << 
	phChTileID[0]<< phChTileID[1]<<" "<<endmsg;
      
      //
      // loop over possible phchtileID (1 or 2 if double mapping)
      //
      for(int iTile=0;iTile<numberOfTileID;iTile++){
        bool found=false; 
        //
        // loop over already created Digits
        //           
        if(m_verboseDebug){
          if(msgLevel(MSG::DEBUG)) debug()<<" Loop on mappings "<<
            iTile<<" "<<numberOfTileID<<endmsg;}             
        for(iterDigit=mcDigitContainer.begin(); 
            iterDigit<mcDigitContainer.end()&&!found; iterDigit++){
          LHCb::MuonTileID tile=(*iterDigit)->key();
          //
          // tile is the key of the existing Digit, phChTileID[] 
          // is the just created ID of the ph.ch.
          //                
	  
          if(tile==phChTileID[iTile]){
            if( m_verboseDebug)info()<<
	      " Loop on mappings found already "
				     <<tile<<" "<<endmsg;
            if( m_verboseDebug) info()<<"  "<<
	      (*iterDigit)->DigitInfo().isAlive()
				      <<" "<<(*iter)->phChInfo().isAlive()
				      <<endmsg;
            found=true;
	    // Digit already exists, update bits and links
            std::vector<MuonHitTraceBack>::iterator iterOnHits;
            std::vector<LHCb::MCMuonHitHistory>::iterator iterOnHitsInDigit;
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
	      
              if( m_verboseDebug)info()<<" importante "<<
		(*iterDigit)->DigitInfo().isAlive()  
				       <<endmsg;
            }
            if(!((*iterDigit)->DigitInfo().isAlive())&&
               !((*iter)->phChInfo().isAlive())){
              if( m_verboseDebug)info()<<
		" molto importante "<<
		(*iterDigit)->DigitInfo().isAlive()  
                                    <<endmsg;                   
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
	      (*iterDigit)->addToMCHits((*iterOnHits).getMCHit());
            }                 
          }
        }
        if(!found){
          if( m_verboseDebug)info()<<
	    " create new Digit with tile "<<
	    phChTileID[iTile]<<" "<<iTile<<endmsg;
          std::vector<MuonHitTraceBack>::iterator iterOnHits;
          LHCb::MCMuonDigit* newMCDigit=new 
            LHCb::MCMuonDigit(phChTileID[iTile]);
          for(iterOnHits=(*iter)->hitsTraceBack().begin();
              iterOnHits<(*iter)->hitsTraceBack().end();iterOnHits++){
	    (newMCDigit)->HitsHistory().push_back((*iterOnHits).
						  getMCMuonHistory());
            
            newMCDigit->addToMCHits((*iterOnHits).getMCHit());
          }
          newMCDigit->setDigitInfo((*iter)->phChInfo());
          newMCDigit->setFiringTime((*iter)->firingTime());
          
          mcDigitContainer.insert(newMCDigit); 
          ++countDigits;
        }
      }
    }
  }
  if(msgLevel(MSG::DEBUG)) debug()<<" MC Digits created "<<countDigits<<endmsg; 
  return StatusCode::SUCCESS;
}


StatusCode MuonDigitization::
createLogicalChannel(MuonDigitizationData<MuonCardiacChannelOutput>&
                     PhyChaOutput, LHCb::MCMuonDigits& mcDigitContainer){
	MsgStream log(msgSvc(), name()); 
  
  int countDigits=0;
  for(int i=0; i<m_partition; i++){
    MuonCardiacChannelOutputs::const_iterator iter =
      PhyChaOutput.getPartition(i)->begin();
    LHCb::MCMuonDigits::iterator iterDigit;        
    for(iter=PhyChaOutput.getPartition(i)->begin();iter<
          PhyChaOutput.getPartition(i)->end();iter++){	
      LHCb::MuonTileID phChTileID=(*iter)->chID();

      std::vector<MuonHitTraceBack*>::iterator iterOnHitsCardiac;
      LHCb::MCMuonDigit* newMCDigit=new 
        LHCb::MCMuonDigit(phChTileID);
      std::vector<MuonHitTraceBack*> pippo;
      pippo=(*iter)->hitsTraceBack();
      
      for(iterOnHitsCardiac=pippo.begin();
          iterOnHitsCardiac!=pippo.end();
          iterOnHitsCardiac++){ 
        // info()<<"a new  hit to add "<<(*iter)->hitsTraceBack().size()<<endmsg;
        (newMCDigit)->HitsHistory().push_back((*iterOnHitsCardiac)->
                                                  getMCMuonHistory());
        // info()<<"a new  hit to add "<<endmsg;
        
        newMCDigit->addToMCHits((*iterOnHitsCardiac)->getMCHit());
        // info()<<" hit added "<<endmsg;
        //          (*iter)->chInfo()  
      }
      
      newMCDigit->setDigitInfo((*iter)->chInfo());
      if((*iter)->hasFired())
        {
          
          newMCDigit->setFiringTime((*iter)->firingTime());
        }
          
          //info()<<(*iter)->firingTime()<<" qui "
          //     <<(*iter)->chInfo().isAlive()<<endmsg;
      mcDigitContainer.insert(newMCDigit); 
      ++countDigits;
        
      
    }
    
    
  }
  
  
  if(msgLevel(MSG::DEBUG)) debug()<<" MC Digits created "<<countDigits<<endmsg; 
  return StatusCode::SUCCESS;
}

  


StatusCode MuonDigitization::createRAWFormat(LHCb::MCMuonDigits& mcDigitContainer, 
					     LHCb::MuonDigits& digitContainer){
  MsgStream log(msgSvc(), name()); 
  LHCb::MCMuonDigits::iterator iterMCDigit; 
  for( iterMCDigit = mcDigitContainer.begin();iterMCDigit < 
         mcDigitContainer.end() ; iterMCDigit ++){
    
    
    LHCb::MCMuonDigitInfo digiinfo=(*iterMCDigit)->DigitInfo();
    
    if(digiinfo.isAlive()){		  
      LHCb::MuonDigit* muonDigit= new LHCb::MuonDigit((*iterMCDigit)->key());
      unsigned int time=(unsigned int)(((*iterMCDigit)->firingTime())
				       /(m_timeBin));
      //			if(time>7)time=7;
      muonDigit->setTimeStamp(time);
      digitContainer.insert(muonDigit);
      LHCb::MuonTileID gg=(*iterMCDigit)->key();
      if(msgLevel(MSG::DEBUG)) {
        debug()<<"new daq word LO/s/r/q/nx/ny/time "<<
          gg.layout()<<" "<<gg.station()<<" "<<gg.region()<< 
          " "<<gg.quarter()<<" "<<gg.nX()<<" "<<gg.nY()<<" "<<" "<<time<<endmsg;
        debug()<<gg<<endmsg;
      }

      //        " "<<time<<endmsg;
    }
  }
  return StatusCode::SUCCESS;	 
}

StatusCode MuonDigitization::
addElectronicNoise(MuonDigitizationData
                   <MuonPhysicalChannel>& PhysicalChannel){
  
	MsgStream log(msgSvc(), name()); 
	MuonPhysicalChannel* pFound;
  for(int ispill=1;ispill<=m_numberOfEvents;ispill++){
    int chamberTillNow=0;
    double shiftOfTOF=-m_BXTime*(ispill-1);
    for(int i=0;i<m_stationNumber;i++){
      for(int k=0;k<m_regionNumber;k++){
        int partitionNumber=i*m_regionNumber+k;
        int chamberInRegion=m_muonDetector->chamberInRegion(i,k);
        //					 int gapFE=m_pGetInfo->getGapPerFE(partitionNumber);
        for(int chamber=0;chamber<chamberInRegion;chamber++){		
          for(int frontEnd=0;frontEnd<(int)(m_muonDetector->gapsInRegion(i,k)/
                                            m_muonDetector->gapsPerFE(i,k));
              frontEnd++){
            for(int readout=0;readout<m_muonDetector->readoutInRegion(i,k);
                readout++){
              int phChInX=m_muonDetector->getPhChannelNX( readout,i,k);   
              int phChInY=m_muonDetector->getPhChannelNY( readout,i,k);
              //              info()<<"pre noiseChannels"<<partitionNumber<<" "<<readout<<endmsg; 
              int noiseChannels=(detectorResponse.getResponse
                (partitionNumber,readout))->
                electronicNoise();
              //info()<<"noiseChannels "<<noiseChannels<<" "<<partitionNumber<<endmsg;
              unsigned int readoutType=
                m_muonDetector->getReadoutType(readout,i,k);
              for(int hitNoise=0;hitNoise<noiseChannels;hitNoise++){
                int chX=(int)(m_flatDist()*phChInX);
                int chY=(int)(m_flatDist()*phChInY);
                if(chX==phChInX)chX=phChInX-1;
                if(chY==phChInY)chY=phChInY-1;
               double time=m_flatDist()*m_BXTime+shiftOfTOF ;
                MuonPhChID ID;
                ID.setStation(i);
                ID.setRegion(k);
                LHCb::MuonTileID chTile;
		StatusCode scchamber=m_muonDetector->Chamber2Tile(chamber,i,k,chTile);
                if(scchamber.isFailure())warning()<<" failure of chamber 2 tile conversion "<<endmsg;
                ID.setQuadrant(chTile.quarter());
                //                ID.setQuadrant(m_pGetInfo->getQuadrantChamber
                //               (chamber+chamberTillNow));
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
                  setHitNature(LHCb::MuonOriginFlag::ELECTRONICNOISE);
                pointerToHit->getMCMuonHitOrigin().setBX(ispill-1);	
                pointerToHit->getMCMuonHistory().setBXOfHit(ispill-1);
                pointerToHit->getMCMuonHistory().
                  setNatureOfHit(LHCb::MuonOriginFlag::ELECTRONICNOISE);
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
                  StatusCode asc=PhysicalChannel.addMuonObject(partitionNumber,
                                                newPhysicalChannel);
                  if(asc.isFailure() && msgLevel(MSG::DEBUG))
                    debug()<<" unable to add hit "<<endmsg;
                  
                }
              }	 
            }	 
          }
        }
        chamberTillNow=chamberTillNow+chamberInRegion;	
      }									 							 		
    }			 
	}		
  //info()<<"esco "<<endmsg;
  return StatusCode::SUCCESS;	 
  
}
