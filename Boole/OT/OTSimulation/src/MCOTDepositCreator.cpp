// $Id: MCOTDepositCreator.cpp,v 1.2 2004-09-10 13:14:22 cattanem Exp $

// Gaudi
#include "GaudiKernel/xtoa.h" // needed for toolName()
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"

// Event
#include "Event/MCHit.h"

// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

// OTSimulation
#include "MCOTDepositCreator.h"
#include "OTSimulation/IOTSmearer.h" 
#include "OTSimulation/IOTrtRelation.h"
#include "OTSimulation/OTDataFunctor.h"
#include "OTSimulation/IOTEffCalculator.h"
#include "OTSimulation/IOTReadOutWindow.h"
#include "OTSimulation/IOTRandomDepositCreator.h"

// xml geometry
#include "DetDesc/IGeometryInfo.h"

static const AlgFactory<MCOTDepositCreator> s_Factory;
const IAlgFactory& MCOTDepositCreatorFactory = s_Factory;

/** @file MCOTDepositCreator.cpp 
 *
 *  Implementation of MCOTDepositCreator
 *  
 *  @author M. Needham
 *  @author adapted to the Event Model by: J. van Tilburg 
 *          jtilburg@nikhef.nl (03-04-2002)
 *  @date   19-09-2000
 */

