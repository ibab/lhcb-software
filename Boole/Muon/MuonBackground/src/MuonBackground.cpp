// $Id: MuonBackground.cpp,v 1.25 2006-01-31 17:04:16 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogramFactory.h"
// from detector description
#include "MuonDet/DeMuonGasGap.h"
#include "MuonDet/MuonBasicGeometry.h"
#include "Kernel/MuonTileID.h"

// Event model
#include "Event/MCHit.h"
#include "Event/GenCollision.h"

// local
#include "MuonBackground.h"
#include "ParticleInfo.h"

#define DIMENSIONMAX 1

//-----------------------------------------------------------------------------
// Implementation file for class : MuonBackground
//
// 2003-02-18 : Alessia Satta
//-----------------------------------------------------------------------------




// Declaration of the Algorithm Factory
static const  AlgFactory<MuonBackground>          s_factory ;
const        IAlgFactory& MuonBackgroundFactory = s_factory ; 


std::string MuonBackground::spill[5] = 
{"","/Prev","/PrevPrev","/Next","/NextNext"};
std::string MuonBackground::numreg[4] = 
{"1","2","3","4"};
std::string MuonBackground::numsta[5]= 
{"1","2","3","4","5"};
 std::string MuonBackground::TESPathOfHitsContainer[4]=
{"Hits","ChamberNoiseHits","FlatSpilloverHits","BackgroundHits"};


enum BackgroundType {LowEnergy=0,FlatSpillover};
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonBackground::MuonBackground( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) {
  declareProperty("HistogramsFile" , m_histoFile="InFile") ;
  //declare the offset to reconstruct each distribution 
  //starting form histogram
  //the template is 100000+par1*10000+station*1000+multiplicity*100
  declareProperty("ContainerName" , m_containerName) ;    
  declareProperty("HistogramsUpCode" , m_histogramsUpCode) ;  
  declareProperty("HistogramsNumber" , m_histogramsMapNumber) ; 
  declareProperty("HistogramsDimension" , m_histogramsDimension) ; 
  declareProperty("HistogramsMeaning" , m_histoName) ; 
  declareProperty("SafetyFactors" , m_safetyFactor ) ; 
  declareProperty("BXTime" , m_BXTime=25.0 ) ; 
  declareProperty("Luminosity" , m_luminosity=2.0 ) ;
  declareProperty("AverageFlatHits" , m_flatSpilloverHit ) ;
  declareProperty("FlatSpillNumber" , m_numberOfFlatSpill=1 ) ;
  declareProperty("BackgroundType" , m_typeOfBackground ) ;
  declareProperty("DebugHistos" , m_histos=false ) ;
}

//=============================================================================
// Destructor
//=============================================================================
MuonBackground::~MuonBackground() {
  //ricordarsi di distruggere tutte le distribuzioni e tutti i
  //mubgdistributions
}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonBackground::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by  GaudiAlgorithm	
  m_muonDetector=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  

  //MsgStream msg(msgSvc(), name());
  debug() << "==> Initialise" << endreq;

  // Get the application manager. Used to find the histogram persistency type
  // and to get number of spillovers from SpillOverAlg
  IAlgManager* algmgr;
  sc = service( "ApplicationMgr", algmgr );
  if( !sc.isSuccess() ) {
    err() << "Failed to locate algManager i/f of AppMgr"<< endmsg;
    return sc;
  }
  IProperty* algmgrProp;
  algmgr->queryInterface( IID_IProperty, (void**)&algmgrProp );

  StringProperty persType;
  persType.assign( algmgrProp->getProperty("HistogramPersistency") );
  m_persType = persType.value();
  debug() << "Histogram persistency type is " << m_persType << endmsg;
    

  // initialize geometry based quantities as station number, 
  //gaps per station and their z position 
  m_numberOfEventsNeed=5;
  
  MuonBasicGeometry basegeometry(detSvc(),msgSvc()); 
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();  int i=0;  
  while(i<m_stationNumber){
    numsta[i]=basegeometry.getStationName(i);    
    debug()<<" station "<<i<<" "<<numsta[i]<<endreq;
    i++;    
  }
 
