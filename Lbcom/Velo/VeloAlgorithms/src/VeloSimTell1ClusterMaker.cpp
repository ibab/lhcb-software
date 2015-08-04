// Include files 

#include "Event/RawEvent.h"

// stl
#include <vector>
#include <algorithm>

// local
#include "VeloSimTell1ClusterMaker.h"
#include "ThresholdMaker.h"

// engine class
#include "SimTell1ZSProcessEngine.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloSimTell1ClusterMaker
//
// 2009-08-06 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloSimTell1ClusterMaker )

using namespace boost::assign;
using namespace VeloTELL1;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloSimTell1ClusterMaker::VeloSimTell1ClusterMaker( const std::string& name,
    ISvcLocator* pSvcLocator)
: VeloSimTell1Algorithm ( name , pSvcLocator ),
  m_rawEvent ( 0 ),
  m_rawEventLoc ( LHCb::RawEventLocation::Default ),
  m_adcs ( ),
  m_adcsMem ( ),
  m_zsEnableMap ( ),
  m_hitThresholdMap ( ),
  m_lowThresholdMap ( ),
  m_sumThresholdMap ( ),
  m_boundaryStripMap ( ),
  m_eventNumber ( 0 ),
  m_sensors ( ),
  m_bankBodySize ( 0 ),
  m_isDebug ( msgLevel(MSG::DEBUG) ),
  m_zsEngines ( ),
  m_clusters ( ),
  m_clustersMem ( )
{

  // names and types
  setTELL1Process(CLUSTER_MAKER);
  setAlgoName("CLUSTER_MAKER");
  setAlgoType("TELL1 Cluster Maker");

  // fill vector with number of first strips in each processing channel
  m_boundaryStripsR+=0, 64, 128, 192, 256, 320, 384, 448, 512, 512,
    576, 640, 704, 768, 832, 896, 960, 1024, 1024,
    1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536,
    1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 0;
  m_boundaryStripsPhi+=0, 64, 128, 683, 747, 811, 875, 939, 1003, 171,
    235, 299, 1024, 1088, 1152, 1216, 1280, 1344,
    342, 406, 470, 1365, 1429, 1493, 1557, 1621, 
    1685, 512, 576, 640, 1707, 1771, 1835, 1899, 
    1963, 2027;

  // algo atributes
  declareProperty("InputDataLoc",
      m_inputDataLoc=LHCb::VeloTELL1DataLocation::SimADCs );
  declareProperty("PrintInfo", m_printInfo=false);
  declareProperty("HitThresholdValue",
                  m_hitThresholdValue=12);
  declareProperty("LowThresholdValue",
                  m_lowThresholdValue=5);
  declareProperty("SumThresholdValue",
                  m_sumThresholdValue=20);
  declareProperty("ZSProcessEnable", m_zsProcessEnable=1);
  declareProperty("DBConfig", m_dbConfig=VeloTELL1::DYNAMIC);
  declareProperty("SrcIdList", m_srcIdList);
  declareProperty("BankVersion", m_bankVersion=V3);
  declareProperty("HitSigmas", m_hitSigmas=6.5);
  declareProperty("IncSigmas", m_incSigmas=0.45);

}
//=============================================================================
// Destructor
//=============================================================================
VeloSimTell1ClusterMaker::~VeloSimTell1ClusterMaker() {}
//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloSimTell1ClusterMaker::initialize() {
  StatusCode sc = VeloSimTell1Algorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  //
  