MCOTDepositCreator::MCOTDepositCreator(const std::string& name,
                                       ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_tempDeposits(0)
{
  // constructor 

  // jobOptions - defaults
  m_spillVector.push_back("/PrevPrev/");
  m_spillVector.push_back("/Prev/");
  m_spillVector.push_back("/");
  m_spillVector.push_back("/Next/");

  m_spillTimes.push_back(-50.0*ns);
  m_spillTimes.push_back(-25.0*ns);
  m_spillTimes.push_back(0.0*ns);
  m_spillTimes.push_back(25.0*ns);
  
  declareProperty("spillVector", m_spillVector);
  declareProperty("spillTimes", m_spillTimes);

  declareProperty("addCrosstalk", m_addCrossTalk = true);
  declareProperty("crossTalkLevel", m_crossTalkLevel = 0.025);
  declareProperty("addNoise", m_addNoise = true);
  declareProperty("noiseToolName",m_noiseToolName = "OTRandomDepositCreator" );

  // container for temporary digit storage 
  m_tempDeposits = new MCOTDepositVector();

  // reserve some space
  m_tempDeposits->reserve(8000);
}


MCOTDepositCreator::~MCOTDepositCreator()
{
  // destructor
  delete m_tempDeposits;
}

StatusCode MCOTDepositCreator::initialize()
{
  // initialization phase
  StatusCode sc;

  // sanity checks
  if  (m_spillVector.size() == 0) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "No spills selected to be digitized !" 
        << endreq;
    return StatusCode::FAILURE;
  }
  
  // Loading OT Geometry from XML
  SmartDataPtr<DeOTDetector> tracker( detSvc(), "/dd/Structure/LHCb/OT" );
  if ( !tracker ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Unable to retrieve Tracker detector element"
        << " from xml." << endreq;
    return StatusCode::FAILURE;
  }
  m_tracker = tracker;
  m_numStations = m_tracker->numStations();
  m_firstOTStation = m_tracker->firstOTStation();  

  // make/retrieve tools

  int numOTStations = m_numStations - m_firstOTStation + 1;
  // retrieve efficiency calculator tool
  for (int iEff = 0 ; iEff < numOTStations; ++iEff) {
    // make name
    IOTEffCalculator* aSingleCellEff = 0;
    std::string aName = toolName("effCalculator", iEff);
    // get tool
    sc = toolSvc()->retrieveTool("OTEffCalculator",aName,aSingleCellEff,this);
    if( sc.isFailure() ) {
      MsgStream msg(msgSvc(), name());
      msg << MSG::FATAL << " Unable to create OTEffCalculator tool" << endreq;
      return sc;
    }
    // add tool to vector
    m_singleCellEffVector.push_back(aSingleCellEff);
  } //loop eff calculators

  // smearer
  for (int iSmearTool = 0; iSmearTool < numOTStations; ++iSmearTool){
    // make name
    IOTSmearer* aSmearer = 0;
    std::string aName = toolName("smearer", iSmearTool);
    // get tool
    sc = toolSvc()->retrieveTool("OTSmearer",aName,aSmearer,this);
    if( sc.isFailure() ) {
      MsgStream msg(msgSvc(), name());
      msg << MSG::FATAL << " Unable to create OTSmearer tool" << endreq;
      return sc;
    }
    else {
      // add tool to vector
      m_smearerVector.push_back(aSmearer);
    }
  } // loop smearing tools

  // make r-t calculators
  for(int iRT = 0; iRT < numOTStations; ++iRT){
    // make name
    IOTrtRelation* aRTrelation = 0;
    std::string aName = toolName("rtRelation", iRT);
    // get tool
    sc = toolSvc()->retrieveTool("OTrtRelation",aName,aRTrelation,this);
    if( sc.isFailure() ) {
      MsgStream msg(msgSvc(), name());
      msg << MSG::FATAL << " Unable to create  tool OTrtRelation" << endreq;
      return sc;
    }
      // add tool to vector
    m_rtRelationVector.push_back(aRTrelation);   
  } //  loop r-t calculators

  // flat rand dist for crosstalk
  sc = m_flatDist.initialize( randSvc(), Rndm::Flat(0.0,1.0));
  if ( !sc.isSuccess() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL 
        << "Unable to initialize rdnm Service" << endreq;
    return sc;
  }

  // Read out window tool
  IOTReadOutWindow* aReadOutWindow = 0;
  sc = toolSvc()->retrieveTool("OTReadOutWindow",aReadOutWindow);
  if( !sc.isSuccess() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL << " Unable to create OTReadOutWindow tool" << endreq;
    return sc;
  }
  m_sizeOfReadOutGate = aReadOutWindow->sizeOfReadOutGate();
  m_startReadOutGate  = aReadOutWindow->startReadOutGate();
  toolSvc()->releaseTool( aReadOutWindow );

  // construct container names once
  std::vector<std::string>::const_iterator iSpillName = m_spillVector.begin();
  while (iSpillName!=m_spillVector.end()){
    // path in Transient data store
    std::string mcHitPath = "/Event"+(*iSpillName)+MCHitLocation::OTHits;
    m_spillNames.push_back(mcHitPath);
    ++iSpillName;
  } // iterSpillName

  sc = toolSvc()->retrieveTool(m_noiseToolName,
                               m_noiseToolName, m_noiseTool, this);
  if( sc.isFailure() || 0 == m_noiseTool) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL << " Unable to create noise tool" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode MCOTDepositCreator::execute(){

  // execute once per event
  StatusCode sc;

  // init the message service
  MsgStream msg(msgSvc(), name());

  // make initial list of deposits
  msg << MSG::DEBUG << "make digitizations from MCHits" << endreq;
  sc = makeDigitizations();
  if (sc.isFailure()){
    msg << MSG::ERROR << "failed to make digitizations" << endreq;
    return StatusCode::FAILURE;
  }

  // single cell eff
  msg << MSG::DEBUG << "deposits size before applying efficiency = " 
      << m_tempDeposits->size() << endreq;
  sc = singleCellEff();
  if (sc.isFailure()){
    msg << MSG::ERROR << "problems applying single cell eff" << endreq;
    return StatusCode::FAILURE;
  }
  msg << MSG::DEBUG << "deposits size after applying efficiency = " 
      << m_tempDeposits->size() << endreq;

  // smear
  msg << MSG::DEBUG << "apply single cell smearing" << endreq;
  sc = applySmear();
  if (sc.isFailure()){
    msg << MSG::ERROR << "problems applying single cell res smear" << endreq;
    return StatusCode::FAILURE;
  }  

  // r-to-t
  msg << MSG::DEBUG << "convert r-to t" << endreq;
  sc = applyRTrelation();
  if (sc.isFailure()){
    msg << MSG::ERROR << "problems applying single cell rt relation" << endreq;
    return StatusCode::FAILURE;
  }  

  // add crosstalk
  if (m_addCrossTalk) {
    msg << MSG::DEBUG << "deposits size before adding XTalk = "
        << m_tempDeposits->size() << endreq;
    sc = addCrossTalk();
    if (sc.isFailure()){
      msg << MSG::ERROR << "problems applying crosstalk" << endreq;
      return StatusCode::FAILURE;
    }  
    msg << MSG::DEBUG << "deposits size after XTalk = " 
        << m_tempDeposits->size() << endreq;
  }

  // add random noise
  if (m_addNoise) {
    msg << MSG::DEBUG << "deposits size before adding noise = "
        << m_tempDeposits->size() << endreq;
    sc = addNoise();
    if (sc.isFailure()){
      msg << MSG::ERROR << "problems applying noise" << endreq;
      return StatusCode::FAILURE;
    }  
    msg << MSG::DEBUG << "deposits size after adding noise = " 
        << m_tempDeposits->size() << endreq;
  }

  // sort - first by channel
  std::stable_sort(m_tempDeposits->begin(),m_tempDeposits->end(),
                   OTDataFunctor::Less_by_ChannelAndTime<const MCOTDeposit*>());

  MCOTDeposits* deposits = new MCOTDeposits();
  deposits->reserve(m_tempDeposits->size());
  MCOTDepositVector::iterator iterDep;
  for ( iterDep = m_tempDeposits->begin(); iterDep != m_tempDeposits->end();
        ++iterDep ) {
    deposits->add(*iterDep);
  }
  m_tempDeposits->clear();

  // store deposit container in EvDS
  sc = this->eventSvc()->registerObject(MCOTDepositLocation::Default, deposits);
  if (!sc.isSuccess()) {
    msg << MSG::ERROR
        << "Unable to store deposit container in EvDS (sc=" << sc.getCode() 
        << ")" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}


StatusCode MCOTDepositCreator::finalize() 
{
  // Release all tools

  for( std::vector<IOTEffCalculator*>::iterator 
         itEff  = m_singleCellEffVector.begin();
         itEff != m_singleCellEffVector.end(); itEff++ ) {
    if( 0 != *itEff) toolSvc()->releaseTool(*itEff);
  }
  
  for( std::vector<IOTSmearer*>::iterator itSmear = m_smearerVector.begin();
                                itSmear != m_smearerVector.end(); itSmear++ ) {
    if( 0 != *itSmear) toolSvc()->releaseTool(*itSmear);
  }
  
  for( std::vector<IOTrtRelation*>::iterator itRt = m_rtRelationVector.begin();
                                   itRt != m_rtRelationVector.end(); itRt++ ) {
    if( 0 != *itRt) toolSvc()->releaseTool(*itRt);
  }

  if (0 != m_noiseTool) toolSvc()->releaseTool(m_noiseTool);

  return StatusCode::SUCCESS;
}


StatusCode MCOTDepositCreator::makeDigitizations()
{
  // retrieve MCTrackinghits and make first list of deposits 

  // init the message service

  for (unsigned int iSpill = 0; iSpill < m_spillNames.size(); ++iSpill){

    // retrieve a MCTrackingHits for this spill
    SmartDataPtr<MCHits> 
      monteCarloTrackerHits(eventSvc(),m_spillNames[iSpill]);

    if ( !monteCarloTrackerHits ) {
      // failed to find hits
      MsgStream msg(msgSvc(), name());
      msg << MSG::DEBUG << "Unable to retrieve "+m_spillNames[iSpill] << endreq;
    }
    else {
      // found spill - create some digitizations and 
      // add them to deposits
      MCHits::const_iterator iterHit;
      for ( iterHit = monteCarloTrackerHits->begin(); 
            iterHit != monteCarloTrackerHits->end(); ++iterHit) {

        // time offset
        double tTimeOffset = (*iterHit)->timeOfFlight() + m_spillTimes[iSpill];

        // make deposits
        std::vector<OTChannelID> channels;
        std::vector<double> driftDistances;
        StatusCode sc = m_tracker->calculateHits( (*iterHit)->entry(), 
                                                  (*iterHit)->exit(),
                                                  channels, driftDistances );
        if ( !sc.isSuccess() ) continue;
        
        std::vector<OTChannelID>::const_iterator iterChan;
        std::vector<double>::const_iterator iterDist = driftDistances.begin();
        for ( iterChan = channels.begin(); iterChan != channels.end();
              ++iterChan) {
          double absDist = fabs(*(iterDist));
          int ambiguity = 1;
          if ( *iterDist < 0.0 ) ambiguity = -1;
          MCOTDeposit* deposit = new MCOTDeposit(*iterHit, 
                                                 *iterChan,
                                                 tTimeOffset,
                                                 absDist,
                                                 ambiguity);
          ++iterDist;
          m_tempDeposits->push_back(deposit);
        } // channels
      } // iterHit
    }  
  } // loop spills

  return StatusCode::SUCCESS;
}


StatusCode MCOTDepositCreator::singleCellEff() 
{
  // apply single cell efficiency

  // initialize
  StatusCode sc = StatusCode::SUCCESS;

  MCOTDepositVector::iterator iterDeposit = m_tempDeposits->begin();
  while (iterDeposit != m_tempDeposits->end() ) {
    
    // number of tool - maybe there is no outer tracker station 1
    int iEffTool = (*iterDeposit)->channel().station() 
                   - m_firstOTStation;

    bool iAccept = false;
    sc = m_singleCellEffVector[iEffTool]->calculate(*iterDeposit,iAccept);
    if (sc.isFailure()) return sc;
    if (iAccept == true){
      ++iterDeposit;
    }
    else{
      delete *iterDeposit;
      iterDeposit = m_tempDeposits->erase(iterDeposit);
    }

  } // loop m_tempDeposits  

  return sc;
}

StatusCode MCOTDepositCreator::applySmear()
{
  // smear m_tempDeposits to simulate single cell resolution

  // initialize
  StatusCode sc = StatusCode::SUCCESS;

  MCOTDepositVector::iterator iterDeposit = m_tempDeposits->begin();
  while (iterDeposit != m_tempDeposits->end()){
    
    // number of tool - there is no outer tracker station 1
    int iSmearTool = (*iterDeposit)->channel().station() - m_firstOTStation;

    // smear      
    sc = m_smearerVector[iSmearTool]->smear(*iterDeposit);
    if (sc.isFailure()) return sc;

    // hack as drift dist can < 0 due to smearing
    double driftDist = (*iterDeposit)->driftDistance();
    if (driftDist < 0.){
      (*iterDeposit)->setDriftDistance( fabs(driftDist) );
      int ambiguity = (*iterDeposit)->ambiguity();
      (*iterDeposit)->setAmbiguity(-1 * ambiguity);
    }

    ++iterDeposit;
  } // loop m_tempDeposits  

  return sc;
}

StatusCode MCOTDepositCreator::applyRTrelation()
{
  // apply r-t relation
  
   // intialize
  StatusCode sc = StatusCode::SUCCESS;

  MCOTDepositVector::iterator iterDeposit = m_tempDeposits->begin();
  while (iterDeposit != m_tempDeposits->end()){
    
    // number of tool - there is no outer tracker station 1
    int iRTTool = (*iterDeposit)->channel().station() - m_firstOTStation;

    // convert      
    sc = m_rtRelationVector[iRTTool]->convertRtoT(*iterDeposit);
    if (sc.isFailure()) return sc;

    ++iterDeposit;
  } // loop m_tempDeposits  

  return sc;
}

StatusCode MCOTDepositCreator::addCrossTalk()
{
  // Add cross talk to deposits
  std::list<MCOTDeposit*> crossTalkList;
  
  MCOTDepositVector::const_iterator iterDeposit = m_tempDeposits->begin();
  while (iterDeposit != m_tempDeposits->end()){
 
    // channel
    OTChannelID aChannel = (*iterDeposit)->channel();

    // find right chan
    std::list<OTChannelID> neighbours;
    OTChannelID nextRight = m_tracker->nextChannelRight(aChannel);
    if ( nextRight.channelID() !=0 ) {
      neighbours.push_back(nextRight);
    } 
    // find right left
    OTChannelID nextLeft = m_tracker->nextChannelLeft(aChannel);
    if ( nextLeft.channelID() !=0 ) {
      neighbours.push_back(nextLeft);
    } 

    std::list<OTChannelID>::iterator iterChan = neighbours.begin();
    while (iterChan != neighbours.end()) {
      double testVal = m_flatDist();
      if ( testVal < m_crossTalkLevel) {

        // crosstalk in neighbour - copy hit - this is very ugly
        MCOTDeposit* newDep = new MCOTDeposit(0, *iterChan,
                                 (*iterDeposit)->tdcTime(),
                                 (*iterDeposit)->driftDistance(),
                                 (*iterDeposit)->ambiguity());
        
        crossTalkList.push_back(newDep);
       } 

      ++iterChan;
    } //iterChan

    ++iterDeposit;
  } // iterDeposit

  // move hits from crosstalk list to deposit vector
  std::list<MCOTDeposit*>::iterator iterTalk = crossTalkList.begin();
  while (iterTalk != crossTalkList.end()){
    m_tempDeposits->push_back(*iterTalk);
    ++iterTalk;
  }  // iterTalk

  return StatusCode::SUCCESS;
}

// Add random noise deposits. 
StatusCode MCOTDepositCreator::addNoise()
{
  m_noiseTool->createDeposits(m_tempDeposits);

  return StatusCode::SUCCESS;
}

std::string MCOTDepositCreator::toolName(const std::string& aName, 
                                         const int id) const
{
  // convert id to station number
  int iStation = id + m_firstOTStation;
 
  char buffer[32];
  return  aName+ ::_itoa(iStation,buffer,10);
}
