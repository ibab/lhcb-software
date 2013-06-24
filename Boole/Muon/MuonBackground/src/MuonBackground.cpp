// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogramFactory.h"
// from detector description
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/MuonBasicGeometry.h"
#include "DetDesc/IGeometryInfo.h"
#include "Kernel/MuonTileID.h"

// Event model
#include "Event/MCHit.h"
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"
//#include "Event/BeamParameters.h"

// local
#include "MuonBackground.h"
#include "ParticleInfo.h"


//-----------------------------------------------------------------------------
// Implementation file for class : MuonBackground
//
// 2003-02-18 : Alessia Satta
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( MuonBackground )

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
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_resultPointer(0)
  , m_flatDistribution(0)
  , m_muonDetector(0) 
{
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
  declareProperty("RadialUnit" , m_unitLength=10.0 ) ;
//  declareProperty("SpilloverPathsSize", m_readSpilloverEvents=0 ) ;
  declareProperty("DebugHistos" , m_histos=false ) ;
  declareProperty("NBXFullFull" , m_BXFillFill=1296 ) ;
  m_alreadyIni=false;
}

//=============================================================================
// Destructor
//=============================================================================
MuonBackground::~MuonBackground() {
  //ricordarsi di distruggere tutte le distribuzioni e tutti i
  //mubgdistributions
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonBackground::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by  GaudiAlgorithm	
  m_muonDetector=getDet<DeMuonDetector>(DeMuonLocation::Default);
  
  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialise" << endmsg;

  // Get the application manager. Used to find the histogram persistency type
  // and to get number of spillovers from SpillOverAlg
  IAlgManager* algmgr = svc<IAlgManager>("ApplicationMgr");
  SmartIF<IProperty> algmgrProp( algmgr );
  if( !algmgrProp ) {
    return Error("Failed to locate IProperty i/f of AppMgr");
  }

  StringProperty persType;
  persType.assign( algmgrProp->getProperty("HistogramPersistency") );
  m_persType = persType.value();
  if(msgLevel(MSG::DEBUG)) 
    debug() << "Histogram persistency type is " << m_persType << endmsg;
    

  // initialize geometry based quantities as station number, 
  //gaps per station and their z position 
  m_numberOfEventsNeed=5;
  
  MuonBasicGeometry basegeometry(detSvc(),msgSvc()); 
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();  int i=0;  
  while(i<m_stationNumber){
    numsta[i]=basegeometry.getStationName(i);    
    if(msgLevel(MSG::DEBUG)) debug()<<" station "<<i<<" "<<numsta[i]<<endmsg;
    i++;    
  }
 
//  sc=toolSvc()->retrieveTool("MuonTileIDXYZ",m_pMuonTileXYZ); 
//  if(sc.isFailure())return StatusCode::FAILURE;
//  sc=toolSvc()->retrieveTool("MuonGetInfoTool",m_pGetInfo);
//  if(sc.isFailure())return StatusCode::FAILURE;
  m_partition=basegeometry.getPartitions();
  sc=initializeGeometry();
  if(sc.isFailure())return sc;
  if(msgLevel(MSG::DEBUG)) debug()<<m_maxDimension<<endmsg;
  
 
  m_correlation.resize(m_maxDimension);
  m_radial.resize(m_maxDimension);
  m_phiglobvsradial.resize(m_maxDimension);
  m_thetalocvsradial.resize(m_maxDimension);
  m_philocvsradial.resize(m_maxDimension);
  m_logtimevsradial.resize(m_maxDimension);  
  m_lintimevsradial.resize(m_maxDimension);
  m_hitgap.resize(m_maxDimension);
  if(msgLevel(MSG::DEBUG)) debug()<<" ready to initialize the parametrization "<<endmsg;
  sc=initializeParametrization();
  if(sc.isFailure())return sc;
  m_flatDistribution =new Rndm::Numbers; 
  sc=m_flatDistribution->initialize(randSvc(), Rndm::Flat(0.0,1.0));
  if(sc.isFailure())return sc;
  if(msgLevel(MSG::DEBUG)) {
    debug()<<" type input "<<m_typeOfBackground<<endmsg;
    debug()<<" safety factor "<<m_safetyFactor[0]<<
      " "<<m_safetyFactor[1]<<" "<<m_safetyFactor[2]<<" "<<
      m_safetyFactor[3]<<" "<<m_safetyFactor[4]<<endmsg;
  }
  
  if(m_typeOfBackground=="LowEnergy"){
    m_type=LowEnergy;    
  }else if(m_typeOfBackground=="FlatSpillover"){
    m_type=FlatSpillover;        
  }
  if(msgLevel(MSG::DEBUG)) debug() <<" type "<< m_type<<endmsg;

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
    info() << "The low energy background is run by default for main events plus (if any) spillover events (max 4)" <<endmsg;
    m_readSpilloverEvents = 4;

   
  }else if(m_type==FlatSpillover){
    m_readSpilloverEvents=0;
    m_luminosityFactor=m_luminosity/2.0;   
  }

  // Release interface, no longer needed  
  releaseSvc(algmgr).ignore();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonBackground::execute() {

  if(msgLevel(MSG::DEBUG)) 
    debug() << "==> Execute" << m_readSpilloverEvents<<endmsg;
  StatusCode sc;
  
  for (int ispill=0;ispill<=m_readSpilloverEvents;ispill++){    
    sc=calculateNumberOfCollision(ispill);    
    if(msgLevel(MSG::VERBOSE))  verbose()<<"spill "<<ispill<<" "<<sc<<endmsg;	
    if(sc.isFailure()){
      //      error()<<" no collision in spill "<<ispill<<endmsg;
      
      return sc;
    }
    
    if(!collisions()) continue;
    if(msgLevel(MSG::VERBOSE))  verbose() << "==> collision " << collisions()<<endmsg; 
    LHCb::MCHits* hitsContainer=new LHCb::MCHits();    
    m_resultPointer = new std::vector<std::vector<int> > (collisions()) ;
    
    if(m_type==LowEnergy){
      sc=calculateStartingNumberOfHits(ispill);    
      if(sc.isFailure())return sc;
      for(int coll=0;coll<collisions();coll++){      
        for(int station=0;station<m_stationNumber;station++){        
          for (int multi=0;multi<m_gaps;multi++){
            int index=station*m_gaps+multi;
            int startingHits=((*m_resultPointer)[coll])[index];
            if(msgLevel(MSG::VERBOSE))  verbose() <<"station safe start end hits "
                                                  <<startingHits<<endmsg;
            
            // extract number of hits to be added
            int hitToAdd=0;
            float floatHit = 0;          
            if(numsta[station]=="M1"){
              floatHit=float(startingHits*(m_safetyFactor[station]));   
              hitToAdd=howManyHit( floatHit);            
            } else{
              //debug()<<"adding "<<	index <<" "<<startingHits<<endmsg;
              int yy=(int)(m_correlation[index])->
                giveRND(startingHits+0.5F);
              
              floatHit=float(m_safetyFactor[station]*(yy));
              hitToAdd=howManyHit( floatHit);
              //debug()<<"station safe start end hits "<<
              //  station<< " "<<m_safetyFactor[station]<< " "<<
              //  yy<< " "<< hitToAdd<<" "<<startingHits<<endmsg;
              
              if(msgLevel(MSG::VERBOSE))  verbose()<<"adding "<< hitToAdd<<" to orginal "
                                                   <<startingHits<<" in station "<<station
                                                   <<" for multiplicity "<<multi
                                                   <<" and collisions/spill "<<coll
                                                   <<" "<<ispill<<endmsg;
            }            
            
            for(int hitID=0;hitID< hitToAdd;hitID++){            
              StatusCode asc=createHit(hitsContainer, station,multi,ispill);
              if(asc.isFailure()){
                warning()<<" error in creating hit "<<endmsg;
              }   
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
      // value of nu in the evnts
      if(!m_alreadyIni){
        if(msgLevel(MSG::DEBUG)) 
          debug()<<" Initializing the luminosity for flast spillover simulation "<<endmsg;
//        LHCb::BeamParameters* beam=get<LHCb::BeamParameters>(LHCb::BeamParametersLocation::Default);
        LHCb::GenHeader* beam=get<LHCb::GenHeader>(LHCb::GenHeaderLocation::Default);
        if(beam){
//          info()<<" beam nu "<<beam->nu()<<" "
          if(msgLevel(MSG::VERBOSE))  
            verbose()<<"luminosity "<<beam->luminosity()<<" "<<m_BXFillFill<<endmsg;
          m_luminosityFactor=(beam->luminosity()*m_BXFillFill)/(2.0e+32/(Gaudi::Units::cm2*Gaudi::Units::s));
          info()<<" luminosity factor "<<m_luminosityFactor<<" compared to default 2*10^32 cm-2 s-1 "<<endmsg;
          m_alreadyIni=true;
        }else{
          info()<<"could not access beam parameter information "<<endmsg;

        }
      }   
      //for flat spillover the spill events have no meaning....
      //thus take care of deleting allocated menory
      if(ispill>0) {
        delete m_resultPointer;
        delete hitsContainer;
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
            if(msgLevel(MSG::VERBOSE))  verbose()<<"adding "<< hitToAdd
                                                 <<" in station "<<station
                                                 <<" for multiplicity "<<multi<<" and spill"
                                                 <<fspill<<endmsg;
            for(int hitID=0;hitID< hitToAdd;hitID++){            
              StatusCode asc=createHit(hitsContainer, station,multi,fspill);   
              if(asc.isFailure() && msgLevel(MSG::DEBUG)) debug()<<"failing hit creation "<<endmsg;
            }         
          }          
        }        
      }
    }
    std::string path="/Event"+spill[ispill]+"/MC/Muon/"+m_containerName;
    if(msgLevel(MSG::DEBUG)) {
      debug()<<" starting saveing the container "<<path<<endmsg;    
      debug()<<" number of total hit added "<< hitsContainer->size()<<endmsg;
    }    
    sc=eventSvc()->registerObject(path,
                               hitsContainer);    
    if(sc.isFailure())debug()<<" error registering object "<<endmsg;
    delete m_resultPointer;
    m_vertexList.clear();
    
  }
  
  return StatusCode::SUCCESS;
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonBackground::finalize() {

  if(msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  delete m_flatDistribution;
  for (int i=0;i<m_maxDimension;i++){
    MuBgDistribution* pointDelete=m_correlation[i];    
    if(pointDelete)delete pointDelete;
    pointDelete=m_radial[i];
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
  return GaudiAlgorithm::finalize();
}


//=============================================================================
//=============================================================================
//  GeometryInitialization
//=============================================================================
StatusCode MuonBackground::initializeGeometry() {
  if(msgLevel(MSG::DEBUG)) debug() << "==> initializeGeometry" << endmsg;
  int gap=0;  
  for(int i=0;i< (int)m_partition;i++){
    int stat=i/4;
    int reg=i-stat*4;
    gap=(gap>=(int)m_muonDetector->gapsInRegion(stat,reg))?
      gap:m_muonDetector->gapsInRegion(stat,reg);    
    if(msgLevel(MSG::DEBUG)) debug() << gap<<endmsg;
    
  }
  
  m_gaps=gap;
  m_maxDimension=m_gaps*m_stationNumber;
  if(msgLevel(MSG::DEBUG)) debug()<<m_gaps<<" "<<m_stationNumber<<endmsg;
  




  
  //  containerOfFirstGapPosition.resize(m_stationNumber+1);
  int chamber=0;
  
  for(int station=0;station<m_stationNumber;station++){
    for(int region=0;region<4;region++){
      m_chamberInRegion[station*4+region]=chamber;
      chamber=chamber+m_muonDetector->chamberInRegion(station,region);    
      if(msgLevel(MSG::DEBUG)) debug() <<" chamber "<<chamber<<endmsg;
      
    }    
  }
  if(msgLevel(MSG::DEBUG)) debug()<<"exit"<<endmsg;
  
  return StatusCode::SUCCESS;
}

StatusCode MuonBackground::initializeParametrization()
{
  int numName=m_histoName.size();
  int numCode=m_histogramsMapNumber.size();
  int code=0;
  //  int gap;  
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
    //    gap=m_numberOfGaps[station*m_gaps];
    for(int mult=0;mult<m_gaps;mult++){
      int index=station*m_gaps+mult;      
      for(int i=0;i<numCode;i++)
        {
          char codePath[10];          
          code=m_histogramsMapNumber[i];
          name=m_histoName[i];
          int tt=mapHistoCode(code,station,mult);
          if( msgLevel(MSG::VERBOSE) )
            verbose() << code <<" "<< station <<" "<< mult <<" "<< tt << endmsg;
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
              StatusCode asc=initializeRNDDistribution1D(histo1d,
                                          distributions ,pointerToFlags,xmin, 
                                          xmax);
              if(asc.isFailure())return asc;
              
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
              warning() <<"not found the 1D  histo " << path << endmsg;
            }
          }            
          else if (m_histogramsDimension[i]==2){
            SmartDataPtr<IHistogram2D> histo2d(histoSvc(),path);
            if(histo2d){
              //   histoPointer=histoin2;
              if( msgLevel(MSG::VERBOSE) )
                debug() << "found the 2D histo " << path << endmsg;
              std::vector<Rndm::Numbers*>  distributions;
              double xmin,xmax,ymin,ymax;
              int nbinx;              
              std::vector<bool>   pointerToFlags;
              StatusCode asc=initializeRNDDistribution2D(histo2d,
                                         distributions ,pointerToFlags,xmin, 
                                          xmax, nbinx ,ymin, ymax);
              if(asc.isFailure())return asc;
              
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
              warning() << "not found the 2D-histo " << path << endmsg;
             }
            }
          
        }      
    }
  }
 return StatusCode::SUCCESS;
}


//=============================================================================
StatusCode MuonBackground::calculateNumberOfCollision(int ispill) {
  if(msgLevel(MSG::DEBUG)) debug() << "==> calculateNumberOfCollsion " << endmsg;
  int collision=0;
  std::string collisionLocation="/Event"
    +spill[ispill]+"/"+LHCb::GenCollisionLocation::Default;
  if(msgLevel(MSG::VERBOSE)) {
    verbose()<<"string "<<collisionLocation<<endmsg;
    verbose()<<"spill "<<ispill<<endmsg;
  }

  SmartDataPtr<LHCb::GenCollisions> collisionsPointer
    ( eventSvc(), collisionLocation );
  LHCb::GenCollisions::iterator itCollision;
  collision=0;  
  if(collisionsPointer){    
    for(itCollision=collisionsPointer->begin();
        itCollision<collisionsPointer->end();itCollision++){
      if(msgLevel(MSG::VERBOSE)) verbose()<<" --- collision number "
                                          << (*itCollision)->key()<<endmsg;
      collision++;      
    }
  }else{
    //     return StatusCode::FAILURE; 
  }
  
  setCollsionNumber(collision);
  if(msgLevel(MSG::DEBUG)) debug()<<" --- total collision number "<<collision<<endmsg;  
  return StatusCode::SUCCESS; 
}


StatusCode MuonBackground::calculateStartingNumberOfHits(int ispill) {

  if(msgLevel(MSG::DEBUG)) debug() << "==> calculateStartingNumberOfHit " << endmsg;
  bool first=true;
  int gap,chamber,index,station,region;
  int preGap,preChamber,preIndex;

  std::string particleLocation="/Event"
    +spill[ispill]+"/"+LHCb::MCParticleLocation::Default;  
  SmartDataPtr<LHCb::MCParticles> particlePointer
    ( eventSvc(),particleLocation  );
  //  LHCb::MCParticles::iterator itParticle;
  // for(itParticle=particlePointer->begin();
  //    itParticle<particlePointer->end();itParticle++){
  // }
  int numberOfParticles=0;
  if(particlePointer){
    
    numberOfParticles=(*(particlePointer->end()-1))->key()+1;
    if(msgLevel(MSG::DEBUG)) debug()<<" --- original number of particles "
                                    << numberOfParticles<<endmsg;
    //create an dimension appropriate vector
    std::vector<ParticleInfo*> particleInfo(numberOfParticles);
    //loop un hits
    for(int container=0; container<1;container++){				
      std::string path="/Event"+spill[ispill]+"/"+LHCb::MCHitLocation::Muon;        
      SmartDataPtr<LHCb::MCHits> hitPointer(eventSvc(),path);
      if(msgLevel(MSG::VERBOSE)) {       
        verbose()<<" container in path "<<path<<" "<<endmsg;
        if(hitPointer!=0)verbose()<<"found "<<endmsg;
        else verbose()<<" not found "<<endmsg;
      }

      LHCb::MCHits::const_iterator iter;	 
      preGap=-1;
      preIndex=-1;
      preChamber=-1;      
      if(hitPointer!=0){
        for (iter=(hitPointer)->begin();iter<(hitPointer)->end();iter++)
      {
        int det=(*iter)->sensDetID();
        if(det<0)continue;
        
        station=m_muonDetector->stationID(det);
        region=m_muonDetector->regionID(det);
        gap=m_muonDetector->gapID(det);;
        chamber=m_muonDetector->chamberID(det)+
          chamberOffset(station,region);   
        index=(*iter)->mcParticle()->key();
        if(msgLevel(MSG::VERBOSE)) {
          verbose()<<" index, chamber, gap "<<index<<" "
                   <<chamber<<" "<<gap<<endmsg;
          verbose()<<" index "<<index<<" in position "<<
            (*iter)->entry().x()<<" "<<
            (*iter)->entry().y()<<" "<<
            (*iter)->entry().z()<<" "<<
            " out position  "<<            
            (*iter)->exit().x()<<" "<<
            (*iter)->exit().y()<<" "<<
            (*iter)->exit().z()<<endmsg;
        }
        const LHCb::MCVertex* pointVertex= (*iter)->mcParticle()->primaryVertex() ;
        int collNumber=numberOfCollision(pointVertex);
        if(msgLevel(MSG::DEBUG)) debug()<<" collNumber "<<collNumber<<endmsg;
        if(collNumber>=collisions()){
          error()<<
            " problem with input data inconsistency in collsions number "<<
            endmsg;  
          return StatusCode::FAILURE;
          
        }
        
        if(particleInfo[index]){
          
          
          if(chamber!=preChamber||gap!=preGap||index!=preIndex)
            particleInfo[index]->setHitIn(station,gap,chamber);   
          
        }
        else{
          ParticleInfo* tmpPointer;
  
          if(!first){                   
            tmpPointer=                 
              new ParticleInfo((*iter)->mcParticle(),collNumber);
          }else{                
            tmpPointer=
              new ParticleInfo((*iter)->mcParticle(),
                               m_stationNumber,m_gaps,collNumber);
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
      itParticleInfo<particleInfo.end();itParticleInfo++){
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
  
  }
  

  if(msgLevel(MSG::DEBUG)) debug()<<" --- end of routine "<< endmsg;  
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
      if(msgLevel(MSG::VERBOSE)) verbose() << " negative value for histogram "
                                           << histoPointer->title()
                                           << " " << tmp << endmsg;
      tmp=0;
    }    
    content[i]=tmp;    
    total=total+(int)histoPointer->binHeight(i);    
  }  
  Rndm::Numbers* pdf=new Rndm::Numbers; 
  if(msgLevel(MSG::VERBOSE)) verbose()<<"total "<<total<<endmsg;
  
  if(total<=0){
    pointerToFlags.push_back(false);
  }
  else{
    pointerToFlags.push_back(true);
    StatusCode sc=pdf->initialize(randSvc(), Rndm::DefinedPdf(content,0));   
    if(sc.isFailure())return sc;
    
    //info()<<"extract two numbers "<<(*pdf)()<<" "<< (*pdf)()<<endmsg;
    
  }  
  //info()<<"total 1 "<<total<<endmsg;
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
    if( msgLevel(MSG::VERBOSE) && entries <= 0 ) {
      verbose()<<" zero entries"<<endmsg;
    }
    StatusCode sc=initializeRNDDistribution1D(ySlice,distributions , 
                                pointerToFlags,ymin, ymax);
    if(sc.isFailure())return sc;    
    sc=histoSvc()->unregisterObject( ySlice );
    if(sc.isFailure()){
     warning()<<" problem in releasing the histo "<<endmsg;
    }
    
    delete ySlice;
  }  
  return StatusCode::SUCCESS;
}

StatusCode MuonBackground::createHit(LHCb::MCHits* 
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
  float xpos = 0.F;
  float ypos = 0.F;
  //float zpos = 0.F;
  if(msgLevel(MSG::VERBOSE)) verbose()<<"new track "<<index<<" "<<hitInsideCha<<endmsg;
  DeMuonChamber* pChamber=NULL;
  while(!hitInsideCha&&tryR<maxTryR){
    if(msgLevel(MSG::VERBOSE)) verbose()<<"tryR "<<endmsg;
    r=(m_radial[index])->giveRND();
    //2) extract the gloabl phi 
    tryPhi=0;  
    while(!hitInsideCha&&tryPhi<maxTryPhi){
      if(msgLevel(MSG::VERBOSE)) verbose()<<"tryphi "<<endmsg;      
      double globalPhi = ((m_phiglobvsradial[index])->giveRND(r))*Gaudi::Units::pi/180.0;
      //3) test whether the r,phi is inside the sensitive chamber volume      
      //  transform r and phi in x,y
      xpos=float(r*cos(globalPhi)*Gaudi::Units::mm*m_unitLength);
      ypos=float(r*sin(globalPhi)*Gaudi::Units::mm*m_unitLength);
      pChamber=NULL;
      LHCb::MuonTileID tile;
      int chNumber=-1;
      int regNumber=-1;
      StatusCode sc;
      if(msgLevel(MSG::VERBOSE)) verbose()<<"x "<<xpos<<" "<<ypos<<" "<<station<<endmsg;
      sc=m_muonDetector->Pos2StChamberNumber(xpos,ypos,station,chNumber,
                                             regNumber);
      if(msgLevel(MSG::VERBOSE)) verbose()<<" hit "<<xpos<<" "<<ypos<<" "<<station<<" "<<
        chNumber<<" "<<regNumber<<endmsg;
      if(sc.isFailure()){
        if(msgLevel(MSG::DEBUG)) debug()<<" no chamber found for muon hit" <<endmsg;
      }else{
        sc=StatusCode::FAILURE;
        //check n ot only that hit is inside chamber but also gap...
         pChamber=dynamic_cast<DeMuonChamber*>(m_muonDetector->
           getChmbPtr(station,regNumber,chNumber));       
         Gaudi::XYZPoint pToCheck=pChamber->geometry()->toGlobal(Gaudi::XYZPoint(0,0,0));
         if(msgLevel(MSG::DEBUG)) debug()<<"to check "<<pToCheck<<endmsg;
         IPVolume* firstGap=pChamber->getFirstGasGap();
         IPVolume* firstGapLayer=pChamber->getGasGapLayer(0);
         bool isIn=false;
         
         
         Gaudi::XYZPoint pInMother=
           firstGap->toMother(Gaudi::XYZPoint(0,0,0));
         Gaudi::XYZPoint pInChamber=
           firstGapLayer->toMother(pInMother);
           if(msgLevel(MSG::DEBUG)) debug()<< pInChamber<<" p in chamber "<<endmsg; 
         Gaudi::XYZPoint pInGlobal=(pChamber->geometry())->toGlobal(pInChamber);
         //Gaudi::XYZPoint pInGlobal=(pChamber->geometry())->toGlobal(pInMother);
         
	 
         Gaudi::XYZPoint point(xpos,ypos,0);
         if(msgLevel(MSG::DEBUG)) debug()<< point<<" "<<pInGlobal.z()<<endmsg;
         point.SetZ(pInGlobal.z());          
         Gaudi::XYZPoint pointInChamber=(pChamber->geometry())->toLocal(point);
         Gaudi::XYZPoint pointInLayer=firstGapLayer->toLocal(pointInChamber);
         
         isIn = firstGap->isInside(pointInLayer);
         if(isIn)sc=StatusCode::SUCCESS;

          
      }
       if(sc.isFailure()){
         if(msgLevel(MSG::DEBUG)) debug()<<" no gap found for muon hit" <<endmsg;
      }else{      
      
        pChamber=dynamic_cast<DeMuonChamber*>(m_muonDetector->
           getChmbPtr(station,regNumber,chNumber));
        chamberIndex=(unsigned int)pChamber->chamberNumber();
        // unsigned int regionIndex=(unsigned int)pChamber->regionNumber(); // never used
        
        //remember this is the chamber number inside a region....
        if(msgLevel(MSG::DEBUG)) debug()<<" chamber number "<<pChamber->chamberNumber()
                                        <<" station number "<<pChamber->stationNumber()
                                        <<" region number "<<pChamber->regionNumber()
                                        <<" position "<<xpos<<" "<<ypos<<" "<<endmsg;        
        hitInsideCha=true;
      } 
      tryPhi++;
    }
    tryR++;
  }
  if(!hitInsideCha){
    if(msgLevel(MSG::DEBUG)) debug()<<" could not find a r.phi combination "<<endmsg;
  }else{
    //define the chamber index in the total reference...
    //extract the hit gaps
    int allgap=(int)(m_hitgap[index])->giveRND();
    int max=8;    
    std::vector<int> gapHitTmp;
    if(msgLevel(MSG::DEBUG)) debug()<<" gap extracted "<<allgap<<" multiplicity "<<multi
                                    <<" "<<index<<" "<<m_gaps<<endmsg;    
    for(int i=0;i<m_gaps;i++){      
      int gap=allgap/max;      
      if(gap>1)gap=1;      
      if(gap==1)gapHitTmp.push_back(m_gaps-i-1);
      allgap=allgap-gap*max;     
      //verbose()<<"allgap "<<gap<<" "<<gapHitTmp.back()<<" "<<
      //  max<<endmsg;
      max=max/2;      
    }
    if(gapHitTmp.size()!=(unsigned int)multi+1) {
      warning()<<"problem in extraction the gaps exit "<<endmsg;
      return StatusCode::SUCCESS;      
    }
    std::vector<int> gapHit;
    std::vector<int>::reverse_iterator it;
    for(it=gapHitTmp.rbegin();it<gapHitTmp.rend();it++){      
      gapHit.push_back((*it)); 
      if(msgLevel(MSG::VERBOSE)) verbose()<<" gap back"<<gapHit.back()<<endmsg;      
    }    
    int firstGap=gapHit[0];    
    int lastGap=gapHit[gapHit.size()-1];    
    int tryPhiLoc=0;  
    int maxTryPhiLoc=20;
    bool allHitsInsideCha=false;
    bool hitsToAdd=false;    
    float xSlope,ySlope;
    for(tryPhiLoc=0;tryPhiLoc<maxTryPhiLoc&&!allHitsInsideCha;tryPhiLoc++){
      allHitsInsideCha=true;      
      hitsToAdd=false;      
      double phiLoc=((m_philocvsradial[index])->giveRND(r))*Gaudi::Units::pi/180.0;
      double thetaLoc=((m_thetalocvsradial[index])->giveRND(r))*Gaudi::Units::pi/180.0;      
      //define the more confortable slope in x-y direction 
      if(cos(thetaLoc)!=0){        
        xSlope=float(sin(thetaLoc)*cos(phiLoc)/cos(thetaLoc));
        ySlope=float(sin(thetaLoc)*sin(phiLoc)/cos(thetaLoc));    
      }      
      else{
        xSlope = 1.0F;
        ySlope = 1.0F;
      }
      if(msgLevel(MSG::VERBOSE)) verbose()<<" local slope "<<xSlope<<" "<<ySlope<<endmsg;
      //define the z of the average gaps position
      float averageZ=0;
      StatusCode sc=calculateAverageGap(pChamber,firstGap,lastGap,xpos,ypos,
                                        averageZ);      
      if(msgLevel(MSG::VERBOSE)) verbose()<<sc<<" "<<averageZ<<endmsg;
      for(int igap=0;igap<=multi;igap++){
        int gapNumber=gapHit[igap];        
        Gaudi::XYZPoint entryGlobal;        
        Gaudi::XYZPoint exitGlobal;        
        //        DeMuonGasGap* p_Gap=NULL;
        sc=calculateHitPosInGap(pChamber,gapNumber,xpos,ypos,xSlope,
                                ySlope,averageZ,entryGlobal,
                                exitGlobal);
        if(msgLevel(MSG::DEBUG)) debug()<<"status code of calhitpos "<<sc<<endmsg;
        if(sc.isSuccess()){

          if(msgLevel(MSG::DEBUG)) debug()<<"found hit in chamber  "<<endmsg;
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
      int _firstGap=gapHit[0];    
      int _lastGap=gapHit[gapHit.size()-1];    
      float averageZ=0;
      StatusCode sc=calculateAverageGap(pChamber,_firstGap,_lastGap,xpos,ypos,
                                        averageZ);
      if ( sc.isFailure() )return  sc;      
      for(int igap=0;igap<=multi;igap++){
        bool correct=true;        
        int gapNumber=gapHit[igap];
        Gaudi::XYZPoint entryGlobal;        
        Gaudi::XYZPoint exitGlobal;        
        
        StatusCode _sc=calculateHitPosInGap(pChamber,gapNumber,xpos,ypos,xSlope,
                                            ySlope,averageZ,entryGlobal,
                                            exitGlobal);
        if ( _sc.isFailure() )return  _sc;
        double x=(entryGlobal.x()+exitGlobal.x())/2.0;
        double y=(entryGlobal.y()+exitGlobal.y())/2.0;
        double z=(entryGlobal.z()+exitGlobal.z())/2.0;
        LHCb::MCHit* pHit = new LHCb::MCHit();
        pHit->setEntry(entryGlobal);                     
        pHit->setDisplacement(exitGlobal-entryGlobal);
        double tofOfLight=(sqrt(x*x+ y*y+z*z))/300.0;
        if(m_type==FlatSpillover){
          float shiftOfTOF=-ispill*m_BXTime;
          pHit->setTime(timeBest+tofOfLight+shiftOfTOF); 
          if(msgLevel(MSG::VERBOSE))  
            verbose()<<"time "<<timeBest+shiftOfTOF<<" spill "<<ispill<<endmsg;
        } else{
          pHit->setTime(timeBest+tofOfLight);  
        }
        if(msgLevel(MSG::DEBUG)) debug()<<" mid point "<<x<<" "<<y<<" "<<z<<endmsg;
        
        
        int sen=m_muonDetector->sensitiveVolumeID(Gaudi::XYZPoint(x,y,z));
        if(msgLevel(MSG::DEBUG)) debug()<<" the volume ID is "<<sen<<endmsg;     
        pHit->setSensDetID(sen);
        //  int part=pChamber->stationNumber()*4+pChamber->regionNumber();
        if(msgLevel(MSG::DEBUG)) debug()<<"gap, time, position "<<gapNumber<<" "<<chamberIndex<<" "
                                        <<timeBest+tofOfLight<<" "<<x<<" "<<y<<" "<<endmsg;
        if(correct){
          (hitsContainer)->push_back(pHit);
          if(msgLevel(MSG::DEBUG)) {
            debug()<<" hits has been inserted "<<endmsg;
            debug()<<" multiplicity "<<multi<<endmsg;          
            debug()<<"entry position "<<pHit->entry().x()
                   <<" "<<pHit->entry().y()
                   <<" "<<pHit->entry().z()<<endmsg;
            debug()<<"exit position "<<pHit->exit().x()
                   <<" "<<pHit->exit().y()
                   <<" "<<pHit->exit().z()<<endmsg;
          }          
        }        
      }     
    }else{
      if(msgLevel(MSG::DEBUG)) debug() << " impossible to add the requested hits in station "
                                       << station << " and multiplicity " <<multi << endmsg;
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
                                                Gaudi::XYZPoint& exitGlobal)
{
  //StatusCode sc=StatusCode::FAILURE;  
//   IDetectorElement::IDEContainer::iterator itGap;
//   int countGap=0;
//   p_Gap=NULL;
//   for(itGap=(pChamber)->childBegin(); itGap<(pChamber)->childEnd(); itGap++){
//     if(countGap==gapNumber){
//       p_Gap=dynamic_cast<DeMuonGasGap*>(*((*itGap)->childBegin()));
//       break;
//     }    
//     countGap++;    
//   } 
    IPVolume* p_Gap=pChamber->getGasGap(gapNumber);
  IPVolume* p_GapLayer=pChamber->getGasGapLayer(gapNumber);


  if(p_Gap!=NULL){  
    const ISolid* gapSolid=(p_Gap)->lvolume()->solid();
    const SolidBox* gapBox = dynamic_cast<const SolidBox *>(gapSolid);
    const double zhalfgap= gapBox->zHalfLength() ;
    const double xhalfgap= gapBox->xHalfLength() ;
    const double yhalfgap= gapBox->yHalfLength() ;
//    verbose()<<"half gap size "<<xhalfgap <<" "<<yhalfgap<<" "<<zhalfgap
//           <<endmsg;    
    
//    verbose()<<"glob gap pos "<<gapcc.x()<<" "<<gapcc.y()<<" "
//                <<gapcc.z()<<endmsg;
                
        Gaudi::XYZPoint loch=pChamber->geometry()->toLocal(Gaudi::XYZPoint(xpos,ypos,zavegaps));
    Gaudi::XYZPoint logaslayer=p_GapLayer->toLocal(loch);
    Gaudi::XYZPoint poslocal=p_Gap->toLocal(logaslayer);

    const double zcenter=poslocal.z();
//    verbose()<<" input data "<<xpos<<" "<<ypos<<" "<<gapNumber<<" "
//          <<zavegaps<<" "<<endmsg;    
//    verbose()<<"center gap "<<poslocal.x()<<" "<<poslocal.y()<<" "
//             <<poslocal.z()<<" "<<zcenter<<endmsg; 
    //Gaudi::Transform3D matrixToLocal= (p_Gap->geometry())->matrix();
    //Gaudi::XYZPoint slopelocal=matrixToLocal*
    Gaudi::XYZPoint slopelocal=Gaudi::XYZPoint(xSlope,ySlope,1.0F);
    const double zinf=-zhalfgap;    
    const double zsup=+zhalfgap;
//    verbose()<<"local slopes "<<
//      slopelocal.x()<<" "<<slopelocal.y()<<" "<<slopelocal.z()<<endmsg;    
    double xentry=poslocal.x()+(zinf-zcenter)*
      (slopelocal.x()/slopelocal.z());
    double xexit=poslocal.x()+(zsup-zcenter)*
      (slopelocal.x()/slopelocal.z());
    double yentry=poslocal.y()+
      (zinf-zcenter)*(slopelocal.y()/slopelocal.z());
    double yexit=poslocal.y()+
      (zsup-zcenter)*(slopelocal.y()/slopelocal.z());

    //check that gap boundaries have not been crossed
    const double xmin=-xhalfgap;    
    const double ymin=-yhalfgap;
    const double xmax=xhalfgap;
    const double ymax=yhalfgap;
    double zmin=-zhalfgap;
    double zmax=zhalfgap;
    bool correct=true;    
    if(xentry<xmin||xentry>xmax)correct=false;
    if(yentry<ymin||yentry>ymax)correct=false;
    if(xexit<xmin||xexit>xmax)correct=false;
    if(yexit<ymin||yexit>ymax)correct=false;
//    verbose()<<"partial  "<<xentry<<" "<<yentry<<" "
//           <<xexit<<" "<<yexit<<" "<<correct<<endmsg;    
//    verbose()<<" correct "<<correct<<" "<<slopelocal.x()<<" "<<
//      slopelocal.y()<<endmsg;
    if(correct){
      //then x
      if(slopelocal.x()==0)return StatusCode::FAILURE;
      double z1=(xmin-poslocal.x())/(slopelocal.x()/slopelocal.z());
      double z2=(xmax-poslocal.x())/(slopelocal.x()/slopelocal.z());
	  double zz1=std::min(z1,z2);
	  double zz2=std::max(z1,z2);
	  zmin=std::max(zmin,zz1);
	  zmax=std::min(zmax,zz2);      
      //then y
      if(slopelocal.y()==0)return StatusCode::FAILURE;
      z1=(xmin-poslocal.y())/(slopelocal.y()/slopelocal.z());
      z2=(xmax-poslocal.y())/(slopelocal.y()/slopelocal.z());
	  zz1=std::min(z1,z2);
	  zz2=std::max(z1,z2);
	  zmin=std::max(zmin,zz1);
	  zmax=std::min(zmax,zz2);
      if(zmin>zhalfgap||zmax<-zhalfgap)return StatusCode::FAILURE;      
      xentry=poslocal.x()+(slopelocal.x()/slopelocal.z())*(zmin-zcenter);  
      xexit=poslocal.x()+(slopelocal.x()/slopelocal.z())*(zmax-zcenter);  
      yentry=poslocal.y()+(slopelocal.y()/slopelocal.z())*(zmin-zcenter);  
      yexit=poslocal.y()+(slopelocal.y()/slopelocal.z())*(zmax-zcenter);
      //back to the global frame    
//      verbose()<<xentry<<" "<<yentry<<" "<<zmin<<endmsg;
//      verbose()<<xexit<<" "<<yexit<<" "<<zmax<<endmsg;      
      Gaudi::XYZPoint entryInLayer=p_Gap->toMother(Gaudi::XYZPoint(xentry,yentry,zmin));
      Gaudi::XYZPoint entryInCh=p_GapLayer->toMother(entryInLayer);
    
      entryGlobal=(pChamber->geometry())->
	toGlobal(entryInCh);
      Gaudi::XYZPoint exitInLayer=
	p_Gap->toMother((Gaudi::XYZPoint(xexit,yexit,zmax)));
      Gaudi::XYZPoint exitInCh=p_GapLayer->toMother(exitInLayer);

      exitGlobal=(pChamber->geometry())->
        toGlobal(exitInCh);

//      verbose()<<"global "<<entryGlobal.x()<<" "<<entryGlobal.y()<<" "<<entryGlobal.z()<<endmsg;
//      verbose()<<"global "<<exitGlobal.x()<<" "<<exitGlobal.y()<<" "<<exitGlobal.z()<<endmsg;
      
      return StatusCode::SUCCESS;
    }else return StatusCode::FAILURE;    
  }else return StatusCode::FAILURE;  
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

  zaverage=0;  
  for(int i=gapNumberStart;i<=gapNumberStop;i++){
    IPVolume* pGapLayer=(pChamber)->getGasGapLayer(i);
    IPVolume* pGap=(pChamber)->getGasGap(i);
    //get 3 points to build a plane for the gap center
    Gaudi::XYZPoint point1=pChamber->geometry()->
      toGlobal(pGapLayer->
	       toMother(pGap->toMother(Gaudi::XYZPoint(0.,0.,0.))));
    Gaudi::XYZPoint point2=pChamber->geometry()->
      toGlobal(pGapLayer->
	       toMother(pGap->toMother(Gaudi::XYZPoint(1.,0.,0.))));
    Gaudi::XYZPoint point3=pChamber->geometry()->
      toGlobal(pGapLayer->
	       toMother(pGap->toMother(Gaudi::XYZPoint(0.,1.,0.))));
    Gaudi::Plane3D plane(point1,point2,point3);
    double zInterceptThisPlane=0.;      
    Gaudi::XYZVector para=plane.Normal();
    if(para.Z()!=0){
      double a=para.X();
      double b=para.Y();
      double d=plane.HesseDistance();        
      zInterceptThisPlane=-(a*xpos+b*ypos+d)/(para.Z());
    }else zInterceptThisPlane=point1.z();      
    zaverage=zaverage+ (float)zInterceptThisPlane;
  }    
  countGap++;    
  
  zaverage=zaverage/(gapNumberStop-gapNumberStart+1);  
  return sc;

  
}
int MuonBackground::chamberOffset(int station,int region)
{
  return m_chamberInRegion[station*4+region];  
}


int MuonBackground::numberOfCollision(const LHCb::MCVertex* pointVertex)
{
  int collision=0;
  //info()<<" entering "<<endmsg;  
  std::vector<const LHCb::MCVertex*>::iterator it;
  for(it=m_vertexList.begin();it<m_vertexList.end();it++){
    if((*it)==pointVertex)return collision;
    collision++;    
  } 
//  info()<<collision<<" qui "<<pointVertex<<endmsg;
  m_vertexList.push_back(pointVertex);
  return collision; 
}