//   IUpdateManagerSvc* mgrSvc=svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
//   mgrSvc->registerCondition(this, m_condPath,
//                             &VeloSimTell1ClusterMaker::i_cacheConditions);
//   StatusCode mgrSvcStatus=mgrSvc->update(this);
//   if(mgrSvcStatus.isFailure()){
//     return ( Error("Failed first UMS update", mgrSvcStatus) );
//   }

  if(m_dbConfig==VeloTELL1::DYNAMIC){

    IUpdateManagerSvc* mgrSvc=svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
    mgrSvc->registerCondition(this, m_condPath,
                              &VeloSimTell1ClusterMaker::i_cacheConditions);
    StatusCode mgrSvcStatus=mgrSvc->update(this);

    if(mgrSvcStatus.isFailure()){

      return ( Error("Failed first UMS update", mgrSvcStatus) );

    }

  }else if(m_dbConfig==VeloTELL1::STATIC){

    // initialize maps with static values
    ThresholdsVec tempHit(2304, m_hitThresholdValue);
    m_hitThresholds=tempHit;
    ThresholdsVec tempLow(2304, m_lowThresholdValue);
    m_lowThresholds=tempLow;
    SumThresholdsVec tempSum(36, m_sumThresholdValue);
    m_sumThresholds=tempSum;
    //
    m_zsEnableMap[0]=m_zsProcessEnable;
    m_hitThresholdMap[0]=m_hitThresholds;
    m_lowThresholdMap[0]=m_lowThresholds;
    m_sumThresholdMap[0]=m_sumThresholds;
    // boundary strips will be passed to the engine object later on
    // depending on engine type - this is only the issue for static
    // configuration, in the case of dynamic config this will be 
    // dealt with automatically

  }

  //
  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloSimTell1ClusterMaker::execute() {

  if(m_isDebug) debug() << "==> Execute" << endmsg;

  ++m_eventNumber;  

  StatusCode engineStatus(StatusCode::SUCCESS); // explicitly set to default

  if(!isInitialized()){

    StatusCode initStatus =  INIT();
    if( initStatus.isFailure() ) {

      if(m_isDebug) debug()<< " --> Unable to initialize the algorithm!."
                           << "  NZS data may be missing!" <<endmsg;
      return ( StatusCode::SUCCESS );

    } 

    engineStatus=createAndConfigureEngines();

    if(engineStatus.isFailure()){
      
      if(m_isDebug) debug()<< " --> Problem with processing engines!" <<endmsg;
      return ( StatusCode::FAILURE );
      
    }

    if(m_forceEnable) { // fix int -> bool warning on Windows

      this->setIsEnable(m_zsProcessEnable != 0 ? true : false);

    }

    if(isEnable()){

      info()<< " --> Algorithm " << (this->algoName())
            << " of type: " << (this->algoType())
            << " is enabled and ready to process data --" <<endmsg;

    }else{

      info()<< " --> Algorithm " << (this->algoName())
            << " is disabled! " <<endmsg;

    }

  }

  StatusCode rawEvtStatus=createRawEvent();

  if((isEnable())&&(engineStatus.isSuccess())){

    StatusCode dataStatus=getData();

    if((dataStatus.isSuccess())&&(rawEvtStatus.isSuccess())){

      dataStatus=inputStream(inputData());

      if(dataStatus.isSuccess()){

        StatusCode cluMakerStatus=runClusterMaker();

        if(cluMakerStatus.isSuccess()){ 

          fillAndWriteRawEvent();

        }else{

          Error(" The cluster maker was not successful! ");
          return ( cluMakerStatus );

        }

      }

    }

    flushMemory();

  }

  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloSimTell1ClusterMaker::finalize() {

  if(m_isDebug) debug() << "==> Finalize" << endmsg;
  // must be called after all other actions
  std::map<unsigned int, SimTell1ZSProcessEngine*>::iterator zsIt;
  zsIt=m_zsEngines.begin();
  for( ; zsIt!=m_zsEngines.end(); ++zsIt){
    if(zsIt->second!=NULL) delete zsIt->second;
  }
  //
  return VeloSimTell1Algorithm::finalize();
}
//=============================================================================
StatusCode VeloSimTell1ClusterMaker::createRawEvent()
{
  if(m_isDebug) debug()<< " createRawEvent() " <<endmsg;
  // create raw event - that is the output of the algorithm 
  if(exist<LHCb::RawEvent>(m_rawEventLoc)){
    m_rawEvent=get<LHCb::RawEvent>(m_rawEventLoc);
  }else{
    // create RawEvent structure
    m_rawEvent=new LHCb::RawEvent();
    eventSvc()->registerObject(m_rawEventLoc, m_rawEvent);
    if(m_isDebug) debug()<< " registered RawEvent " <<endmsg;
  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
StatusCode VeloSimTell1ClusterMaker::runClusterMaker()
{
  if(m_isDebug) debug()<< " ==> runClusterMaker() " <<endmsg;
  //
  LHCb::VeloTELL1Datas::iterator sensIt=inputData()->begin();

  //sensIt=inputData()->begin();
  for( ; sensIt!=inputData()->end(); ++sensIt){
    const unsigned int tell1=static_cast<unsigned int>((*sensIt)->key());
    const DeVeloSensor* sens=deVelo()->sensorByTell1Id(tell1);
    // check if the sensor pointer is valid
    if(sens==0){
      Error(" ==> No match between TELL1 and sensor found! ");
      Error(" ==> Check your XML/DB conditions file! ");
      return ( StatusCode::FAILURE );
    }
    unsigned int sensNb=sens->sensorNumber();
    if(m_isDebug) debug() << " the sensor number: " << sensNb 
                          << " for TELL1 number: " << tell1 <<endmsg;
    if(sens->isReadOut()){
      // cache the current read-out sensor number
      if(m_eventNumber>=convergenceLimit()){
        m_sensors.push_back(tell1); 
        if(m_isDebug) debug()<< " added TELL1 number: " << tell1
                             << " to cache." << endmsg;
      }
    }
    m_clusters.clear();
    m_adcs.clear();
    // get info about current sensor
    // input data for clusterization
    if(sens->isReadOut()){
      VeloTELL1::sdataVec data=(*sensIt)->data();
      // --> check if there is any additional data source
      if(m_zsEngines.find(tell1)==m_zsEngines.end()){
        if(m_isDebug)
          debug()<< " --> found new data source, will add processing engine "
              <<endmsg;
        if(m_dbConfig==VeloTELL1::STATIC){
          this->addEngineInStaticMode(tell1, sens);
        }else if(m_dbConfig==VeloTELL1::DYNAMIC){
          this->addEngineInDynamicMode(tell1);
        }
      }
      m_zsEngines[tell1]->setInData(data);
      m_zsEngines[tell1]->setOutClusters(m_clusters);
      m_zsEngines[tell1]->setOutADCS(m_adcs);
      m_zsEngines[tell1]->runZeroSuppression();
    }
    // store clusters, adcs and corresponding counters in memory
    m_clustersMem.push_back(m_clusters);
    m_adcsMem.push_back(m_adcs);   
  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
void VeloSimTell1ClusterMaker::flushMemory()
{
  if(m_isDebug) debug()<< " ==> flushMemory() " <<endmsg;
  //
  m_clustersMem.clear();
  m_adcsMem.clear();
  m_sensors.clear();
}
//=============================================================================
void VeloSimTell1ClusterMaker::fillAndWriteRawEvent()
{
  if(m_isDebug) debug()<< " ==> fillAndWriteRawEvent() " <<endmsg;
  //
  // coding of the RawBank bank body: header word, clusters and adcs
  // create one RawBank per sensor, to do so loop over container
  // with sensor numbers, use const_iterator - see Kernel/DecodeCore.h
  unsigned int rawBankCnt=0;
  cdatIt IT=m_sensors.begin();
  if(m_isDebug) debug()<< " size of sens vec: " << m_sensors.size() <<endmsg;
  if(m_isDebug) debug()<< " first sens: " << (*IT) <<endmsg;
  for( ; IT!=m_sensors.end(); ++IT){
    // clear bank body for next bank
    m_bankBody.clear();
    TELL1ClusterVec clusters=m_clustersMem[rawBankCnt];
    unsigned int clusterNumber=clusters.size();
    // create and sort a header word
    SiHeaderWord aHWord(clusterNumber, 0, 0);
    SiDAQ::buffer_word aHeader=aHWord.value();
    //buffer_word aHeader=aHWord.value();
    m_bankBody.push_back(aHeader);
    // check if clusters were produced for current sensor
    // check if the cluster number is even
    unsigned bufferWords=0;
    if((clusterNumber)&&(clusterNumber%CLUSTER_PER_WORD==0)){
      // calculate number of buffer_words needed to code clusters
      bufferWords=
        static_cast<unsigned int>(floor(
              static_cast<double>(clusterNumber)/
              static_cast<double>(CLUSTER_PER_WORD)));
      for(unsigned int word=0; word<bufferWords; ++word){
        SiDAQ::buffer_word codedClusters=0x0, tempBuffWord=0x0;
        //buffer_word codedClusters=0x0, tempBuffWord=0x0;
        for(int cluCnt=0; cluCnt<CLUSTER_PER_WORD; ++cluCnt){
          tempBuffWord=clusters[CLUSTER_PER_WORD*word+cluCnt];
          tempBuffWord<<=(cluCnt*CLUSTER_SHIFT);
          codedClusters|=tempBuffWord;
        }
        if(m_printInfo) bitReader<VeloTELL1::u_int32_t>(codedClusters);
        m_bankBody.push_back(codedClusters);
      }
    }else if((clusterNumber)&&((clusterNumber%CLUSTER_PER_WORD!=0))){
      unsigned int fullWords=
        static_cast<unsigned int>(floor(
              static_cast<double>(clusterNumber)/
              static_cast<double>(CLUSTER_PER_WORD)));
      // take into account last cluster
      bufferWords=fullWords+1;
      for(unsigned int word=0; word<bufferWords; ++word){
        SiDAQ::buffer_word codedClusters=0x0, tempBuffWord=0x0;
        //buffer_word codedClusters=0x0, tempBuffWord=0x0;
        // write first full words (2 clusters per word)
        if(clusterNumber==1){
          codedClusters|=clusters[CLUSTER_PER_WORD*word];
          //codedClusters<<=CLUSTER_SHIFT;
          if(m_printInfo) bitReader<VeloTELL1::u_int32_t>(codedClusters);
          m_bankBody.push_back(codedClusters);
        }else if((clusterNumber>1)&&(word<fullWords)){
          for(int cluCnt=0; cluCnt<CLUSTER_PER_WORD; ++cluCnt){
            tempBuffWord=clusters[CLUSTER_PER_WORD*word+cluCnt];
            tempBuffWord<<=(cluCnt*CLUSTER_SHIFT);
            codedClusters|=tempBuffWord;
          }
          if(m_printInfo) bitReader<VeloTELL1::u_int32_t>(codedClusters);
          m_bankBody.push_back(codedClusters);
        }else if((clusterNumber>1)&&(word==fullWords)){
          codedClusters|=clusters[CLUSTER_PER_WORD*word];
          if(m_printInfo) bitReader<VeloTELL1::u_int32_t>(codedClusters);
          m_bankBody.push_back(codedClusters);
        }
      }
    }
    // store adcs in raw bank
    TELL1ADCVec adcs=m_adcsMem[rawBankCnt];
    unsigned int adcNumber=adcs.size();
    if(clusterNumber&&(adcNumber%ADCS_PER_WORD==0)){
      // calculate number of buffer_words needed to code the adcs 
      bufferWords=
        static_cast<unsigned int>(floor(
              static_cast<double>(adcNumber)/static_cast<double>(ADCS_PER_WORD)));
      for(unsigned int word=0; word<bufferWords; ++word){
        SiDAQ::buffer_word codedADCS=0x0, tempBuffWord=0x0;
        //buffer_word codedADCS=0x0, tempBuffWord=0x0;
        for(int adcCnt=0; adcCnt<ADCS_PER_WORD; ++adcCnt){
          tempBuffWord=adcs[ADCS_PER_WORD*word+adcCnt];
          tempBuffWord<<=(adcCnt*ADC_SHIFT);
          codedADCS|=tempBuffWord;
          //if(adcCnt<ADCS_PER_WORD-1) codedADCS<<=ADC_SHIFT;
        }
        m_bankBody.push_back(codedADCS);
      } 
    }else if(clusterNumber&&(adcNumber%ADCS_PER_WORD!=0)){
      unsigned int fullWords=
        static_cast<unsigned int>(floor(
              static_cast<double>(adcNumber)/static_cast<double>(ADCS_PER_WORD)));
      // take into account remaining adcs
      bufferWords=fullWords+1;
      for(unsigned int word=0; word<bufferWords; ++word){
        SiDAQ::buffer_word codedADCS=0x0, tempBuffWord=0x0;
        //buffer_word codedADCS=0x0, tempBuffWord=0x0;
        // write first full word (4 adcs per word)
        if(word<fullWords){
          for(int adcCnt=0; adcCnt<ADCS_PER_WORD; ++adcCnt){
            if(m_isDebug) debug()<< " adc: " 
                                 << adcs[ADCS_PER_WORD*word+adcCnt] <<endmsg;
            tempBuffWord=adcs[ADCS_PER_WORD*word+adcCnt];
            tempBuffWord<<=(adcCnt*ADC_SHIFT);
            if(m_printInfo) bitReader<VeloTELL1::u_int32_t>(tempBuffWord);
            codedADCS|=tempBuffWord;
            //if(adcCnt<ADCS_PER_WORD-1) codedADCS<<=ADC_SHIFT;
            if(m_printInfo) bitReader<VeloTELL1::u_int32_t>(codedADCS);
          }
          m_bankBody.push_back(codedADCS);
        }else if(word==fullWords){
          // calculate number of adcs for the last buffer word
          unsigned int lastADCS=adcNumber%ADCS_PER_WORD;
          for(unsigned int adcCnt=0; adcCnt<lastADCS; ++adcCnt){
            tempBuffWord=adcs[ADCS_PER_WORD*word+adcCnt];
            tempBuffWord<<=(adcCnt*ADC_SHIFT);
            if(m_printInfo) bitReader<VeloTELL1::u_int32_t>(tempBuffWord);
            codedADCS|=tempBuffWord;
            //codedADCS<<=ADC_SHIFT;
            if(m_printInfo) bitReader<VeloTELL1::u_int32_t>(codedADCS);
          }
          m_bankBody.push_back(codedADCS);
        }
      }
    }
    // calculate bank size in bytes:
    int paddingSpace=adcNumber%ADCS_PER_WORD;
    m_bankBodySize=sizeof(SiDAQ::buffer_word)*m_bankBody.size()-
      (paddingSpace?sizeof(SiDAQ::buffer_word)-
       paddingSpace*sizeof(VeloTELL1::u_int8_t):0);
    // some checks
    for(unsigned int i=0; i<m_bankBody.size(); ++i){
      if(m_isDebug) debug()<< " m_bankBody[" << i << "]=" << m_bankBody[i] <<endmsg;
    }
    // store raw bank
    LHCb::RawBank* aBank=
      m_rawEvent->createBank(static_cast<SiDAQ::buffer_word>(*IT),
          LHCb::RawBank::Velo, m_bankVersion, m_bankBodySize, &(m_bankBody[0]));
    // 
    m_rawEvent->adoptBank(aBank,true);
    rawBankCnt++;
  }
  //
  return;
}
//=============================================================================
StatusCode VeloSimTell1ClusterMaker::createAndConfigureEngines()
{
  if(m_isDebug) debug()<< " ==> prepareEngine() "<<endmsg;
  // pedestal engine
  VeloTELL1::AListPair idList=this->getSrcIdList();
  VeloTELL1::ucIT iT=idList.first;
  if(iT==idList.second){
    return Error(" ==> Empty list with src Ids!");
  }else{
    // create engines and put them into the map with tell1 as keys
    if(m_dbConfig==VeloTELL1::STATIC){
      info()<< " --> Creating static config for Cluster Maker! " <<endmsg;
      for( ; iT!=idList.second; ++iT){
        const DeVeloSensor* sens=deVelo()->sensorByTell1Id(*iT);
        if(m_isDebug) debug()<< " sensor from srcIdList: " << (*iT) <<endmsg;
        // check if the sensor pointer is valid
        if(sens==0){
          Error(" ==> No match between TELL1 and sensor found! ");
          Error(" ==> Check your XML/DB conditions file! ");
          return ( StatusCode::FAILURE );
        }
        this->addEngineInStaticMode(*iT, sens);
      }
    }else if(m_dbConfig==VeloTELL1::DYNAMIC){
      // all parameters are already cached - use them to config engines
      info()<< " --> Creating dynamic config for Cluster Maker! " <<endmsg;
      info()<< " --> Thresholds calculated usign SIMCOND data base " <<endmsg;
      for( ; iT!=idList.second; ++iT){
        this->addEngineInDynamicMode(*iT);
      }
    }else{
      return ( Error(" ==> Unknown configuration mode! ") );
    }
  }
  return ( StatusCode::SUCCESS );
}
//=============================================================================
StatusCode VeloSimTell1ClusterMaker::i_cacheConditions()
{
  if(m_isDebug) debug()<< " ==> i_cacheCondition() " <<endmsg;

  VeloTELL1::AListPair iniList=getSrcIdList();
  if(iniList.first==iniList.second){

    return ( Error(" --> No Tell1 List cached!", StatusCode::FAILURE) );

  }

  VeloTELL1::ucIT srcId=iniList.first;
  bool rType=false;
  VeloTELL1::scdatIt iT;
  std::vector<double> tmpdouble;
  VeloTELL1::sdataVec tmpint(2048);
  VeloTELL1::SumThresholdsVec aSumThVec(36, m_sumThresholdValue);
  
  // loop over all tell1s and initialize conditions
  for( ; srcId!=iniList.second; ++srcId){

    unsigned int tell1=(*srcId);
    const DeVeloSensor* sens=deVelo()->sensorByTell1Id(tell1);
    (sens->isR()||sens->isPileUp()) ? rType=true : rType=false;

    // build the zsEnable map - use the readout flag from the SIMCOND
    m_zsEnableMap[tell1]=static_cast<int>(sens->isReadOut());
    
    // now the real stuff with the SIMCOND - get the noise
    std::string stell1=boost::lexical_cast<std::string>(tell1);
    std::string name=m_condPath+"/Sensors/VeloSensor-"+stell1+"-StripNoise";
    Condition* cond=getDet<Condition>(name);
    tmpdouble=cond->param<std::vector<double> >("StripNoise");

    // produce thresholds - first hit 
    std::transform(tmpdouble.begin(), tmpdouble.end(), tmpint.begin(),
                   ThresholdMaker<double>(m_hitSigmas));
    this->addProcThresholds(tmpint, rType);
    VeloTELL1::ThresholdsVec aHitThVec;
    iT=m_thresholds.begin();
    for( ; iT!=m_thresholds.end(); ++iT){

      aHitThVec.push_back(static_cast<VeloTELL1::u_int8_t>(*iT));

    }
    m_hitThresholdMap[tell1]=aHitThVec;

    // and then inclusion
    std::transform(tmpdouble.begin(), tmpdouble.end(), tmpint.begin(),
                   ThresholdMaker<double>(m_hitSigmas*m_incSigmas));
    this->addProcThresholds(tmpint, rType);
    m_lowThresholdMap[tell1]=m_thresholds;    
        
    // sum thresholds - keep this assignmet here for consistency
    m_sumThresholdMap[tell1]=aSumThVec;

    // as above for the sum thresholds - boundary strips
    VeloTELL1::BoundaryStripVec aBdrStripVec;
    rType ? aBdrStripVec=m_boundaryStripsR : aBdrStripVec=m_boundaryStripsPhi;
    m_boundaryStripMap[tell1]=aBdrStripVec;

  }

  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
void VeloSimTell1ClusterMaker::addEngineInStaticMode(unsigned int tell1,
                                                     const DeVeloSensor* sens)
{
  if(m_isDebug) debug()<< " --> addEngineInStaticMode() " <<endmsg;
  //
  m_zsEngines[tell1]=new SimTell1ZSProcessEngine();
  m_zsEngines[tell1]->setProcessEnable(m_zsEnableMap[0]);
  m_zsEngines[tell1]->setHitThresholds(m_hitThresholdMap[0]);
  m_zsEngines[tell1]->setLowThresholds(m_lowThresholdMap[0]);
  m_zsEngines[tell1]->setSumThresholds(m_sumThresholdMap[0]);
  if(sens->isR()||sens->isPileUp()){
    // processing R sensor set start strip sequence for R
    m_zsEngines[tell1]->setBoundaryStrips(m_boundaryStripsR);
  }else if(sens->isPhi()){
    // processing Phi sensor
    m_zsEngines[tell1]->setBoundaryStrips(m_boundaryStripsPhi);
  }
  //
  return;
}
//=============================================================================
void VeloSimTell1ClusterMaker::addEngineInDynamicMode(unsigned int tell1)
{
  if(m_isDebug) debug()<< " --> addEngineInDynamicMode() " <<endmsg;
  //
  // pedestal processing objects
  m_zsEngines[tell1]=new SimTell1ZSProcessEngine();
  m_zsEngines[tell1]->setProcessEnable(m_zsEnableMap[tell1]);
  m_zsEngines[tell1]->setHitThresholds(m_hitThresholdMap[tell1]);
  m_zsEngines[tell1]->setLowThresholds(m_lowThresholdMap[tell1]);
  m_zsEngines[tell1]->setSumThresholds(m_sumThresholdMap[tell1]);
  m_zsEngines[tell1]->setBoundaryStrips(m_boundaryStripMap[tell1]);
  //
  return;
}
//--