//  sc=toolSvc()->retrieveTool("MuonTileIDXYZ",m_pMuonTileXYZ); 
//  if(sc.isFailure())return StatusCode::FAILURE;
//  sc=toolSvc()->retrieveTool("MuonGetInfoTool",m_pGetInfo);
//  if(sc.isFailure())return StatusCode::FAILURE;
  m_partition=basegeometry.getPartitions();
  initializeGeometry();
 
 
  m_correlation.resize(m_maxDimension);
  m_radial.resize(m_maxDimension);
  m_phiglobvsradial.resize(m_maxDimension);
  m_thetalocvsradial.resize(m_maxDimension);
  m_philocvsradial.resize(m_maxDimension);
  m_logtimevsradial.resize(m_maxDimension);  
  m_lintimevsradial.resize(m_maxDimension);
  m_hitgap.resize(m_maxDimension);
 
  initializeParametrization();
  m_flatDistribution =new Rndm::Numbers; 
  m_flatDistribution->initialize(randSvc(), Rndm::Flat(0.0,1.0));
  debug()<<" type input "<<m_typeOfBackground<<endreq;
    debug()<<" safety factor "<<m_safetyFactor[0]<<
	" "<<m_safetyFactor[1]<<" "<<m_safetyFactor[2]<<" "<<
	m_safetyFactor[3]<<" "<<m_safetyFactor[4]<<endreq;

  if(m_typeOfBackground=="LowEnergy"){
    m_type=LowEnergy;    
  }else if(m_typeOfBackground=="FlatSpillover"){
    m_type=FlatSpillover;        
  }
  debug() <<" type "<< m_type<<endreq;
  if( m_type==LowEnergy ){    

    if( m_histos ) {
      for (int y=0;y<5;y++){
        for (int kk=0;kk<4;kk++){
          int hh;
          hh=y*4+kk;
          char label[10]; 
          if(hh<9)sprintf(label,"%1i",hh+1);
          if(hh>=9)sprintf(label,"%2i",hh+1);
 
          std::string ap="STAT/";
          std::string label2=ap+label;

          m_pointer1D[hh]=
            histoSvc()->book( label, "HT multiplicity", 50, 0., 50. );
          m_pointer2D[hh]=
            histoSvc()->book( label2, "ADD bk vs HT multiplicity",
                              50, 0., 50., 50 ,0., 50. );
        }
      }
    }
    
    // Get the number of spillover events from the SpilloverAlg
    IAlgorithm*  spillAlg;
    sc = algmgr->getAlgorithm( "SpilloverAlg", spillAlg );
    if( !sc.isSuccess() ) {
      warning() << "SpilloverAlg not found" << endmsg;
      m_readSpilloverEvents = 0;
    }
    else {
      IProperty* spillProp;
      spillAlg->queryInterface( IID_IProperty, (void**)&spillProp );
      StringArrayProperty evtPaths;
      evtPaths.assign( spillProp->getProperty("PathList") );
      m_readSpilloverEvents = evtPaths.value().size();
      // Release the interface, no longer needed
      spillAlg->release();
    }

    debug() << "number of spillover events read from aux stream "
        << m_readSpilloverEvents << endmsg;  
  }else if(m_type==FlatSpillover){
    m_readSpilloverEvents=1;
    m_luminosityFactor=m_luminosity/2.0;   
    //    m_readSpilloverEvents=m_numberOfFlatSpill;    
  }

  // Release interface, no longer needed  
  algmgr->release();
  //initialize the detector
  m_muonDetector=NULL;
  m_muonDetector=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonBackground::execute() {

  //MsgStream  msg( msgSvc(), name() );
  debug() << "==> Execute" << endreq;
  for (int ispill=0;ispill<=m_readSpilloverEvents;ispill++){    
    calculateNumberOfCollision(ispill);  
    //    if(!collisions())return  StatusCode::SUCCESS;
    //	 if(collisions()>1)return  StatusCode::SUCCESS;
      if(!collisions()) continue;

    debug() << "==> collision " << collisions()<<endreq;

 
    LHCb::MCHits * 
    hitsContainer[DIMENSIONMAX]; // fix for 	VC6
    hitsContainer[0]
      =new LHCb::MCHits();
    
    m_resultPointer = new std::vector<std::vector<int> > (collisions()) ;

    if(m_type==LowEnergy){
      // m_resultPointer = new std::vector<std::vector<int> > (collisions()) ;
      calculateStartingNumberOfHits(ispill);    
      for(int coll=0;coll<collisions();coll++){      
        for(int station=0;station<m_stationNumber;station++){        
          for (int multi=0;multi<m_gaps;multi++){
            int index=station*m_gaps+multi;
            int startingHits=((*m_resultPointer)[coll])[index];
            debug()<<"station safe start end hits "
                <<startingHits<<endreq;
            
            // extract number of hits to be added
            int hitToAdd=0;
            float floatHit = 0;          
            if(numsta[station]=="M1"){
              floatHit=float(startingHits*(m_safetyFactor[station]));   
              hitToAdd=howManyHit( floatHit);            
            } else{
	debug()<<"adding "<<	index <<" "<<startingHits<<endreq;          
              int yy=(int)(m_correlation[index])->
                         giveRND(startingHits+0.5F);
              
              floatHit=float(m_safetyFactor[station]*(yy));
              hitToAdd=howManyHit( floatHit);
              //debug()<<"station safe start end hits "<<
              //  station<< " "<<m_safetyFactor[station]<< " "<<
              //  yy<< " "<< hitToAdd<<" "<<startingHits<<endreq;
              
              debug()<<"adding "<< hitToAdd<<" to orginal "<<
                startingHits<<" in station "<<station<<
                " for multiplicity "<<multi<<" and collisions/spill "<<coll
                 <<" "<<ispill<<endreq;
            }            
		 debug()<<" station "<<station<<endreq;
            for(int hitID=0;hitID< hitToAdd;hitID++){            
            createHit(hitsContainer, station,multi,ispill);            
            }
            if( m_histos ) {
              m_pointer1D[index]->fill(startingHits+0.00001,1.0);
              m_pointer2D[index]->
                fill(startingHits+0.00001,hitToAdd+0.00001,1.0);
            }
          }
          
        }        
      }      
    }else if(m_type==FlatSpillover){
      if(ispill>0) {
        delete m_resultPointer;
        delete hitsContainer[0];
        break;
      }
      
      for(int station=0;station<m_stationNumber;station++){        
        for (int multi=0;multi<m_gaps;multi++){
          int index=station*m_gaps+multi;
          float floatHit = float(m_flatSpilloverHit[index]*m_luminosityFactor
            *m_safetyFactor[station]);         
          for (int fspill=0;fspill<=m_numberOfFlatSpill;fspill++){
            int hitToAdd=0;
            hitToAdd=howManyHit( floatHit);          
            debug()<<"adding "<< hitToAdd<<
              " in station "<<station<<
              " for multiplicity "<<multi<<" and spill"
               <<fspill<<endreq;
            for(int hitID=0;hitID< hitToAdd;hitID++){            
              createHit(hitsContainer, station,multi,fspill);            
            }         
          }          
        }        
      }
    }
    debug()<<" starting saveing the ocntainer "<<endreq;
    
    /*    //alessia attenta a rimmetter a posto questo ..//      
    for(int station=0;station<m_stationNumber;station++){
      for(int region=0;region<m_regionNumber;region++){
        std::string path="/Event"+spill[ispill]+"/MC/Muon/"+numsta[station]+
          "/R"+numreg[region]+"/"+m_containerName;
        debug()<<" number of total hit added "<<
          hitsContainer[station*m_regionNumber+region]->size()<<endreq;
        //   eventSvc()->registerObject(path,
        //                         hitsContainer[station*m_regionNumber
        //                                       +region]);   
      }      
      }*/
    delete m_resultPointer;    
  }  
  return StatusCode::SUCCESS;
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonBackground::finalize() {

  MsgStream msg(msgSvc(), name());
  debug() << "==> Finalize" << endreq;
  delete m_flatDistribution;
  for (int i=0;i<m_maxDimension;i++){
    MuBgDistribution* pointDelete=m_correlation[i];    
    if(pointDelete)delete pointDelete;
    pointDelete=m_radial[i];
    //  msg << MSG::INFO << "==> deleting  " << endreq;    
    if(pointDelete)delete pointDelete;
    pointDelete=m_phiglobvsradial[i];
    if(pointDelete)delete pointDelete;
    pointDelete=m_thetalocvsradial[i];
    if(pointDelete)delete pointDelete;
    pointDelete=m_philocvsradial[i];
    if(pointDelete)delete pointDelete;
    pointDelete=m_logtimevsradial[i];
    if(pointDelete)delete pointDelete;
    pointDelete=m_lintimevsradial[i];
    if(pointDelete)delete pointDelete;
    pointDelete=m_hitgap[i];    
    if(pointDelete)delete pointDelete;
  }
//  if( m_pMuonTileXYZ ) toolSvc()->releaseTool( m_pMuonTileXYZ );    
 // if( m_pGetInfo )     toolSvc()->releaseTool( m_pGetInfo );    
 return GaudiAlgorithm::finalize();

  return StatusCode::SUCCESS;
}


//=============================================================================
//=============================================================================
//  GeometryInitialization
//=============================================================================
StatusCode MuonBackground::initializeGeometry() {

  MsgStream msg(msgSvc(), name());
  debug() << "==> initializeGeometry" << endreq;

  int gap=0;
  
  for(int i=0;i< (int)m_partition;i++){
    int stat=i/4;
    int reg=i-stat*4;
    gap=(gap>=(int)m_muonDetector->gapsInRegion(stat,reg))?
      gap:m_muonDetector->gapsInRegion(stat,reg);
    
  }
  
  m_gaps=gap;
  m_maxDimension=m_gaps*m_stationNumber;
  //  containerOfFirstGapPosition.resize(m_stationNumber+1);
  int chamber=0;
  
  for(int station=0;station<5;station++){
    for(int region=0;region<4;region++){
      m_chamberInRegion[station*4+region]=chamber;
      chamber=chamber+m_muonDetector->chamberInRegion(station,region);      
    }    
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonBackground::initializeParametrization()
{
  MsgStream msg(msgSvc(), name());
  int numName=m_histoName.size();
  int numCode=m_histogramsMapNumber.size();
  int code=0;
  int gap;  
  std::string name;
  // Add an "h" to histogram identifiers translated by h2root
  std::string sep;
  if( m_persType == "ROOT" ) {
    sep = "/h";
  }
  else {
    sep = "/";
  }
  
  if(numName!=numCode)return StatusCode::FAILURE;
  // the first station is without background!!!!!
  for(int station=0;station<m_stationNumber;station++){
    gap=m_numberOfGaps[station*m_gaps];
    for(int mult=0;mult<m_gaps;mult++){
      int index=station*m_gaps+mult;      
      for(int i=0;i<numCode;i++)
        {
          char codePath[10];          
          code=m_histogramsMapNumber[i];
          name=m_histoName[i];
          int tt=mapHistoCode(code,station,mult);
          debug()<<code<<" "<< station<<" "<<mult<<" "<<tt<<endreq;
          if(tt<100000){
            sprintf(codePath,"%5i", tt); 
          }else{              
            sprintf(codePath,"%6i", tt);
          }
          
          std::string path=m_histoFile+sep+codePath;
          if(numsta[station]=="M1"&&m_histoName[i]=="correlation"){
            // skip the input of the correlation plot for M1 it does not exist
          }else if(m_histogramsDimension[i]==1){
            SmartDataPtr<IHistogram1D> histo1d(histoSvc(),path);
            if(histo1d){ 
              std::vector<Rndm::Numbers*>  distributions;
              double xmin,xmax;
              std::vector<bool>   pointerToFlags;
              initializeRNDDistribution1D(histo1d,
                                          distributions ,pointerToFlags,xmin, 
                                          xmax);
              MuBgDistribution* mubg = new MuBgDistribution( distributions,
                                                             pointerToFlags,
                                                             float(xmin),
                                                             float(xmax)    ); 
              if(m_histoName[i]=="correlation"){                
                m_correlation[index]=mubg;
              }else if(m_histoName[i]=="r"){
                m_radial[index]=mubg;
              }else if(m_histoName[i]=="phi"){
                m_phiglobvsradial[index]=mubg;
              }else if(m_histoName[i]=="thetaloc"){
                m_thetalocvsradial[index]=mubg;
              }else if(m_histoName[i]=="philoc"){
                m_philocvsradial[index]=mubg;
              }else if(m_histoName[i]=="logtime"){
                m_logtimevsradial[index]=mubg;
              } else if(m_histoName[i]=="lintime"){
                m_lintimevsradial[index]=mubg;
              } else if(m_histoName[i]=="hitgap"){
                m_hitgap[index]=mubg;
              }             
            }
            else{
              msg<<MSG::WARNING<<"not found the 1D  histo "<<path<<endreq;
            }
          }            
          else if (m_histogramsDimension[i]==2){
            SmartDataPtr<IHistogram2D> histo2d(histoSvc(),path);
            if(histo2d){
              //   histoPointer=histoin2;
              debug()<<"found the 2D histo "<<path<<endreq;
              std::vector<Rndm::Numbers*>  distributions;
              double xmin,xmax,ymin,ymax;
              int nbinx;              
              std::vector<bool>   pointerToFlags;
              initializeRNDDistribution2D(histo2d,
                                         distributions ,pointerToFlags,xmin, 
                                          xmax, nbinx ,ymin, ymax);
              MuBgDistribution* mubg=new
                MuBgDistribution(distributions,pointerToFlags,
                                 float(xmin), float(xmax), nbinx,
                                 float(ymin), float(ymax)        ); 
              if(m_histoName[i]=="correlation"){
                m_correlation[index]=mubg;
              }else if(m_histoName[i]=="r"){
                m_radial[index]=mubg;
              }else if(m_histoName[i]=="phi"){
                m_phiglobvsradial[index]=mubg;
              }else if(m_histoName[i]=="thetaloc"){
                m_thetalocvsradial[index]=mubg;
              }else if(m_histoName[i]=="philoc"){
                m_philocvsradial[index]=mubg;
              }else if(m_histoName[i]=="logtime"){
                m_logtimevsradial[index]=mubg;
              }else if(m_histoName[i]=="lintime"){
                m_lintimevsradial[index]=mubg;
              } else if(m_histoName[i]=="hitgap"){
                m_hitgap[index]=mubg;
              }                  
            }
            else{
              msg<<MSG::WARNING<<"not found the 2D-histo "<<path<<endreq;
             }
            }
          
        }      
    }
  }
 return StatusCode::SUCCESS;
}


//=============================================================================
StatusCode MuonBackground::calculateNumberOfCollision(int ispill) {
  MsgStream msg(msgSvc(), name());
  debug() << "==> calculateNumberOfCollsion " << endreq;
  int collision=0;
  std::string collisionLocation="/Event"
    +spill[ispill]+"/"+LHCb::GenCollisionLocation::Default;
  debug()<<"string "<<collisionLocation<<endreq;
  debug()<<"spill "<<ispill<<endreq;
  
  SmartDataPtr<LHCb::GenCollisions> collisionsPointer
    ( eventSvc(), collisionLocation );
  LHCb::GenCollisions::iterator itCollision;
  collision=0;  
  if(collisionsPointer){    
    for(itCollision=collisionsPointer->begin();
        itCollision<collisionsPointer->end();itCollision++){
      debug()<<" --- collision number "
         << (*itCollision)->key()<<endreq;
      collision++;      
    }
  }  
  setCollsionNumber(collision);
  debug()<<" --- total collision number "<<collision<<endreq;  
  return StatusCode::SUCCESS; 
}


StatusCode MuonBackground::calculateStartingNumberOfHits(int ispill) {

  debug() << "==> calculateStartingNumberOfHit " << endreq;
  bool first=true;
  int gap,chamber,index,station,region;
  int preGap,preChamber,preIndex;

  std::string particleLocation="/Event"
    +spill[ispill]+"/"+LHCb::MCParticleLocation::Default;
  debug()<<"string "<<particleLocation<<endreq;
  
  SmartDataPtr<LHCb::MCParticles> particlePointer
    ( eventSvc(),particleLocation  );
  LHCb::MCParticles::iterator itParticle;
  for(itParticle=particlePointer->begin();
      itParticle<particlePointer->end();itParticle++){
  }
  int numberOfParticles=0;
  numberOfParticles=(*(particlePointer->end()-1))->key()+1;
  debug()<<" --- original number of particles "
         << numberOfParticles<<endreq;
  //create an dimension appropriate vector
  std::vector<ParticleInfo*> particleInfo(numberOfParticles);
  //loop un hits  
  

  for(int container=0; container<1;container++){				
    std::string path="/Event"+spill[ispill]+LHCb::MCHitLocation::Muon;        
    SmartDataPtr<LHCb::MCHits> hitPointer(eventSvc(),path);
    LHCb::MCHits::const_iterator iter;	 
    preGap=-1;
    preIndex=-1;
    preChamber=-1;      
    if(hitPointer!=0){
      for (iter=(hitPointer)->begin();iter<(hitPointer)->end();iter++)
      {
        //correggere con il pezzo che ci dara' Gaia
        station=0;
        region=0;
          gap=0;
        chamber=0+chamberOffset(station,region);   
        index=(*iter)->mcParticle()->key();
        if(station==1){
          debug()<<" index, chamber, gap "<<index<<" "<<
          chamber<<" "<<gap<<endreq;
          debug()<<" index "<<index<<" in position "<<
            (*iter)->entry().x()<<" "<<
            (*iter)->entry().y()<<" "<<
            (*iter)->entry().z()<<" "<<
            " out position  "<<            
            (*iter)->exit().x()<<" "<<
            (*iter)->exit().y()<<" "<<
          (*iter)->exit().z()<<endreq;
        }
        
        if(station==1)
          debug()<<" gap cha ind "<<preGap<<" "<<preChamber<<
            " "<<preIndex<<endreq;
        
        if(particleInfo[index]){
          if(chamber!=preChamber||gap!=preGap||index!=preIndex)
            particleInfo[index]->setHitIn(station,gap,chamber);            
        }
        else{
          ParticleInfo* tmpPointer;
          if(!first){                   
            tmpPointer=                 
              new ParticleInfo((*iter)->mcParticle());
          }else{                
            tmpPointer=
            new ParticleInfo((*iter)->mcParticle(),
                             m_stationNumber,m_gaps);
            first=false;
          }
          particleInfo[index]=tmpPointer;
          particleInfo[index]->setHitIn(station,gap,chamber);
        }              
        preGap=gap;
        preIndex=index;
        preChamber=chamber;
      }       
    }      
  }    
  int partCollision;
  
  for(int i=0;i<collisions();i++){
    (*m_resultPointer)[i].resize(m_maxDimension);      
  }    
  std::vector<int> m_particleResult(m_maxDimension) ;    
  
  //all hits in the event have been added
  //then count the multiplicity per station  and track lenght   and delete
  std::vector<ParticleInfo*>::iterator itParticleInfo;
  for(itParticleInfo=particleInfo.begin();
      itParticleInfo<particleInfo.end();itParticleInfo++)      
  {
    if(*itParticleInfo){
      (m_particleResult)=(*itParticleInfo)->multiplicityResults(); 
      partCollision=(*itParticleInfo)->getCollision();        
      for(int i=0;i<m_maxDimension;i++){          
        ((* m_resultPointer)[partCollision])[i]= 
          ((* m_resultPointer)[partCollision])[i]
          +(m_particleResult)[i];
      }      
      delete *itParticleInfo;      
    }      
  }
  debug()<<" --- end of routine "<< endreq;  
  return StatusCode::SUCCESS;
}

StatusCode 
MuonBackground::initializeRNDDistribution1D(IHistogram1D* 
                                            histoPointer,
                                            std::vector<Rndm::Numbers*>& 
                                            distributions , 
                                            std::vector<bool>& 
                                            pointerToFlags,
                                            double & xmin, 
                                            double& xmax)
{
 
  //   MsgStream msg(msgSvc(), name());
  //  const IAxis* axis=&(histoPointer->axis());
  const IAxis& axis=(histoPointer->axis());
  int nbin=axis.bins();
  xmin=axis.lowerEdge();
  xmax=axis.upperEdge();
  std::vector<double> content(nbin);
  int total=0;
  
  for(int i=0;i<nbin;i++){
    double tmp=histoPointer->binHeight(i);
    if(tmp<0){
      debug()<<" negative value for histogragm "<<histoPointer->title()
         <<" "<<tmp<<endreq;
            tmp=0;      
    }    
    content[i]=tmp;    
    total=total+(int)histoPointer->binHeight(i);    
  }  
  Rndm::Numbers* pdf=new Rndm::Numbers; 
  //info()<<"total "<<total<<endreq;
  
  if(total==0){
    pointerToFlags.push_back(false);
  }
  else{
    pointerToFlags.push_back(true);
    pdf->initialize(randSvc(), Rndm::DefinedPdf(content,0));            
    //info()<<"extract two numbers "<<(*pdf)()<<" "<< (*pdf)()<<endreq;
    
  }  
  //info()<<"total 1 "<<total<<endreq;
  distributions.push_back(pdf);
   return StatusCode::SUCCESS; 
}


StatusCode 
MuonBackground::initializeRNDDistribution2D(IHistogram2D* histoPointer,
                                            std::vector<Rndm::Numbers*>& 
                                            distributions , std::vector<bool>& 
                                            pointerToFlags,double& xmin, 
                                            double& xmax, int& nbinx ,
                                            double &ymin, double& ymax  )
{  
  const IAxis& axisx=(histoPointer->xAxis());
  nbinx=axisx.bins();
  xmin=axisx.lowerEdge();
  xmax=axisx.upperEdge();
  const IAxis& axisy=(histoPointer->yAxis());
  //  int nbiny=axisy.bins();
  ymin=axisy.lowerEdge();
  ymax=axisy.upperEdge();
  for(int xbin=0;xbin<nbinx;xbin++){
    // create slice parallel to x axis
    IHistogram1D* ySlice = histoSvc()->histogramFactory()->
      sliceY( "MuBG/1" , *histoPointer, xbin);
    int entries=ySlice->entries();
    if(entries==0) {
      debug()<<" zero entries"<<endmsg;
    }
    initializeRNDDistribution1D(ySlice,distributions , 
                                pointerToFlags,ymin, ymax);
    histoSvc()->unregisterObject( ySlice );
    delete ySlice;
  }  
  return StatusCode::SUCCESS;
}

StatusCode MuonBackground::createHit(LHCb::MCHits** 
                                     hitsContainer,int station,
                                     int multi,int ispill)
{
  int maxTryPhi=10;
  int maxTryR=10;
  int tryR=0;
  int tryPhi=0;
  bool hitInsideCha=false;
  
  //first define all the relevant quantities .. if ok then crete the hit!
  //1) extract the radial position
  int index=station*m_gaps+multi;
  float r = 0.F;
  unsigned int chamberIndex = 0;
  unsigned int regionIndex  = 0;  
  float xpos = 0.F;
  float ypos = 0.F;
  //float zpos = 0.F;
  debug()<<"new track "<<index<<" "<<hitInsideCha<<endreq;
  DeMuonChamber* pChamber=NULL;
  while(!hitInsideCha&&tryR<maxTryR){
    debug()<<"tryR "<<endreq;
    r=(m_radial[index])->giveRND();
    //2) extract the gloabl phi 
    tryPhi=0;  
    while(!hitInsideCha&&tryPhi<maxTryPhi){
      debug()<<"tryphi "<<endreq;      
      double globalPhi = ((m_phiglobvsradial[index])->giveRND(r))*pi/180.0;
      //3) test whether the r,phi is inside the sensitive chamber volume      
      //  transform r and phi in x,y
      xpos=float(r*cos(globalPhi)*cm);
      ypos=float(r*sin(globalPhi)*cm);
      if(multi==3) 
        debug()<<r<<" "<<globalPhi<<" "<<cos(globalPhi)<<" "<<
          sin(globalPhi)<<endreq;
      if(multi==3)debug()<<"xpos, ypos "<<xpos<<" "<<ypos<<endreq;      
      pChamber=NULL;
      LHCb::MuonTileID tile; 
      debug()<<"numbers test "<<endmsg;
      int chNumber=-1;
      int regNumber=-1;
      debug()<<" sono nei pa "<<endreq;
      StatusCode sc;
      debug()<<" sono nei pa "<<m_muonDetector<<endreq;
      debug()<<"x "<<xpos<<" "<<ypos<<" "<<station<<endreq;
      sc=m_muonDetector->Pos2StChamberNumber(xpos,ypos,station,chNumber,
                                             regNumber);
      debug()<<" hit "<<xpos<<" "<<ypos<<" "<<station<<" "<<
        chNumber<<" "<<regNumber<<endmsg;
      debug()<<"sc "<<sc<<endreq;
      debug()<<"numbers test "<<chNumber<<" "<<regNumber<<endmsg;
      //ripartire  da qui alessia 
      //estraggo la camera
      //  StatusCode sc=m_pMuonTileXYZ->
      //   locateChamberTileAndGap(xpos,ypos,zpos,tile,pGasGap);
      if(sc.isFailure()){
        debug()<<" no gap found for muon hit" <<endreq;
      }else{      
        pChamber=dynamic_cast<DeMuonChamber*> 
          //DetectorElement pp
          (m_muonDetector->
           ReturnADetElement(station,regNumber,chNumber));
        chamberIndex=(unsigned int)pChamber->chamberNumber();
        regionIndex=(unsigned int)pChamber->regionNumber();
        
        //remember this is the chamber number inside a region....
        debug()<<" chamber number "<<pChamber->chamberNumber()
               <<" station number "<<pChamber->stationNumber()
               <<" region number "<<pChamber->regionNumber()<<
          "position "<<xpos<<" "<<ypos<<" "<<endreq;        
        hitInsideCha=true;
      } 
      tryPhi++;
    }
    tryR++;
  }
  if(!hitInsideCha){
    debug()<<" could not find a r.phi combination "<<endreq;
  }  else{    //define the chamber index in the total reference...
    debug()<<" ciao "<<endreq;
    //extract the hit gaps
    int allgap=(int)(m_hitgap[index])->giveRND();
    int max=8;    
    std::vector<int> gapHitTmp;
    debug()<<" gap extracted "<<allgap<<" multiplicity "<<multi<<
      " "<<index<<" "<<m_gaps<<endreq;    
    for(int i=0;i<m_gaps;i++){      
      int gap=allgap/max;      
      if(gap>1)gap=1;      
      if(gap==1)gapHitTmp.push_back(m_gaps-i-1);
      allgap=allgap-gap*max;      
      if(gap==1)
        debug()<<"allgap "<<gap<<" "<<gapHitTmp.back()<<" "<<
          max<<endreq;
      max=max/2;      
    }
    if(gapHitTmp.size()!=(unsigned int)multi+1) {
      warning()<<"problem in extraction the gaps exit "<<endreq;
      return StatusCode::SUCCESS;      
    }
    std::vector<int> gapHit;
    std::vector<int>::reverse_iterator it;
    for(it=gapHitTmp.rbegin();it<gapHitTmp.rend();it++){      
      gapHit.push_back((*it)); 
      debug()<<" gap back"<<gapHit.back()<<endreq;      
    }    
    int firstGap=gapHit[0];    
    int lastGap=gapHit[gapHit.size()-1];    
    int tryPhiLoc=0;  
    int maxTryPhiLoc=20;
    bool allHitsInsideCha=false;
    bool hitsToAdd;    
    float xSlope,ySlope;
    for(tryPhiLoc=0;tryPhiLoc<maxTryPhiLoc&&!allHitsInsideCha;tryPhiLoc++){
      allHitsInsideCha=true;      
      hitsToAdd=false;      
      double phiLoc=((m_philocvsradial[index])->giveRND(r))*pi/180.0;
      double thetaLoc=((m_thetalocvsradial[index])->giveRND(r))*pi/180.0;      
      //define the more confortable slope in x-y direction 
      if(cos(thetaLoc)!=0){        
        xSlope=float(sin(thetaLoc)*cos(phiLoc)/cos(thetaLoc));
        ySlope=float(sin(thetaLoc)*sin(phiLoc)/cos(thetaLoc));    
      }      
      else{
        xSlope = 1.0F;
        ySlope = 1.0F;
      }
      //define the z of the average gaps position
      float averageZ=0;
      StatusCode sc=calculateAverageGap(pChamber,firstGap,lastGap,xpos,ypos,
                                        averageZ);      
      for(int igap=0;igap<=multi;igap++){
        int gapNumber=gapHit[igap];        
        Gaudi::XYZPoint entryGlobal;        
        Gaudi::XYZPoint exitGlobal;        
        DeMuonGasGap* p_Gap=NULL;
        sc=calculateHitPosInGap(pChamber,gapNumber,xpos,ypos,xSlope,
                                ySlope,averageZ,entryGlobal,
                                exitGlobal,p_Gap);
        debug()<<" exit hit pos "<<sc.isSuccess()<<endmsg;        
        if(sc.isSuccess()){
          unsigned int chamberTmp=(unsigned int)p_Gap->chamberNumber();
          unsigned int regionTmp=(unsigned int)p_Gap->regionNumber();
          debug()<<chamberTmp<<" "<<regionTmp<<endmsg;
        }else{
          allHitsInsideCha=false;            
        }          
      }      
      if(allHitsInsideCha)hitsToAdd=true;      
    }    
    if(hitsToAdd){
      float scale = log(10.0F);      
      float time= exp(scale*(m_logtimevsradial[index])->giveRND(r));
      float timeBest=0;    
      //patch to extract the timein linear mode if needed other 
      //wise just transform inlinear fromlog scale
      if(time<=m_lintimevsradial[index]->giveMax()){
        timeBest=m_lintimevsradial[index]->giveRND(r);
      }else {
        timeBest=time;
      }
      int firstGap=gapHit[0];    
      int lastGap=gapHit[gapHit.size()-1];    
      float averageZ=0;
      StatusCode sc=calculateAverageGap(pChamber,firstGap,lastGap,xpos,ypos,
                                        averageZ);
      if ( sc.isFailure() )return  sc;      
      for(int igap=0;igap<=multi;igap++){
        bool correct=true;        
        int gapNumber=gapHit[igap];
        Gaudi::XYZPoint entryGlobal;        
        Gaudi::XYZPoint exitGlobal;        
        DeMuonGasGap* p_Gap=NULL;
        StatusCode sc=calculateHitPosInGap(pChamber,gapNumber,xpos,ypos,xSlope,
                                           ySlope,averageZ,entryGlobal,
                                           exitGlobal,p_Gap);
        if ( sc.isFailure() )return  sc;
        float x=(entryGlobal.x()+exitGlobal.x())/2.0F;
        float y=(entryGlobal.y()+exitGlobal.y())/2.0F;
        float z=(entryGlobal.z()+exitGlobal.z())/2.0F;
        LHCb::MCHit* pHit = new LHCb::MCHit();
        pHit->setEntry(entryGlobal);                     
        pHit->setDisplacement(exitGlobal-entryGlobal);
        double tofOfLight=(sqrt(x*x+ y*y+z*z))/300.0;
        //        float shiftOfTOF=-ispill*m_BXTime;
        if(m_type==FlatSpillover){
          float shiftOfTOF=-ispill*m_BXTime;
          pHit->setTime(timeBest+tofOfLight+shiftOfTOF); 
          debug()<<"time "<<
            timeBest+shiftOfTOF<<" spill "<<ispill<<endreq;
          
        } else{
          pHit->setTime(timeBest+tofOfLight);  
        }          
		 int 
sen=m_muonDetector->sensitiveVolumeID(Gaudi::XYZPoint(x,y,z));
                pHit->setSensDetID(sen);

        int part=pChamber->stationNumber()*4+pChamber->regionNumber();        
        debug()<<"gap , time ,position "<<gapNumber<<" "<<
          timeBest+tofOfLight<<" "<<x<<" "<<y<<" "<<endreq;
        debug()<<"chamber "<<chamberIndex	<<endreq;        
        debug()<<" correct "<<correct<<" "<<part<<endreq;        
        if(correct)(hitsContainer[part])->push_back(pHit);
        if(correct) debug()<<" inserted "<<endreq;
        if(correct){
          debug()<<multi<<endmsg;          
          debug()<<"entry position "<<pHit->entry().x()<<
            " "<<pHit->entry().y()
                <<" "<<pHit->entry().z()<<endreq;
          debug()<<"exit position "<<pHit->exit().x()<<
            " "<<pHit->exit().y()
                <<" "<<pHit->exit().z()<<endreq;          
        }        
      }     
    }else{
      warning()<<" impossible to add the requested hits "<<
        "in station "<<station<<" and multiplicity "<<multi<<endreq;
    } 
  }  
  return StatusCode::SUCCESS;
}





float MuonBackground::min(float a,float b)
{
  if(a<b){
    return a;
  }else{
    return b;
  }
}
float MuonBackground::max(float a,float b)
{
  if(a<b){
    return b;
  }else{
    return a;
  }
}


  
int  MuonBackground::howManyHit( float floatHit)
{
  int intHit=0;
  float partHit=0;
  intHit=(int)floatHit;
  partHit=floatHit-intHit;
   
  if((*m_flatDistribution)()<partHit){
    return intHit+1;
  }
  else{
    return intHit;
  }              
}



StatusCode MuonBackground::calculateHitPosInGap(DeMuonChamber* pChamber,
                                                int gapNumber,float xpos,
                                                float ypos,float xSlope,
                                                float ySlope,float zavegaps,
                                                Gaudi::XYZPoint& entryGlobal,
                                                Gaudi::XYZPoint& exitGlobal,
                                                DeMuonGasGap*& p_Gap)
{
  StatusCode sc=StatusCode::FAILURE;
  
  IDetectorElement::IDEContainer::iterator itGap;
  int countGap=0;
  p_Gap=NULL;
  
  for(itGap=(pChamber)->childBegin(); itGap<(pChamber)->childEnd(); itGap++){
    // debug()<<(*itGap)->name()<<endmsg;
    if(countGap==gapNumber){
      p_Gap=dynamic_cast<DeMuonGasGap*>(*((*itGap)->childBegin()));
      break;
    }    
    countGap++;    
  }  
  if(p_Gap!=NULL){
    const ISolid* gapSolid=(p_Gap->geometry())->lvolume()->solid();
    const SolidBox* gapBox = dynamic_cast<const SolidBox *>(gapSolid);
    float zhalfgap= gapBox->zHalfLength() ;
    float xhalfgap= gapBox->xHalfLength() ;
    float yhalfgap= gapBox->yHalfLength() ;
    debug()<<"half gap size "<< xhalfgap <<" "<<yhalfgap<<" "<<
      zhalfgap<<endmsg;    
    Gaudi::XYZPoint poslocal= 
    (p_Gap->geometry())->toLocal(Gaudi::XYZPoint(xpos,ypos,zavegaps));
//    Gaudi::XYZPoint poslocalGap= 
//      (p_Gap->geometry())->toLocal(Gaudi::XYZPoint(0,0,0));
    float zcenter=poslocal.z();
    debug()<<" inut data "<<xpos<<" "<<ypos<<" "<<gapNumber<<" "
          <<zavegaps<<" "<<endmsg;
    
    debug()<<"center gap "<<poslocal.x()<<" "<<
      poslocal.y()<<" "<<poslocal.z()<<" "<<
      zcenter<<endmsg;    
    Gaudi::Transform3D matrixToLocal= (p_Gap->geometry())->matrix();
    Gaudi::XYZPoint slopelocal=matrixToLocal*
      Gaudi::XYZPoint(xSlope,ySlope,1.0F);
    float zinf=-zhalfgap;    
    float zsup=+zhalfgap;
    debug()<<"local slopes "<<
      slopelocal.x()<<" "<<slopelocal.y()<<" "<<slopelocal.z()<<endmsg;
    
    float xentry=poslocal.x()+(zinf-zcenter)*
      (slopelocal.x()/slopelocal.z());
    float xexit=poslocal.x()+(zsup-zcenter)*
      (slopelocal.x()/slopelocal.z());
    float yentry=poslocal.y()+
      (zinf-zcenter)*(slopelocal.y()/slopelocal.z());
    float yexit=poslocal.y()+
      (zsup-zcenter)*(slopelocal.y()/slopelocal.z());

    //check that gap boundaries have not been crossed 
    
    float xmin=-xhalfgap;    
    float ymin=-yhalfgap;
    float xmax=xhalfgap;
    float ymax=yhalfgap;
    float zmin=-zhalfgap;
    float zmax=zhalfgap;
    bool correct=false;    
    if(xentry<xmin||xentry>xmax)correct=true;
    if(yentry<ymin||yentry>ymax)correct=true;
    if(xexit<xmin||xexit>xmax)correct=true;
    if(yexit<ymin||yexit>ymax)correct=true;
    debug()<<"partial  "<<xentry<<" "<<yentry<<" "
           <<xexit<<" "<<yexit<<" "<<correct<<endmsg;
    
    debug()<<" correct "<<correct<<" "<<slopelocal.x()<<" "<<
      slopelocal.y()<<endmsg;
    if(correct){
      float z1,z2;
      // float zmin,zmax;
      //then x
      if(slopelocal.x()==0)return StatusCode::FAILURE;
      z1=(xmin-poslocal.x())/(slopelocal.x()/slopelocal.z());
      z2=(xmax-poslocal.x())/(slopelocal.x()/slopelocal.z());

      float zz1=min(z1,z2);
      float zz2=max(z1,z2);
      zmin=max(zmin,zz1);
      zmax=min(zmax,zz2);      
      //then y
      if(slopelocal.y()==0)return StatusCode::FAILURE;
      z1=(xmin-poslocal.y())/(slopelocal.y()/slopelocal.z());
      z2=(xmax-poslocal.y())/(slopelocal.y()/slopelocal.z());
      zz1=min(z1,z2);
      zz2=max(z1,z2);
      zmin=max(zmin,zz1);
      zmax=min(zmax,zz2);

      if(zmin>zhalfgap||zmax<-zhalfgap)return StatusCode::FAILURE;
      
      xentry=poslocal.x()+(slopelocal.x()/slopelocal.z())*(zmin-zcenter);  
      xexit=poslocal.x()+(slopelocal.x()/slopelocal.z())*(zmax-zcenter);  
      yentry=poslocal.y()+(slopelocal.y()/slopelocal.z())*(zmin-zcenter);  
      yexit=poslocal.y()+(slopelocal.y()/slopelocal.z())*(zmax-zcenter);  
      // zentry=zmin;
      //zexit=zmax;     
    }
    
    //back to the global frame    
    entryGlobal=(p_Gap->geometry())->
      toGlobal(Gaudi::XYZPoint(xentry,yentry,zmin));
    exitGlobal=(p_Gap->geometry())->
      toGlobal(Gaudi::XYZPoint(xexit,yexit,zmax));
    return StatusCode::SUCCESS;
    
}else return sc;  
}



StatusCode MuonBackground::calculateAverageGap(DeMuonChamber* pChamber,
                                               int gapNumberStart ,
                                               int  gapNumberStop ,
                                               float xpos,float ypos,
                                               float& zaverage)
{
  StatusCode sc=StatusCode::SUCCESS;
  
  IDetectorElement::IDEContainer::iterator itGap;
  int countGap=0;
  DeMuonGasGap*  p_Gap=NULL;
  zaverage=0;
  
  for(itGap=(pChamber)->childBegin(); itGap<(pChamber)->childEnd(); itGap++){
    // debug()<<(*itGap)->name()<<endmsg;
    
    if(countGap>=gapNumberStart&&countGap<=gapNumberStop){
      p_Gap=dynamic_cast<DeMuonGasGap*>(*((*itGap)->childBegin()));
      //get 3 points to build a plane for the gap center
      Gaudi::XYZPoint point1=(p_Gap->geometry())->
        toGlobal(Gaudi::XYZPoint(0,0,0));
      Gaudi::XYZPoint point2=(p_Gap->geometry())->
        toGlobal(Gaudi::XYZPoint(1,0,0));
      Gaudi::XYZPoint point3=(p_Gap->geometry())->
        toGlobal(Gaudi::XYZPoint(0,1,0));
      Gaudi::Plane3D plane(point1,point2,point3);
      float zInterceptThisPlane=0;      
      Gaudi::XYZVector para=plane.Normal();
      if(para.Z()!=0){
        double a=para.X();
        double b=para.Y();
        double d=plane.HesseDistance();        
        zInterceptThisPlane=-(a*xpos+b*ypos+d)/(para.Z());
      }else zInterceptThisPlane=point1.z();      
      zaverage=zaverage+ zInterceptThisPlane;
    }    
    countGap++;    
  } 
  zaverage=zaverage/(gapNumberStop-gapNumberStart+1);
  
  return sc;
  
}
int MuonBackground::chamberOffset(int station,int region)
{
  return m_chamberInRegion[station*4+region];  
};


