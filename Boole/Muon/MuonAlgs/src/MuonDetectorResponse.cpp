// $Id: MuonDetectorResponse.cpp,v 1.10 2005-04-05 09:21:08 cattanem Exp $
#include <vector>
#include "MuonAlgs/MuonDetectorResponse.h"

static std::string spill[] = {"","/Prev","/PrevPrev","/Next","/NextNext"};
static std::string numreg[] = {"1","2","3","4"};
static std::string numsta[] = {"1","2","3","4","5"};

static std::string geoBase="/dd/Structure/LHCb/Muon/";





void MuonDetectorResponse::initialize(IToolSvc* toolSvc,IRndmGenSvc * randSvc,
                                      IDataProviderSvc* detSvc, 
                                      IMessageSvc * msgSvc){  
	MsgStream log(msgSvc, "MuonDetectorResponse");
  // MuonGeometryStore::Parameters usefull( toolSvc,detSvc, msgSvc);
  toolSvc->retrieveTool("MuonGetInfoTool",m_pGetInfo);
  //if(sc.isFailure())return StatusCode::FAILURE;
  m_toolSvc=toolSvc;  
  // usefullPointer= 
  //new MuonGeometryStore::Parameters( toolSvc,detSvc, msgSvc);
	m_gaussDist.initialize( randSvc, Rndm::Gauss(0.0,1.0));
	m_flatDist.initialize( randSvc, Rndm::Flat(0.0,1.0)); 
	std::vector<double> alreadyInsertedValue;
  std::string regionName[20];
  MuonBasicGeometry basegeometry(detSvc,msgSvc); 
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();
  int i=0;  
  while(i<m_stationNumber){
    numsta[i]=basegeometry.getStationName(i);    
    log<<MSG::DEBUG<<" station "<<i<<" "<<numsta[i]<<endreq;
    i++;    
  }
  m_partition=basegeometry.getPartitions();    
  
  for(i=0;i<m_stationNumber;i++){
    for (int k=0;k<m_regionNumber;k++){
      regionName[i*4+k]=geoBase+numsta[i]+"/R"+numreg[k];
      log<<MSG::DEBUG<<"region name "<<regionName[i*4+k]<<endreq;
    }
  }
  
  //	std::vector<Rndm::Numbers*>::iterator iterOnPoisson;
  //	bool found=false;
	double newMean;
  for (int indexRegion=0;indexRegion<m_partition;indexRegion++){    
    SmartDataPtr<DeMuonRegion>  detRegionPointer(detSvc,
                                                 regionName[indexRegion]);
    IDetectorElement::IDEContainer::iterator itChamber = 
      detRegionPointer->childBegin();
    SmartDataPtr<DeMuonChamber> muChamberOne(detSvc, (*itChamber)->name());
    Condition *muCondReadout = muChamberOne->readOut()->condition();
    MuonReadoutCond *muReadout = dynamic_cast<MuonReadoutCond*>(muCondReadout);
    newMean=muReadout->chamberNoise(0)/100; //tranform from cm^2 to mm^2
    Rndm::Numbers*	poissonDist=new Rndm::Numbers;
    int readout=0;
    double sizeOfChannel = m_pGetInfo->
      getPhChannelSizeX(readout,indexRegion)*
      m_pGetInfo->getPhChannelSizeY(readout,indexRegion);
    int numberOfChannels = m_pGetInfo->
      getPhChannelNX(readout,indexRegion)*		
      m_pGetInfo->getPhChannelNY(readout,indexRegion);
    double areaOfChamber = sizeOfChannel* numberOfChannels;
    int	 numberOfGaps=m_pGetInfo->getGapPerRegion(indexRegion);
    double totalArea=areaOfChamber* numberOfGaps;
    double meanOfNoisePerChamber=newMean*totalArea*25*1.0E-9;
    log<<MSG::DEBUG<<"region name "<<regionName[indexRegion]
       <<" noise level "<<meanOfNoisePerChamber<<endreq;			
    poissonDist->initialize( randSvc, Rndm::Poisson(meanOfNoisePerChamber));  
    m_poissonDist.push_back(poissonDist);
    
    MuonChamberResponse* responseOfChamber= new 
      MuonChamberResponse(&m_flatDist,poissonDist,newMean );
    responseChamber[indexRegion]=responseOfChamber;
    {for(int readout=0;readout<=m_pGetInfo->getReadoutNumber(indexRegion);
         readout++){
      double min,max; 
      std::vector<double> timeJitterPDF=muReadout->
        timeJitter( min, max ,  readout);
      Rndm::Numbers*	m_time=new Rndm::Numbers;
      m_time->initialize(randSvc, Rndm::DefinedPdf(timeJitterPDF,0));
      m_timeJitter.push_back(m_time);
      //				 log<<MSG::INFO<<" time jitter "<<timeJitterPDF<<endreq;
      std::vector<double>::iterator itPDF;
      for (itPDF=timeJitterPDF.begin();itPDF<timeJitterPDF.end();itPDF++){
        //				     log<<MSG::INFO<<" time jitter "<<*itPDF<<endreq;
      }
      Rndm::Numbers*	p_electronicNoise=new Rndm::Numbers;
      double noiseCounts=muReadout->electronicsNoise(readout)*
        m_pGetInfo->getPhChannelNX( readout, indexRegion)*
        m_pGetInfo->getPhChannelNY( readout, indexRegion)*25*1.0E-9;
      p_electronicNoise->initialize( randSvc, Rndm::Poisson(noiseCounts));	
      m_electronicNoise.push_back(p_electronicNoise);	
      MuonPhysicalChannelResponse* response= new
        MuonPhysicalChannelResponse(&m_flatDist,&m_gaussDist,
                                    m_time,p_electronicNoise,min,max,
                                    muReadout, readout );
      responseVector[readout][indexRegion]=response;
    }} // for loop, fix for w2k											
  }
}	 
MuonDetectorResponse::~MuonDetectorResponse(){
  for(int indexRegion=0;indexRegion<m_partition;indexRegion++){
	  delete responseChamber[indexRegion];
		delete m_poissonDist[indexRegion];
    for(int readout=0;readout<=m_pGetInfo->getReadoutNumber(indexRegion);
        readout++){
      delete responseVector[readout][indexRegion];      
		}
	}
  std::vector<Rndm::Numbers*>::iterator  iterJitter;
  for(iterJitter=m_timeJitter.begin();iterJitter<m_timeJitter.end();
      iterJitter++){
    (*iterJitter)->finalize();
    delete *iterJitter;    
  }
 
  std::vector<Rndm::Numbers*>::iterator iterNoise;	
	for(iterNoise=m_electronicNoise.begin();iterNoise<m_electronicNoise.end();
      iterNoise++){
    (*iterNoise)->finalize();
    delete *iterNoise;
	}
   if( m_pGetInfo ) m_toolSvc->releaseTool( m_pGetInfo );
   // delete m_pGetInfo;
  
}


MuonPhysicalChannelResponse* MuonDetectorResponse::getResponse(MuonPhChID& 
                                                               phChID ){
  int partition=phChID.getStation()*4+phChID.getRegion();
  unsigned int readoutType=phChID.getReadout();
  int readout=-1;
  for(int ireadout=0;ireadout<=m_pGetInfo->
        getReadoutNumber(partition);ireadout++){
    if(readoutType==m_pGetInfo->getReadoutType(ireadout, partition)){
      readout=(int)ireadout;
    }   
  }
  //	if(partition==4)cout<<" dentro detector response "<<readoutType<<" " 
  //<< readout<<endl;
  if(readout>=0){    
    return responseVector[readout][partition]; 
  }else{
    return 0; }	
}

MuonPhysicalChannelResponse* MuonDetectorResponse::getResponse(int partition, 
                                                               int readout  )
{
 	return responseVector[readout][partition]; 
}
