// $Id: MuonTest.cpp,v 1.2 2002-11-06 09:51:23 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "MuonAlgs/MuonTest.h"
 
#include "GaudiKernel/IToolSvc.h"  
#include "MuonAlgs/MuonTest.h"
#include "GaudiKernel/LinkManager.h" 
#include "GaudiKernel/IChronoStatSvc.h"

#include <iostream>


static const AlgFactory<MuonTest>  Factory;
const IAlgFactory& MuonTestFactory = Factory;
static std::string stationPath[5]={"1","2","3","4","5"};
static std::string regionPath[4]={"1","2","3","4"};
static std::string TESPathOfHitsContainer[]={"Hits","ChamberNoiseHits",
                                             "FlatSpilloverHits",
                                             "BackgroundHits"};
static std::string spill[] = {"","/Prev","/PrevPrev","/Next","/NextNext"};
const int OriginOfHitsContainer[4]=
{MuonOriginFlag::GEANT,MuonOriginFlag::CHAMBERNOISE,
 MuonOriginFlag::FLATSPILLOVER,MuonOriginFlag::BACKGROUND};



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonTest::MuonTest( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) {

}


//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonTest::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  StatusCode sc=toolSvc()->retrieveTool("MuonTileIDXYZ",m_pMuonTileXYZ); 
  m_histoTiming=histoSvc()->book("/stat/muon/1"," (ms)",50,0.,10.0);
  NTuplePtr nt(ntupleSvc(), "FILE1/100");
  StatusCode status;
  if ( !nt )  {
    nt = ntupleSvc()->book ("FILE1/100",CLID_ColumnWiseTuple  , 
                            "Test digitization");
    if ( nt )    {
      status = nt->addItem ("Multiplicity",   m_hit ,0, 4000);
      status = nt->addIndexedItem ("x ", m_hit, m_x);
      status = nt->addIndexedItem ("y ", m_hit, m_y);
      status = nt->addIndexedItem ("z ", m_hit, m_z);
      status = nt->addIndexedItem ("time ", m_hit, m_time);
      status = nt->addIndexedItem ("nature ", m_hit, m_nature);
      status = nt->addIndexedItem ("region ", m_hit, m_region);
      status = nt->addIndexedItem ("spill ", m_hit, m_spill);
      status = nt->addIndexedItem ("muonflag ", m_hit, m_muon)	;		
      status = nt->addIndexedItem ("fired ", m_hit, m_fired)		;	
      status = nt->addIndexedItem ("hitted ", m_hit, m_hitted)	;		
      status = nt->addIndexedItem ("dead ", m_hit, m_dead)	;		
      //      status = nt->addIndexedItem ("notfound ", m_hit, m_notfound);		
      status = nt->addIndexedItem ("mother ", m_hit, m_mother)	;		 
			m_digiNtuple = nt;
	  }	
  }
 NTuplePtr ntError(ntupleSvc(), "FILE1/101");
 
 if ( !ntError )  {
    ntError = ntupleSvc()->book ("FILE1/101",CLID_ColumnWiseTuple  , 
                                 "Errors in the muon digitization");
    if ( ntError )    {
		  status = ntError->addItem ("HitNotInDigit",   m_hitNoInDigit ,0, 100);
		  status = ntError->addItem ("HitInInput",   m_hitInInput ,0, 4000);
		  status = ntError->addItem ("HitWPlacedX",  m_hitWronglyPalcedX  ,0, 100);
		  status = ntError->addItem ("HitWPlaced",  m_hitWronglyPalcedY  ,0, 100);
 			m_digiError = ntError;
	  }	
 }
 
  NTuplePtr ntMCDigit(ntupleSvc(), "FILE1/102");
  
  if ( !ntMCDigit )  {
    ntMCDigit = ntupleSvc()->book ("FILE1/102",CLID_ColumnWiseTuple  , 
                                   "MCMuonDigit");
    if ( ntMCDigit )    {
		  status = ntMCDigit->addItem ("MCDigit",   m_mcdigitnumber ,0, 4000);
      status = ntMCDigit->addIndexedItem ("alive ", m_mcdigitnumber, 
                                          m_mcmuondigitalive);			
      status = ntMCDigit->addIndexedItem ("region ", m_mcdigitnumber, 
                                          m_mcmuondigitregion);			
		  status = ntMCDigit->addItem ("MCMuonHit",   m_mcmuonhit ,0, 5000);
      status = ntMCDigit->addIndexedItem ("nature ", m_mcmuonhit, 
                                          m_mcmuonhitnature);
      status = ntMCDigit->addIndexedItem ("firing ", m_mcmuonhit, 
                                          m_mcmuonhitalive);
      status = ntMCDigit->addIndexedItem ("partition ", m_mcmuonhit, 
                                          m_mcmuonhitpartition);
      status = ntMCDigit->addIndexedItem ("time ", m_mcmuonhit, 
                                          m_mcmuonhittime);			
      status = ntMCDigit->addIndexedItem ("chaefficiency ", m_mcmuonhit, 
                                          m_mcmuonhitefficiency);
      m_mcdigit = ntMCDigit;
	  }	
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonTest::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
//test poisson distribution


	Rndm::Numbers*	poissonDist=new Rndm::Numbers;
	poissonDist->initialize( randSvc(), Rndm::Poisson(0.0231556));
  //	for(int jjj=0;jjj<1000000;jjj++){
  //	m_histoTiming->fill((*poissonDist)(),1.0);}
  m_hitNoInDigit=0;
  m_hitInInput=0;
	m_hitWronglyPalcedX =0;
	m_hitWronglyPalcedY =0;
 	MuonTileID tile;
  SmartDataPtr<MCMuonDigits> mcdigit(eventSvc(),
                                     MCMuonDigitLocation::MCMuonDigit);  	
  MCMuonDigits::const_iterator idigit;
	SmartRefVector<MCMuonHit>::iterator it;	
	for (int nspill=0;nspill<5;nspill++){
    for(int station=0;station<5;station++){
      for(int region=0;region<4;region++){
        for (int container=0;container<4;container++){
          std::string
			         path="/Event"+ spill[nspill]+"/MC/Muon/M"+
            stationPath[station]+"/R"+regionPath[region]+"/"
            +TESPathOfHitsContainer[container];
          log<<MSG::DEBUG<<path<<endreq;
          SmartDataPtr<MCMuonHits> hitPointer(eventSvc(),path);			
          MCMuonHits::const_iterator i;
							 if(hitPointer){
                 for (i=(hitPointer)->begin();i<(hitPointer)->end();i++){  
                   // test that each hit is already been inserted 
                   //inside on MCMuonDigit
                   bool muon=false;
                   int mcode=9999;
                   int notfound=0;
                   int fired=0;
                   int dead=0;
                   double x=  ((*i)->entry().x()+(*i)->exit().x())/2 ;
                   double y=  ((*i)->entry().y()+(*i)->exit().y())/2 ;
                   double z=  ((*i)->entry().z()+(*i)->exit().z())/2 ;
                   int numberTileOutput;
                   MuonTileID phChTileID[2];
                   StatusCode errorcode=locateMuonTileFromXYZ(x,y,z, 
                                                              numberTileOutput
                                                              , phChTileID);
                   if(nspill==0){
                     MCParticle* particle=(*i)->mcParticle();
                     if(particle){
                       int pid= particle->particleID().abspid();
                       if(pid==13||pid==-13){
												   muon=true;
													 const MCParticle* madre=particle->mother();
													 if(madre){
                             //	 log<<MSG::INFO<<"ciao" <<madre<<endreq;
                             mcode=madre->particleID().abspid();
                             //	log<<MSG::INFO<<"ciao" <<madre<<" "<<mcode
                             //<<endreq;
													 }
                       }
                     }
                   }
							      for (int tile=0; tile<numberTileOutput;tile++){			
                      MCMuonDigit* object = mcdigit->object(phChTileID[tile]);
                      if(object) {
                        if(object->DigitInfo().isAlive()){
													  fired++;
                        }else{
                          if(object->DigitInfo().isInDeadTime())dead=dead+1;
                          if(object->DigitInfo().
                             isDeadForChamberInefficiency())dead=dead+10;
                          if(object->DigitInfo().isDeadByTimeJitter())
                            dead=dead+100;
                          if(object->DigitInfo().isDeadByTimeAdjustment())
                            dead=dead+1000;
													}
                        bool found=false;
                        SmartRefVector<MCMuonHit>::iterator iterhit= 
                          (object)->mcMuonHits().begin();
                        for(iterhit=(object)->mcMuonHits().begin(); 
                            iterhit<(object)->mcMuonHits().end() ; iterhit++){
                          if((*iterhit).target()==(*i)){
                            found=true ; 
                            break ;
                          }		
                        }
                        if(!found)log<<MSG::INFO<<path<<
                                    " missing a MCMuonHit in "
                                     <<phChTileID[tile]<<endreq;
                      }
                      else {
                        if(nspill==0){   
                          notfound=notfound+1;
                          m_hitNoInDigit++;												
                          log<<MSG::INFO<<path<<
                            " a missing MCMuonDigit has been found "<<
                            phChTileID[tile]<<endreq;
                          log<<MSG::DEBUG<<"error code"<<errorcode<<endreq;
                          log<<MSG::DEBUG<<"coordinates "<<x<< " "<<y<<" "
                           <<z<<endreq ;
                          locateMuonTileFromXYZ(x,y,z, numberTileOutput, 
                                                phChTileID, false);
                        }
                        
                      }
							      }
                    if(m_hit < m_hit->range().distance()){
                      m_x[m_hit]=((*i)->entry().x()+(*i)->exit().x())/2;
                      m_y[m_hit]=((*i)->entry().y()+(*i)->exit().y())/2;
                      m_z[m_hit]=((*i)->entry().z()+(*i)->exit().z())/2;
                      m_time[m_hit]=((*i)->timeOfFlight());
                      m_nature[m_hit]=OriginOfHitsContainer[container];
                      m_region[m_hit]=station*4+region;
                      m_spill[m_hit]=nspill;
                      m_muon[m_hit]=(int)muon;
                      m_fired[m_hit]=fired;
                      m_hitted[m_hit]=numberTileOutput;
                      m_dead[m_hit]=dead;
                      //										 m_notfound[m_hit]=notfound;
                      m_mother[m_hit]=mcode;										 
                      m_hit++;
                    }	 
 					  	   }
				  	   }else{
							    log<<MSG::DEBUG<<path<<  " not found" <<endreq; 
							 }
        }                        					
      }
	   } 
	}
  
  //test that all hits inside a digit are correctly placed there
	
 	for(idigit=mcdigit->begin();idigit<mcdigit->end();idigit++){
	  std::vector<MCMuonHitHistory>& HitsHistory();
		std::vector<MCMuonHitHistory>::iterator iterHistory=
      (*idigit)->HitsHistory().begin();
		SmartRefVector<MCMuonHit>::iterator it;
		double x,y,z,dx,dy,dz;
		if(m_mcdigitnumber < m_mcdigitnumber->range().distance()){ 
		  if((*idigit)->DigitInfo().isAlive())m_mcmuondigitalive[m_mcdigitnumber]
                                            =1 ;
			m_mcmuondigitregion[m_mcdigitnumber]=
        (*idigit)->key().station()*4+(*idigit)->key().region();
		   m_mcdigitnumber++;
		}
		it=(*idigit)->mcMuonHits().begin();
		m_pMuonTileXYZ->calcTilePos((*idigit)->key(), x,dx,y,dy,z,dz ) ; 	
		bool testfiring=false;
		for(iterHistory=	(*idigit)->HitsHistory().begin();iterHistory
          <(*idigit)->HitsHistory().end();iterHistory++){
 		  if(m_mcmuonhit < m_mcmuonhit->range().distance()){
        m_mcmuonhitnature[m_mcmuonhit]=(int)iterHistory->natureOfHit();
        m_mcmuonhitalive[m_mcmuonhit]=(int)iterHistory->hasFired();	
			if(iterHistory->hasFired()){
        m_mcmuonhittime[m_mcmuonhit]=(*idigit)->firingTime();
			}else{
        m_mcmuonhittime[m_mcmuonhit]=-999;
			}
      
			if(iterHistory->hasFired()){
        if(!testfiring){testfiring=true;}
        else {log<<MSG::INFO<<"error this hit fired"<<endreq;}
			}
      m_mcmuonhitpartition[m_mcmuonhit]=
        (*idigit)->key().station()*4+(*idigit)->key().region();
      m_mcmuonhitefficiency[m_mcmuonhit]=  
        (int) iterHistory->isKilledByChamberInefficiency();
			m_mcmuonhit++;}
		  if((*it)){
        double xHiti=  ((*it)->entry().x()) ;
        double yHiti=  ((*it)->entry().y()) ;
        double zHiti=  ((*it)->entry().z()) ;
        double xHito=  ((*it)->exit().x()) ;
        double yHito=  ((*it)->exit().y()) ;
        double zHito=  ((*it)->exit().z()) ;
				 
				 
        if(fabs(x-xHiti)>dx+0.1){
          if(fabs(x-xHito)>dx+0.1){
            if((x>xHiti)&&(x<xHito)) goto xcheck;
            if((x>xHito)&&(x<xHiti)) goto xcheck;							 
							 m_hitWronglyPalcedX++;
						   log<<MSG::INFO<<"error this hit is outside the digit x"<<endreq;
							 log<<MSG::INFO<<"  "<<x<<" "<<xHiti<<" "<<xHito<<" "
                  <<dx<<endreq; }
        } 
      xcheck: 
        if(fabs(y-yHiti)>dy+0.1){
				    if(fabs(y-yHito)>dy+0.1){
              if((y>yHiti)&&(y<yHito)) goto ycheck;
						   if((y>yHito)&&(y<yHiti)) goto ycheck;							 
							 m_hitWronglyPalcedY++;
               log<<MSG::INFO<<"error this hit is outside the digit y"
                  <<endreq;  
               log<<MSG::INFO<<"  "<<y<<" "<<yHiti<<" "<<yHito<<" "<<dy
                  <<endreq; }
        }
			ycheck: 
        int ipop;
  		}
			it++;
 	  }
 	}
	m_digiNtuple->write();
//	m_digiError->write();
	m_mcdigit->write();

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonTest::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode MuonTest::locateMuonTileFromXYZ( double x,double y,double z,
                                            int& numberTileOutput, MuonTileID
                                            phChTileID[2], bool debug){
  MuonDigitizationParameters::Parameters usefull;  
	MsgStream log(msgSvc(), name());
  int layoutX[2][20]=
  {
    {24,24,24,24,48,48,48,48,48,48,48,48,12,12,12,12,12,12,12,12},
    { 0, 0, 0, 0, 8, 4, 2, 2, 8, 4, 2, 2, 0, 4, 2, 2, 0, 4, 2, 2}
  };
  int layoutY[2][20]=
  {  
    { 8, 8, 8, 8, 1, 2, 2, 2, 1, 2, 2, 2, 8, 2, 2, 2, 8, 2, 2, 2},
    { 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 0, 8, 8, 8, 0, 8, 8, 8}
  }; 
  
  unsigned int station;
  unsigned int region;
  unsigned int chamber;
  unsigned int readout;
  unsigned int quadrant;
  unsigned int idX;
  unsigned int idY;
  unsigned int newidX,newidY;
  unsigned int idXGlobal,idYGlobal;
  unsigned int idLogX,idLogY;
  int numberOfTileCreated=0;
  unsigned int numberOfPCX, numberOfPCY;
  MuonTileID chamberTile;
  std::vector<MuonChannel> muonChannels;
  DeMuonGasGap* pGasGap;
  m_pMuonTileXYZ->locateFEFromXYZ(x,y,z,chamberTile,muonChannels, pGasGap);
  station=chamberTile.station();
  region=chamberTile.region();
  quadrant=chamberTile.quarter();
  int partition=(station)*4+region;	 
  std::vector<MuonChannel>::iterator iter;
  unsigned int chamberAbs=1;  
  numberTileOutput=0;	
  for(int partit=0; partit<partition; partit++){
    chamberAbs=chamberAbs+usefull.getChamberPerRegion(partit);
  }
  if(debug){
    log<<MSG::INFO<<"station "<<station<<" region "
       <<region<<" quadrant"<< quadrant<<endreq;
  }
	 
  for (iter=muonChannels.begin();iter<muonChannels.end();iter++){
    idX=iter->xPos();
    idY=iter->yPos();
    readout=iter->readout();
    if(debug){
      log<<MSG::DEBUG<<" channels  "<<idX << " "<<idY<<endreq;
			}
    
    
    //
    // loop over FE channel readout
    //
    for(int mapNumber=0;mapNumber<(int)usefull.getLogMapPerRegion(partition);
        mapNumber++){
      //
      // check if current readout coincides with one of the LogMap readouts
      //
      
      if(usefull.getLogMapRType(mapNumber,partition)==readout)  {
        //
        // define order of FE channel according to the MuonTileID 
        //conventions: from 0,0 left,bottom to radial coordinates
        //
        for(int countReadout=0; countReadout<=
              usefull.getReadoutNumber(partition);countReadout++)
          {
            if(readout==usefull.getReadoutType(countReadout,partition)){
              numberOfPCX=usefull.getPhChannelNX(countReadout,partition);
              numberOfPCY=usefull.getPhChannelNY(countReadout,partition);
              if(debug){
                log<<MSG::DEBUG<<" channels in the chamber  "
                   << numberOfPCX << " "<<numberOfPCY<<endreq;    
              }	     
            }
          }
        if(idX>=numberOfPCX||idX<0){
          return StatusCode::FAILURE;
        }
        if(idY>=numberOfPCY||idX<0){
          return StatusCode::FAILURE;
        }
   
        // 
        // FE ch address relative to the chamber transformed in 
        //quadrant reference system
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

          idXGlobal=newidX+chamberTile.nX()*numberOfPCX ;
          idYGlobal=newidY+chamberTile.nY()*numberOfPCY ;
          
          //
   //  compute Logical Channel address now
          //
          idLogX=idXGlobal/usefull.getLogMapMergex(mapNumber,partition);    
          idLogY=idYGlobal/usefull.getLogMapMergey(mapNumber,partition);   
          //
          // create the tile of the phys chan
          //  
          ++numberTileOutput;
          
          // MuonLayout layout(layoutX[mapNumber][partition],
          //layoutY[mapNumber][partition]);   
          MuonLayout layout(usefull.getLayoutX(mapNumber,partition),
                            usefull.getLayoutY(mapNumber,partition));   
          phChTileID[numberOfTileCreated].setLayout(layout);
          phChTileID[numberOfTileCreated].setStation(station);
          phChTileID[numberOfTileCreated].
            setReadout(usefull.getLogMapRType(mapNumber,
                                              partition));
          phChTileID[numberOfTileCreated].setRegion(region);
          phChTileID[numberOfTileCreated].setQuarter(quadrant);
          phChTileID[numberOfTileCreated].setX(idLogX);
          phChTileID[numberOfTileCreated].setY(idLogY);
          
          ++numberOfTileCreated;
          //cout <<"calculated nb of tile created "<< 
          //numberOfTileCreated<< endl;
      }else{
        //		cout<<"calculate readout not coincident"<<
        //usefull.getLogMapRType(readoutNumber,partition)<<" "<<readout<<endl;
      }
    }
  }   
  return StatusCode::SUCCESS;
}
