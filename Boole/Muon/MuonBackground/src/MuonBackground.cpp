// $Id: MuonBackground.cpp,v 1.17 2003-12-11 07:40:17 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 

//from detector description

#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElementException.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/IReadOut.h"
#include "MuonDet/DeMuonRegion.h"
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/MuonParameters.h"
#include "MuonDet/DeMuonGasGap.h"
#include "MuonDet/MuonReadoutCond.h"
#include "MuonDet/MuonBasicGeometry.h"
#include "Event/MCMuonHit.h"
// local
#include "MuonBackground.h"

#define DIMENSIONMAX 20

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
  : Algorithm ( name , pSvcLocator ) {
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

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;
  // initialize geometry based quantities as station number, 
  //gaps per station and their z position 
  m_numberOfEventsNeed=5;
  
  MuonBasicGeometry basegeometry(detSvc(),msgSvc()); 
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();  int i=0;  
  while(i<m_stationNumber){
    numsta[i]=basegeometry.getStationName(i);    
    msg<<MSG::DEBUG<<" station "<<i<<" "<<numsta[i]<<endreq;
    i++;    
  }
 
  StatusCode sc=toolSvc()->retrieveTool("MuonTileIDXYZ",m_pMuonTileXYZ); 
  if(sc.isFailure())return StatusCode::FAILURE;
  sc=toolSvc()->retrieveTool("MuonGetInfoTool",m_pGetInfo);
  if(sc.isFailure())return StatusCode::FAILURE;
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
  msg<<MSG::DEBUG<<" type input "<<m_typeOfBackground<<endreq;
    msg<<MSG::DEBUG<<" safety factor "<<m_safetyFactor[0]<<
	" "<<m_safetyFactor[1]<<" "<<m_safetyFactor[2]<<" "<<
	m_safetyFactor[3]<<" "<<m_safetyFactor[4]<<endreq;

  if(m_typeOfBackground=="LowEnergy"){
    m_type=LowEnergy;    
  }else if(m_typeOfBackground=="FlatSpillover"){
    m_type=FlatSpillover;        
  }
  msg << MSG::DEBUG <<" type "<< m_type<<endreq;
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
    IAlgManager* algmgr;
    sc = service( "ApplicationMgr", algmgr );
    if( !sc.isSuccess() ) {
      msg << MSG::ERROR << "Failed to locate algManager i/f of AppMgr"<< endmsg;
      return sc;
    }
    IAlgorithm*  spillAlg;
    sc = algmgr->getAlgorithm( "SpilloverAlg", spillAlg );
    if( !sc.isSuccess() ) {
      msg << MSG::WARNING << "SpilloverAlg not found" << endmsg;
      m_readSpilloverEvents = 0;
    }
    else {
      IProperty* spillProp;
      spillAlg->queryInterface( IID_IProperty, (void**)&spillProp );
      StringArrayProperty evtPaths;
      evtPaths.assign( spillProp->getProperty("PathList") );
      m_readSpilloverEvents = evtPaths.value().size();
      // Release the interfaces no longer needed
      spillAlg->release();
    }
    algmgr->release();

    msg << MSG::DEBUG << "number of spillover events read from aux stream "
        << m_readSpilloverEvents << endmsg;  
  }else if(m_type==FlatSpillover){
    m_readSpilloverEvents=1;
    m_luminosityFactor=m_luminosity/2.0;   
    //    m_readSpilloverEvents=m_numberOfFlatSpill;    
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonBackground::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;
  for (int ispill=0;ispill<=m_readSpilloverEvents;ispill++){    
    calculateNumberOfCollision(ispill);  
    if(!collisions())return  StatusCode::SUCCESS;
    //	 if(collisions()>1)return  StatusCode::SUCCESS;
    

    msg << MSG::DEBUG << "==> collsion " << collisions()<<endreq;

 
    KeyedContainer<MCMuonHit>* hitsContainer[DIMENSIONMAX]; // fix for VC6
    {for(int station=0;station<m_stationNumber;station++){
      for(int region=0;region<m_regionNumber;region++){          
        hitsContainer[station*m_regionNumber+region]
          =new KeyedContainer<MCMuonHit>();
      }
    }
    }// Fix for VC6 scoping bug
    
    m_resultPointer = new std::vector<std::vector<int> > (collisions()) ;

    if(m_type==LowEnergy){
      // m_resultPointer = new std::vector<std::vector<int> > (collisions()) ;
      calculateStartingNumberOfHits(ispill);    
      for(int coll=0;coll<collisions();coll++){      
        for(int station=0;station<m_stationNumber;station++){        
          for (int multi=0;multi<m_gaps;multi++){
            int index=station*m_gaps+multi;
            int startingHits=((*m_resultPointer)[coll])[index];
            msg<<MSG::DEBUG<<"station safe start end hits "
                <<startingHits<<endreq;
            
            // extract number of hits to be added
            int hitToAdd=0;
            float floatHit=0;          
            if(numsta[station]=="M1"){
              floatHit=(startingHits*(m_safetyFactor[station]));   
              hitToAdd=howManyHit( floatHit);            
            } else{          
              int yy=(int)(m_correlation[index])->
                         giveRND(startingHits+0.5);
              
              floatHit=(m_safetyFactor[station]*
                        (yy));
              hitToAdd=howManyHit( floatHit);
              //msg<<MSG::DEBUG<<"station safe start end hits "<<
              //  station<< " "<<m_safetyFactor[station]<< " "<<
              //  yy<< " "<< hitToAdd<<" "<<startingHits<<endreq;
              
              msg<<MSG::DEBUG<<"adding "<< hitToAdd<<" to orginal "<<
                startingHits<<" in station "<<station<<
                " for multiplicity "<<multi<<" and collisions/spill "<<coll
                 <<" "<<ispill<<endreq;
            }            
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
      for(int station=0;station<m_stationNumber;station++){        
        for (int multi=0;multi<m_gaps;multi++){
          int index=station*m_gaps+multi;
          float floatHit=m_flatSpilloverHit[index]*m_luminosityFactor
            *m_safetyFactor[station];         
          for (int fspill=0;fspill<=m_numberOfFlatSpill;fspill++){
            int hitToAdd=0;
            hitToAdd=howManyHit( floatHit);          
            msg<<MSG::DEBUG<<"adding "<< hitToAdd<<
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
    msg<<MSG::DEBUG<<" starting saveing the ocntainer "<<endreq;
    
      
    {for(int station=0;station<m_stationNumber;station++){
      for(int region=0;region<m_regionNumber;region++){
        std::string path="/Event"+spill[ispill]+"/MC/Muon/"+numsta[station]+
          "/R"+numreg[region]+"/"+m_containerName;
        msg<<MSG::DEBUG<<" number of total hit added "<<
          hitsContainer[station*m_regionNumber+region]->size()<<endreq;
        eventSvc()->registerObject(path,
                                   hitsContainer[station*m_regionNumber
                                                 +region]);   
      }      
    }
    } // Fix for VC6 scoping bug    
    delete m_resultPointer;    
  }  
  return StatusCode::SUCCESS;
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonBackground::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;
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
  if( m_pMuonTileXYZ ) toolSvc()->releaseTool( m_pMuonTileXYZ );    
  if( m_pGetInfo )     toolSvc()->releaseTool( m_pGetInfo );    
  return StatusCode::SUCCESS;
}


//=============================================================================
//=============================================================================
//  GeometryInitialization
//=============================================================================
StatusCode MuonBackground::initializeGeometry() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> initializeGeometry" << endreq;
  

  
  //  MuonGeometryStore::Parameters usefull( toolSvc(),detSvc(), msgSvc());
  
  {for(int i=0;i<20;i++){       
    msg<<MSG::DEBUG<<" partition "<<i<<" chamber "<<m_pGetInfo<<endreq;    
    msg<<MSG::DEBUG<<" partition "<<i<<" chamber "<<
      m_pGetInfo->getChamberPerRegion(i)<<endreq;  
  }
  } // Fix for VC6 scoping bug
  int gap=0;
  
  {for(int i=0;i< (int)m_partition;i++){
    gap=(gap>=(int)m_pGetInfo->getGapPerRegion((unsigned int)i))?
      gap:m_pGetInfo->getGapPerRegion(i);
    
  }
  } // Fix for VC6 scoping bug
  
  m_gaps=gap;
  m_maxDimension=m_gaps*m_stationNumber;
  containerOfFirstGapPosition.resize(m_stationNumber+1);
  
  // store the first gap position per station for all the chamber 
  // where the first gap differs by nore than half gap from 
  // all the other already stored
  int station;
  int chamberInPartition;
  int chamberNumber=0;
  float zgaphwidth=0;
  float localZGap=0;
  std::vector<float>* gapContainer; 
  std::vector<float>::iterator itGap;
  bool insert;

  
  
  {for(int i=0;i< m_partition;i++){
    m_numberOfGaps.push_back(m_pGetInfo->getGapPerRegion(i));    
    station=i/m_regionNumber;
    chamberInPartition=(int)m_pGetInfo->getChamberPerRegion(i);   
    zgaphwidth= (m_pGetInfo->getStopGapZ(0,i)-
                 m_pGetInfo->getStartGapZ(0,i))/2;    
    gapContainer=getVectorOfGapPosition(station);
    msg<<MSG::DEBUG<<" gap "<<i<<" "<<zgaphwidth<<endreq;
    for(int localChamberNumber=0;localChamberNumber<chamberInPartition;
        localChamberNumber++){
      //msg<<MSG::INFO<<" imbecille chamber "<<localChamberNumber<<endreq;
      
      localZGap=m_pGetInfo->getStartPositionFirstGapZ(chamberNumber)+
        zgaphwidth/2;
      insert=true;      
      //  msg<<MSG::INFO<<"insert "<<insert<<endreq;
      // msg<<MSG::INFO<<gapContainer->begin()<<endreq;
  
      for(itGap=gapContainer->begin();itGap<gapContainer->end();
          itGap++){
        if(fabs(localZGap-*(itGap)) <zgaphwidth){
          insert=false;          
        }        
      }      
      if(insert)gapContainer->push_back(localZGap);    
      chamberNumber++;      
    }
  }
  } // Fix for VC6 scoping bug
  
  
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
          msg<<MSG::DEBUG<<code<<" "<< station<<" "<<mult<<" "<<tt<<endreq;
          if(tt<100000){
            sprintf(codePath,"%5i", tt); 
          }else{              
            sprintf(codePath,"%6i", tt);
          }
          
          std::string path=m_histoFile+"/"+codePath;
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
              MuBgDistribution* mubg=new   
                MuBgDistribution(distributions,pointerToFlags,xmin, xmax ); 
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
              msg<<MSG::DEBUG<<"found the 2D histo "<<path<<endreq;
              std::vector<Rndm::Numbers*>  distributions;
              double xmin,xmax,ymin,ymax;
              int nbinx;              
              std::vector<bool>   pointerToFlags;
              initializeRNDDistribution2D(histo2d,
                                         distributions ,pointerToFlags,xmin, 
                                          xmax, nbinx ,ymin, ymax);
              MuBgDistribution* mubg=new   
                MuBgDistribution(distributions,pointerToFlags,
                                 xmin, xmax,nbinx,ymin,ymax); 
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
  msg << MSG::DEBUG << "==> calculateNumberOfCollsion " << endreq;
  int collision=0;
  std::string collisionLocation="/Event"
    +spill[ispill]+"/"+CollisionLocation::Default;
  msg<<MSG::DEBUG<<"string "<<collisionLocation<<endreq;
  msg<<MSG::DEBUG<<"spill "<<ispill<<endreq;
  
  SmartDataPtr<Collisions> collisionsPointer
    ( eventSvc(), collisionLocation );
  Collisions::iterator itCollision;
  collision=0;  
  if(collisionsPointer){
    
    for(itCollision=collisionsPointer->begin();
        itCollision<collisionsPointer->end();itCollision++){
      msg<<MSG::DEBUG<<" --- collision number "
         << (*itCollision)->key()<<endreq;
      collision++;      
    }
  }  
  setCollsionNumber(collision);
  msg<<MSG::DEBUG<<" --- total collision number "<<collision<<endreq;
  
  return StatusCode::SUCCESS; 
}


StatusCode MuonBackground::calculateStartingNumberOfHits(int ispill) {
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> calculateStartingNumberOfHit " << endreq;
  bool first=true;
  int gap,chamber,index;
  int preGap,preChamber,preIndex;

  std::string particleLocation="/Event"
    +spill[ispill]+"/"+MCParticleLocation::Default;
  msg<<MSG::DEBUG<<"string "<<particleLocation<<endreq;
  
  SmartDataPtr<MCParticles> particlePointer
    ( eventSvc(),particleLocation  );
  MCParticles::iterator itParticle;
  for(itParticle=particlePointer->begin();
      itParticle<particlePointer->end();itParticle++){
    }
  int numberOfParticles=0;
  numberOfParticles=(*(particlePointer->end()-1))->key()+1;
  msg<<MSG::DEBUG<<" --- original number of particles "
     << numberOfParticles<<endreq;
  //create an dimension appropriate vector
  std::vector<ParticleInfo*> particleInfo(numberOfParticles);
  //loop un hits  
  //  MuonGeometryStore::Parameters usefull( toolSvc(),detSvc(), msgSvc());
  
  int chamberOffset=0;
  
  for(int iterRegion=0;iterRegion<m_partition;iterRegion++){
    int station=iterRegion/m_regionNumber;
    int region=iterRegion%m_regionNumber;    
            msg<<MSG::DEBUG<<" region" <<iterRegion<<endreq;
    
    for(int container=0; container<1;container++){				
      std::string path="/Event"+spill[ispill]+"/MC/Muon/"+
        numsta[station]+"/R"+numreg[region]+"/"+
        TESPathOfHitsContainer[container];        
      SmartDataPtr<MCMuonHits> hitPointer(eventSvc(),path);
      MCMuonHits::const_iterator iter;	 
      preGap=-1;
      preIndex=-1;
      preChamber=-1;      
      if(hitPointer!=0){
        for (iter=(hitPointer)->begin();iter<(hitPointer)->end();iter++)
          {gap=(*iter)->gapID();
          chamber=(*iter)->chamberID()+chamberOffset;   
          index=(*iter)->mcParticle()->key();
          if(station==1){
            msg<<MSG::DEBUG<<" index, chamber, gap "<<index<<" "<<
            chamber<<" "<<gap<<endreq;
            msg<<MSG::DEBUG<<" index "<<index<<" in position "<<
             (*iter)->entry().x()<<" "<<
             (*iter)->entry().y()<<" "<<
             (*iter)->entry().z()<<" "<<
              " out position  "<<            
             (*iter)->exit().x()<<" "<<
             (*iter)->exit().y()<<" "<<
             (*iter)->exit().z()<<endreq;
          }
          
          if(station==1)
            msg<<MSG::DEBUG<<" gap cha ind "<<preGap<<" "<<preChamber<<
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
    chamberOffset=chamberOffset+m_pGetInfo->getChamberPerRegion(iterRegion);   
  }
  
   int partCollision;
  
  {for(int i=0;i<collisions();i++){
    (*m_resultPointer)[i].resize(m_maxDimension);      
  }    
  } // Fix for VC6 scoping bug
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
  
  
  //  msg<<MSG::INFO<<collisions()<<" "<<m_maxDimension<<endreq;
  
  {for (int i=0; i<collisions();i++){
    for(int j=0;j<m_maxDimension;j++){ 
      //    msg<<MSG::INFO<<"multiplicity for collison "
      //   <<i<<" "<< j<<" "<<((* m_resultPointer)[i])[j]<<endreq;
    }      
  }
  } // Fix for VC6 scoping bug
  
  
  msg<<MSG::DEBUG<<" --- end of routine "<< endreq;  
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
 
   MsgStream msg(msgSvc(), name());
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
      msg<<MSG::DEBUG<<" negative value for histogragm "<<histoPointer->title()
         <<" "<<tmp<<endreq;
            tmp=0;
      
    }
    
    content[i]=tmp;    
    total=total+(int)histoPointer->binHeight(i);    
    // msg<<MSG::INFO<<"content "<<histoPointer->binHeight(i)
    //   <<" "<<content[i]<<" =  "<<tmp<< endreq;
    
  }  
  Rndm::Numbers* pdf=new Rndm::Numbers; 
  //msg<<MSG::INFO<<"total "<<total<<endreq;
  
  if(total==0){
    pointerToFlags.push_back(false);
  }
  else{
    pointerToFlags.push_back(true);
    pdf->initialize(randSvc(), Rndm::DefinedPdf(content,0));            
    //msg<<MSG::INFO<<"extract two numbers "<<(*pdf)()<<" "<< (*pdf)()<<endreq;
    
  }  
  //msg<<MSG::INFO<<"total 1 "<<total<<endreq;
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
  MsgStream msg(msgSvc(), name());
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
    //char codePath[nbinx];
    //sprintf(codePath,"%5i", tt); 
    //    msg<<MSG::INFO<<"slicing "<<xbin<<" "<<ymax<<endreq;
    
    IHistogram1D* ySlice = histoSvc()->histogramFactory()->
      sliceY( "MuBG/1" , *histoPointer, xbin);
    int entries=ySlice->entries();
    if(entries==0) {
      msg<<MSG::DEBUG<<" zero entries"<<endmsg;
    }
    
    //  msg<<MSG::INFO<<" entries "<<entries<<endreq;
    /*  for(int bincount=0;bincount<nbiny;bincount++){
      msg<<MSG::INFO<<" entries in bin "<<bincount<<" "<<
      ySlice->binEntries(bincount)<<" "<<
      ySlice->binHeight(bincount)<<endreq;
    } */
    
    initializeRNDDistribution1D(ySlice,distributions , 
                                pointerToFlags,ymin, ymax);
    // msg<<MSG::INFO<<"slicing "<<xbin<<" "<<ymax<<endreq;
    histoSvc()->unregisterObject( ySlice );
    delete ySlice;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode MuonBackground::createHit(KeyedContainer<MCMuonHit>** 
                                                  hitsContainer,int station,
                                                  int multi,int ispill)
{
  MsgStream msg(msgSvc(), name());
  //  MuonGeometryStore::Parameters usefull( toolSvc(),detSvc(), msgSvc());
  
  int maxTryPhi=10;
  int maxTryR=10;
  int tryR=0;
  int tryPhi=0;
  bool hitInsideCha=false;
  //  std::vector<MCMuonHit*> hitsContainer;
  //first define all the relevant quantities .. if ok then crete the hit!
  //1) extract the radial position
  int index=station*m_gaps+multi;
  float r = 0.;
  float globalPhi = 0.;
  unsigned int chamberIndex = 0;
  unsigned int regionIndex  = 0;  
  //  int partitionIndex;
  
  float xpos = 0.;
  float ypos = 0.;
  float zpos = 0.;
  msg<<MSG::DEBUG<<"new track"<<endreq;
  
  while(!hitInsideCha&&tryR<maxTryR){
    
    r=(m_radial[index])->giveRND();
  //2) extract the gloabl phi 
    tryPhi=0;  
    while(!hitInsideCha&&tryPhi<maxTryPhi){
      
      globalPhi=((m_phiglobvsradial[index])->giveRND(r))*pi/180.0;
      //3) test whether the r,phi is inside the sensitive chamber volume 
      // getVectorOfGapPosition(int station);
      //  transform r and phi in x,y
      xpos=r*cos(globalPhi)*cm;
      ypos=r*sin(globalPhi)*cm;
      if(multi==3) 
        msg<<MSG::DEBUG<<r<<" "<<globalPhi<<" "<<cos(globalPhi)<<" "<<
        sin(globalPhi)<<endreq;
      if(multi==3)msg<<MSG::DEBUG<<"xpos, ypos "<<xpos<<" "<<ypos<<endreq;
      
      DeMuonGasGap* pGasGap;
      MuonTileID tile; 
       
      for(int loopOnGaps=0;loopOnGaps<(int)
            getVectorOfGapPosition(station)->size()&&
            !hitInsideCha;loopOnGaps++){
        zpos= (*getVectorOfGapPosition(station))[loopOnGaps];    
      msg<<MSG::DEBUG<<"zpos "<<zpos<<endreq;
        
        StatusCode sc=m_pMuonTileXYZ->
          locateChamberTileAndGap(xpos,ypos,zpos,tile,pGasGap);
        if(sc.isFailure()){
          msg<<MSG::DEBUG<<" no gap found for muon hit" <<endreq;
        }else{      
          chamberIndex=(unsigned int)pGasGap->chamberNumber();
          regionIndex=(unsigned int)pGasGap->regionNumber();
          
          //remember this is the chamber number inside a region....
          msg<<MSG::DEBUG<<" chamber number "<<pGasGap->chamberNumber()
             <<" station number "<<pGasGap->stationNumber()
             <<" region number "<<pGasGap->regionNumber()<<
            "position "<<xpos<<" "<<ypos<<" "<<endreq;
          
            
          hitInsideCha=true;
        }
      }
      tryPhi++;
    }
    tryR++;
  }  
  //  partitionIndex=station*m_regionNumber+regionIndex;
  
  if(!hitInsideCha){
    msg<<MSG::DEBUG<<" could not find a r.phi combination "<<endreq;
  }else{
    //define the chamber index in the total reference...
    int partition=station*m_regionNumber+regionIndex-1;    
    int chamberGlobal=
      m_pGetInfo->getGlobalChamberNumber(chamberIndex,partition)-1;    

    //extract the hit gaps
    int allgap=(int)(m_hitgap[index])->giveRND();
    int max=8;    
    std::vector<int> gapHitTmp;
    msg<<MSG::DEBUG<<" gap extracted "<<allgap<<" multiplicity "<<multi<<
      endreq;
    
    for(int i=0;i<m_gaps;i++){      
      int gap=allgap/max;      
      if(gap>1)gap=1;      
      if(gap==1)gapHitTmp.push_back(m_gaps-i-1);
      allgap=allgap-gap*max;      
      if(gap==1)
        msg<<MSG::DEBUG<<"allgap "<<gap<<" "<<gapHitTmp.back()<<" "<<
          max<<endreq;
      max=max/2;      
    }
    std::vector<int> gapHit;
    std::vector<int>::reverse_iterator it;
    for(it=gapHitTmp.rbegin();it<gapHitTmp.rend();it++){      
      gapHit.push_back((*it)); 
      msg<<MSG::DEBUG<<" gap back"<<gapHit.back()<<endreq;;
      
    }
    
    int firstGap=gapHit[0];    
    int lastGap=gapHit[gapHit.size()-1];

    
    int tryPhiLoc=0;  
    int maxTryPhiLoc=20;
    bool allHitsInsideCha=false;
    bool hitsToAdd;    
    float phiLoc;
    float thetaLoc;
    float xSlope,ySlope;
    for(tryPhiLoc=0;tryPhiLoc<maxTryPhiLoc&&!allHitsInsideCha;tryPhiLoc++){
      allHitsInsideCha=true;      
      hitsToAdd=false;      
      phiLoc=((m_philocvsradial[index])->giveRND(r))*pi/180.0;
      thetaLoc=((m_thetalocvsradial[index])->giveRND(r))*pi/180.0;    
      //      thetaLoc=0;
      
      //define the more confortable slope in x-y direction 
      if(cos(thetaLoc)!=0){        
        xSlope=sin(thetaLoc)*cos(phiLoc)/cos(thetaLoc);
        ySlope=sin(thetaLoc)*sin(phiLoc)/cos(thetaLoc);    
      }      
      else{
        xSlope=1.0;
        ySlope=1.0;
      }
        
      //define the z of the average gaps position
      float averageZ=m_pGetInfo->getStartPositionFirstGapZ(chamberGlobal)+
        (m_pGetInfo->getStartGapZ(firstGap,partition)+
         m_pGetInfo->getStopGapZ(lastGap,partition))/2.0;
      for(int igap=0;igap<=multi;igap++){
       
        
        int gapNumber=gapHit[igap];
        
        
        float zStartGap,zStopGap;
        zStartGap=m_pGetInfo->getStartPositionFirstGapZ(chamberGlobal)+
          m_pGetInfo->getStartGapZ(gapNumber,partition);
        zStopGap=m_pGetInfo->getStartPositionFirstGapZ(chamberGlobal)+
          m_pGetInfo->getStopGapZ(gapNumber,partition);
        float xentry,yentry,zentry;
        float xexit,yexit,zexit;
        
        zentry=zStartGap;
        zexit=zStopGap;
        xentry=xpos+xSlope*(zentry-averageZ);
        xexit=xpos+xSlope*(zexit-averageZ);
        yentry=ypos+ySlope*(zentry-averageZ);
        yexit=ypos+ySlope*(zexit-averageZ);
        float x=(xentry+xexit)/2.0;
        float y=(yentry+yexit)/2.0;
        float z=(zentry+zexit)/2.0;
        
        
        DeMuonGasGap* pGasGap;
        MuonTileID tile; 
    
        StatusCode sc=m_pMuonTileXYZ->
          locateChamberTileAndGap(x,y,z,tile,pGasGap);
        if(sc.isSuccess()){
          unsigned int chamberTmp=(unsigned int)pGasGap->chamberNumber();
          unsigned int regionTmp=(unsigned int)pGasGap->regionNumber();
          if(regionTmp==regionIndex&&chamberTmp==chamberIndex){
          }else{
            allHitsInsideCha=false;
            
          }          
          
          
        } else{
          allHitsInsideCha=false;
          msg<<MSG::DEBUG<<"rejected "<<x<<" "<<y<<" "<<z<<" "<<multi<<endreq;
          msg<<MSG::DEBUG<<"rejected "<<xpos<<" "<<ypos<<" "<<
            zpos<<" "<<averageZ<<endreq;
          msg<<MSG::DEBUG<<" chamber "<<chamberGlobal<< " gap position "<<
            m_pGetInfo->getStartPositionFirstGapZ(chamberGlobal)+2.5
             <<" "<<chamberGlobal<<
            endreq;
          
        }
        
      }
      
      if(allHitsInsideCha)hitsToAdd=true;
      
    }

    

    if(hitsToAdd){
      float scale=log(10.0);      
      float time= exp(scale*(m_logtimevsradial[index])->giveRND(r));
      float timeBest=0;    
      //patch to extract the timein linear mode if needed other 
      //wise just transform inlinear fromlog scale
      if(time<=m_lintimevsradial[index]->giveMax()){
        timeBest=m_lintimevsradial[index]->giveRND(r);
      }else {
        timeBest=time;
      }
      float averageZ=m_pGetInfo->getStartPositionFirstGapZ(chamberGlobal)+
        (m_pGetInfo->getStartGapZ(firstGap,partition)+
         m_pGetInfo->getStopGapZ(lastGap,partition))/2.0;
      for(int igap=0;igap<=multi;igap++){
        bool correct=true;        
        int gapNumber=gapHit[igap];
        float zStartGap,zStopGap;
        zStartGap=m_pGetInfo->getStartPositionFirstGapZ(chamberGlobal)+
          m_pGetInfo->getStartGapZ(gapNumber,partition);
        zStopGap=m_pGetInfo->getStartPositionFirstGapZ(chamberGlobal)+
          m_pGetInfo->getStopGapZ(gapNumber,partition);
        float xentry,yentry,zentry;
        float xexit,yexit,zexit;
        zentry=zStartGap;
        zexit=zStopGap;
        xentry=xpos+xSlope*(zentry-averageZ);
        xexit=xpos+xSlope*(zexit-averageZ);
        yentry=ypos+ySlope*(zentry-averageZ);
        yexit=ypos+ySlope*(zexit-averageZ);
        float x=(xentry+xexit)/2.0;
        float y=(yentry+yexit)/2.0;
        float z=(zentry+zexit)/2.0;
        MCMuonHit* pHit = new MCMuonHit();
        pHit->setEntry(HepPoint3D(xentry,yentry,zentry));                     
        pHit->setExit(HepPoint3D(xexit,yexit,zexit));
        
        double tofOfLight=(sqrt(x*x+ y*y+z*z))/300.0;
        float shiftOfTOF=-ispill*m_BXTime;
        
        pHit->setTimeOfFlight(timeBest+tofOfLight+shiftOfTOF); 
        pHit->setVolumeID(chamberIndex,gapNumber+1);
        msg<<MSG::DEBUG<<"gap , time ,position "<<gapNumber<<" "<<
          timeBest+tofOfLight<<" "<<x<<" "<<y<<" "<<chamberGlobal<<endreq;
        msg<<MSG::DEBUG<<"chamber "<<chamberIndex	<<endreq;
        
        DeMuonGasGap* pGasGap;
        MuonTileID tile; 
    
        StatusCode sc=m_pMuonTileXYZ->
          locateChamberTileAndGap(x,y,z,tile,pGasGap);
        if(sc.isSuccess()){
          int chamberTest=(unsigned int)pGasGap->chamberNumber();
          int regionTest=(unsigned int)pGasGap->regionNumber();
          int gapTest=(unsigned int)pGasGap->gasGapNumber();
          msg<<MSG::DEBUG<<" inserting in chamber "<<chamberTest<<" region "<<
            regionTest<<" gap "<<gapTest<<endreq;
          sc=m_pMuonTileXYZ->
            locateChamberTileAndGap(xentry+0.1,yentry+0.1,zentry+0.1,
                                  tile,pGasGap);
          
          if(sc.isSuccess()){           
            int chamberTest1=(unsigned int)pGasGap->chamberNumber();
            int regionTest1=(unsigned int)pGasGap->regionNumber();
            //            int gapTest1=(unsigned int)pGasGap->gasGapNumber();  
            
            if(chamberTest!=chamberTest1){
              correct=false;
              //delete pHit;                
            }        
            if(regionTest!=regionTest1){
              correct=false;
              //delete pHit;                
            }        
    
          }          
          else{
              correct=false;
              //delete pHit; 
          }
          sc=m_pMuonTileXYZ->
            locateChamberTileAndGap(xexit-0.1,yexit-0.1,zexit-0.1,
                                    tile,pGasGap);
          if(sc.isSuccess()){ 
            int chamberTest2=(unsigned int)pGasGap->chamberNumber();
            int regionTest2=(unsigned int)pGasGap->regionNumber();
            // int gapTest2=(unsigned int)pGasGap->gasGapNumber();
            
            if(chamberTest!=chamberTest2){
                correct=false;
                //delete pHit;                
            }
            if(regionTest!=regionTest2){
              correct=false;
              //delete pHit;                
            }        
          }              
          else{
            correct=false;
            //delete pHit; 
          }              
        
         


        
        
        
          if(!correct){
            float xlow=m_pGetInfo->getStartChamberPositionX(chamberGlobal);
            float ylow=m_pGetInfo->getStartChamberPositionY(chamberGlobal);
            float Xlenght=m_pGetInfo->getPhChannelNX(0,partition)*
              m_pGetInfo->getPhChannelSizeX(0,partition);
            float Ylenght=m_pGetInfo->getPhChannelNY(0,partition)*
              m_pGetInfo->getPhChannelSizeY(0,partition);
            float xup=xlow+ Xlenght;
            float yup=ylow+ Ylenght;
            float zlow=zentry;
            float zup=zexit;
            float xave=(xentry+xexit)/2.0;
            float yave=(yentry+yexit)/2.0;
            float zave=(zentry+zexit)/2.0;
            msg<<MSG::DEBUG<<" entry position "<<xentry<<" "<<yentry<<" "<<
              zentry<<endreq;
            msg<<MSG::DEBUG<<" exit position "<<xexit<<" "<<yexit<<" "<<
              zexit<<endreq;
            msg<<MSG::DEBUG<<" chamber dimension "<<xlow<<" "<<xup<<" "<<
              ylow<<" "<<yup<<" "<<zlow<<" "<<zup<<endreq;
            
            StatusCode test=correctInterceptPosition(xlow,xup,
                                                   ylow,yup,
                                                     zlow,zup,
                                                     xave,yave,
                                                     zave,xSlope, 
                                                     ySlope,xentry,
                                                     xexit,
                                                     yentry,
                                                     yexit, 
                                                     zentry,
                                                     zexit);
            if(test.isSuccess()){
              msg<<MSG::DEBUG<<" entry position "<<xentry<<" "<<yentry<<" "<<
                zentry<<endreq;
              msg<<MSG::DEBUG<<" exit position "<<xexit<<" "<<yexit<<" "<<
                zexit<<endreq;
              msg<<MSG::DEBUG<<" chamber dimension "<<xlow<<" "<<xup<<" "<<
                ylow<<" "<<yup<<" "<<zlow<<" "<<zup<<endreq;
              
              pHit->setEntry(HepPoint3D(xentry,yentry,zentry));
              pHit->setExit(HepPoint3D(xexit,yexit,zexit));
              float xd=(xentry+xexit)/2;
              
              float yd=(yentry+yexit)/2;
              
              float zd=(zentry+zexit)/2;
              
              tofOfLight=(sqrt(xd*xd+ yd*yd+zd*zd))/300.0;
        
        
              pHit->setTimeOfFlight(timeBest+tofOfLight+shiftOfTOF);
      
              //(hitsContainer[partition])->insert(pHit);
              correct=true;
              
            } else{
              delete pHit;
              
            }
            
          }
          
          
        }else{
          msg<<MSG::INFO<<"qualcosa non funziona "<<endreq;
          
        }
        msg<<MSG::DEBUG<<" correct "<<correct<<" "<<partition<<endreq;
        
        if(correct)(hitsContainer[partition])->insert(pHit);
        if(correct) msg<<MSG::DEBUG<<" inserted "<<endreq;
        if(correct){
          msg<<MSG::DEBUG<<"entry position "<<pHit->entry().x()<<
            " "<<pHit->entry().y()
             <<" "<<pHit->entry().z()<<endreq;
          msg<<MSG::DEBUG<<"exit position "<<pHit->exit().x()<<
            " "<<pHit->exit().y()
             <<" "<<pHit->exit().z()<<endreq;
          
        }
        
      }
      
      
    }else{
       msg<<MSG::WARNING<<" impossible to add the requested hits "<<
         "in station "<<station<<" and multiplicity "<<multi<<endreq;
    }
    

    
  }
  
  return StatusCode::SUCCESS;
}



StatusCode MuonBackground::correctInterceptPosition(float xlow,float xup,
                                                    float ylow,float yup,
                                                    float zlow,float zup,
                                                    float xave,float yave,
                                                    float zave,float xslope, 
                                                    float yslope,float& xentry,
                                                    float & xexit,
                                                    float& yentry,
                                                    float& yexit, 
                                                    float& zentry,
                                                    float& zexit)
{
  MsgStream msg(msgSvc(), "hit correction");
 
  float z1,z2;
  float zmin,zmax;
  zmin=zlow;
  zmax=zup;
  


  //then x
  if(xslope==0)return StatusCode::FAILURE;
  z1=(xlow-xave)/xslope+zave;
  z2=(xup-xave)/xslope+zave;
  float zz1=min(z1,z2);
  float zz2=max(z1,z2);
  zmin=max(zmin,zz1);
  zmax=min(zmax,zz2);

  //then y
  if(yslope==0)return StatusCode::FAILURE;
  z1=(ylow-yave)/yslope+zave;
  z2=(yup-yave)/yslope+zave;
  zz1=min(z1,z2);
  zz2=max(z1,z2);
  zmin=max(zmin,zz1);
  zmax=min(zmax,zz2);
  xentry=xave+xslope*(zmin-zave);  
  xexit=xave+xslope*(zmax-zave); 
  yentry=yave+yslope*(zmin-zave);  
  yexit=yave+yslope*(zmax-zave); 
  zentry=zmin;
  zexit=zmax;
  //checks
  DeMuonGasGap* pGasGap;
  MuonTileID tile; 
  int chamber;
  int  gap;
  int region;
  int station;  
  StatusCode sc=m_pMuonTileXYZ->
    locateChamberTileAndGap(xave,yave,zave,tile,pGasGap);
  if(sc.isSuccess()){
    chamber=pGasGap->chamberNumber();
    gap=pGasGap->gasGapNumber();
    station=pGasGap->stationNumber();
    region=pGasGap->regionNumber();    
  }
  else{
     return StatusCode::FAILURE;
  }
  msg<<MSG::DEBUG<<"middle station region chamber index "<<
    station<<" "<<region<<" "<<chamber<<" "<<gap<<endreq;
  
  sc=m_pMuonTileXYZ->
    locateChamberTileAndGap(xentry,yentry,zentry,tile,pGasGap);

 
 
  if(sc.isSuccess()){
   msg<<MSG::DEBUG<<"entry station region chamber index "<<
    pGasGap->stationNumber()<<" "<<pGasGap->regionNumber()
     <<" "<<pGasGap->chamberNumber()<<" "<<pGasGap->gasGapNumber()<<endreq;  
    if(chamber!=pGasGap->chamberNumber()) return StatusCode::FAILURE;
    if(gap!=pGasGap->gasGapNumber())return StatusCode::FAILURE;
    if(station!=pGasGap->stationNumber())return StatusCode::FAILURE;    
    if(region!=pGasGap->regionNumber())return StatusCode::FAILURE;    
  }
  else{
    return StatusCode::FAILURE;
  }
  sc=m_pMuonTileXYZ->
    locateChamberTileAndGap(xexit,yexit,zexit,tile,pGasGap);

  if(sc.isSuccess()){
    msg<<MSG::DEBUG<<"exit station region chamber index "<<
    pGasGap->stationNumber()<<" "<<pGasGap->regionNumber()
     <<" "<<pGasGap->chamberNumber()<<" "<<pGasGap->gasGapNumber()<<endreq;
   if(chamber!=pGasGap->chamberNumber()) return StatusCode::FAILURE;
    if(gap!=pGasGap->gasGapNumber())return StatusCode::FAILURE;
    if(station!=pGasGap->stationNumber())return StatusCode::FAILURE;    
    if(region!=pGasGap->regionNumber())return StatusCode::FAILURE;
  }
  else{
    return StatusCode::FAILURE;
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



