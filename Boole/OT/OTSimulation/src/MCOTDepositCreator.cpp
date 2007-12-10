// $Id: MCOTDepositCreator.cpp,v 1.25 2007-12-10 09:31:25 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Point3DTypes.h"

// OTDet
#include "OTDet/DeOTDetector.h" 
#include "OTDet/DeOTModule.h"
 
// Event
#include "Event/MCHit.h"
#include "Event/MCOTDeposit.h"

// Kernel
#include "Kernel/OTDataFunctor.h"
#include "Kernel/OTChannelID.h"

// OTDAQ
#include "OTDAQ/IOTReadOutWindow.h"

// OTSimulation
#include "MCOTDepositCreator.h"
#include "IOTSmearer.h" 
#include "IOTrtRelation.h"
#include "IOTEffCalculator.h"
#include "IOTRandomDepositCreator.h"

// Boost
#include "boost/lexical_cast.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

/** @file MCOTDepositCreator.cpp 
 *
 *  Implementation of MCOTDepositCreator
 *  
 *  @author M. Needham
 *  @author adapted to the Event Model by: J. van Tilburg 
 *          jtilburg@nikhef.nl (03-04-2002)
 *  @date   19-09-2000
 */

using namespace LHCb;
using namespace boost;
using namespace boost::lambda;
using boost::lexical_cast;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCOTDepositCreator );

MCOTDepositCreator::MCOTDepositCreator(const std::string& name,
                                       ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator),
  m_tempDeposits(0)
{
  // constructor 

  // jobOptions - defaults
  m_spillVector = boost::assign::list_of("/PrevPrev/")("/Prev/")("/")
                         ("/Next/")("/NextNext/");

  m_spillTimes = boost::assign::list_of(-50.0*Gaudi::Units::ns)
                        (-25.0*Gaudi::Units::ns)
                        (0.0*Gaudi::Units::ns)
                        (25.0*Gaudi::Units::ns)
                        (50.0*Gaudi::Units::ns);

  declareProperty("spillVector", m_spillVector);
  declareProperty("spillTimes", m_spillTimes);

  declareProperty("addCrosstalk", m_addCrossTalk = true);
  declareProperty("crossTalkLevel", m_crossTalkLevel = 0.025);
  declareProperty("addNoise", m_addNoise = true);

  declareProperty("addPulse", m_addPulse = true);
  declareProperty("PulseTime", m_PulseTime = 30*Gaudi::Units::ns);
  declareProperty("PulseProbability", m_PulseProbability = 0.3);

  declareProperty("noiseToolName",m_noiseToolName = "OTRandomDepositCreator" );

  declareProperty("smearTime", m_smearTime = true);

  // container for temporary digit storage 
  m_tempDeposits = new MCOTDepositVec();

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

  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // sanity checks
  if  (m_spillVector.empty()) return Error("No spills selected to be digitized !");
    
  // Get OT Geometry
  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  m_firstStation = m_tracker->firstStation();  
  unsigned int nStations = m_tracker->nStation();
   
  // retrieve efficiency calculator tool
  for (unsigned int iEff = 0 ; iEff < nStations; ++iEff) {
    // get tool
    std::string aName = toolName("EffCalculator", iEff);
    IOTEffCalculator* aSingleCellEff = tool<IOTEffCalculator>("OTEffCalculator", aName, this);
    // add tool to vector
    m_singleCellEffVector.push_back(aSingleCellEff);
  } //loop eff calculators

  // smearer
  for (unsigned int iSmearTool = 0; iSmearTool < nStations; ++iSmearTool){
    // get tool
    std::string aName = toolName("Smearer", iSmearTool);
    IOTSmearer* aSmearer = tool<IOTSmearer>("OTSmearer", aName, this);
    // add tool to vector
    m_smearerVector.push_back(aSmearer);
  } // loop smearing tools

  // make r-t calculators
  for (unsigned int iRT = 0; iRT < nStations; ++iRT) {
    // get tool
    std::string aName = toolName("RTrelation", iRT);
    IOTrtRelation* aRTrelation = tool<IOTrtRelation>("OTrtRelation", aName, this);
    // add tool to vector
    m_rtRelationVector.push_back(aRTrelation);   
  } //  loop r-t calculators

  // flat rand dist for crosstalk
  sc = m_flatDist.initialize( randSvc(), Rndm::Flat(0.0,1.0));
  if ( !sc.isSuccess() ) return Error("Unable to initialize rdnm Service", sc);
  
  // Read out window tool
  IOTReadOutWindow* aReadOutWindow = tool<IOTReadOutWindow>("OTReadOutWindow");
  m_sizeOfReadOutGate = aReadOutWindow->sizeOfReadOutGate();
  m_startReadOutGate  = aReadOutWindow->startReadOutGate();
  release( aReadOutWindow );

  // retrieve pointer to random number service
  IRndmGenSvc* randSvc = 0;
  sc = serviceLocator()->service( "RndmGenSvc", randSvc, true ); 
  if ( sc.isFailure() ) return Error("Failed to retrieve random number service", sc);
    

  // get interface to generator
  sc = randSvc->generator(Rndm::Flat(0.,1.0),m_genDist.pRef()); 
  if ( sc.isFailure() ) return Error("Failed to generate random number distribution", sc);
  randSvc->release();

  // construct container names once
  std::vector<std::string>::const_iterator iSpillName = m_spillVector.begin();
  while (iSpillName!=m_spillVector.end()){
    // path in Transient data store
    std::string mcHitPath = "/Event"+(*iSpillName)+MCHitLocation::OT;
    m_spillNames.push_back(mcHitPath);
    ++iSpillName;
  } // iterSpillName
  m_noiseTool = tool<IOTRandomDepositCreator>(m_noiseToolName);
  
  return StatusCode::SUCCESS;
}

StatusCode MCOTDepositCreator::execute(){

  // execute once per event

  // make initial list of deposits
  if (msgLevel(MSG::DEBUG)) debug() << "Making deposits from hits" << endmsg;
  makeDigitizations();
  
  // single cell eff
  if (msgLevel(MSG::DEBUG)) debug() << "Number of deposits before applying efficiency cut = "
                                    << m_tempDeposits->size() << endmsg;
  singleCellEff();
  if (msgLevel(MSG::DEBUG)) debug() << "Numer of deposits after applying efficiency cut = "
                                    << m_tempDeposits->size() << endmsg;

  // smear
  if (msgLevel(MSG::DEBUG))  debug() << "Applying single cell smearing" << endmsg;
  applySmear();
  
  // r-to-t
  if (msgLevel(MSG::DEBUG)) debug() << "Applying r-t relation" << endmsg;
  applyRTrelation();

  // add crosstalk
  if (m_addCrossTalk) {
    if (msgLevel(MSG::DEBUG)) debug() << "Number of deposits before adding XTalk = "
                                      << m_tempDeposits->size() << endmsg;
    addCrossTalk();
    //if (sc.isFailure()) return Error("problems applying crosstalk", sc ); 
    if (msgLevel(MSG::DEBUG))debug() << "Number of deposits after adding XTalk = "
                                     << m_tempDeposits->size() << endmsg;
  }
  
  // add random noise
  if (m_addNoise) {
    if (msgLevel(MSG::DEBUG)) debug() << "Number of deposits before adding noise = "
                                      << m_tempDeposits->size() << endmsg;
    m_noiseTool->createDeposits(m_tempDeposits);
    if (msgLevel(MSG::DEBUG)) debug() << "Number of deposits after adding XTalk = "
                                      << m_tempDeposits->size() << endmsg;
  }

  // add Double Pulse Reflection
  if (m_addPulse) {
    if (msgLevel(MSG::DEBUG)) debug() << "Number of deposits before adding double pulse = "
                                      << m_tempDeposits->size() << endmsg;
    addPulseReflect();
    if (msgLevel(MSG::DEBUG)) debug() << "Number of deposits after adding noise = "
                                      << m_tempDeposits->size() << endmsg;
  }
  
  // sort - first by channel
  std::stable_sort(m_tempDeposits->begin(),m_tempDeposits->end(),
                   OTDataFunctor::Less_by_ChannelAndTime<const MCOTDeposit*>());

  MCOTDeposits* deposits = new MCOTDeposits();
  deposits->reserve(m_tempDeposits->size());
  for_each(m_tempDeposits->begin(), m_tempDeposits->end(),
           bind(&MCOTDeposits::add, deposits, _1));
  m_tempDeposits->clear();

  // store deposit container in EvDS
  put(deposits, MCOTDepositLocation::Default);

  return StatusCode::SUCCESS;
}

void MCOTDepositCreator::makeDigitizations()
{
  // retrieve MCHits and make first list of deposits
  for (unsigned int iSpill = 0; iSpill < m_spillNames.size(); ++iSpill){
    /// Retrieve MCHits for this spill
    /// This is plain ugly. The problem is there is no NextNext so we reuse PrevPrev,
    /// which is statistically incorrect. Here we also assume that the order of the 
    /// spills is as specified above. Just hope that someone doesn't mess-up the order
    /// in the options.
    SmartDataPtr<MCHits> otMCHits( eventSvc(), m_spillNames[iSpill<4?iSpill:0] );

    /// Can't assume that there are hits in spills
    if (!otMCHits) {
      if (msgLevel(MSG::DEBUG)) debug() << "Spillover missing in the loop " +m_spillNames[iSpill] <<endmsg;
    } else {
      // found spill - create some digitizations and add them to deposits
      for (MCHits::const_iterator iHit = otMCHits->begin(), iHitEnd = otMCHits->end(); iHit != iHitEnd; ++iHit) {
        MCHit* aMCHit = (*iHit);
        
        // time offset
        const double tTimeOffset = aMCHit->time() + m_spillTimes[iSpill];

        // make deposits
        std::vector<std::pair<OTChannelID, double> > chanAndDist;
        const DeOTModule* aModule = m_tracker->findModule(aMCHit->midPoint());
        if (aModule) {
          aModule->calculateHits(aMCHit->entry(), aMCHit->exit(), chanAndDist);
        } else continue;
                
        if (!chanAndDist.empty()) {
          // OK got some hits
          for (std::vector<std::pair<OTChannelID,double> >::const_iterator iT = chanAndDist.begin(), iTend = chanAndDist.end(); 
               iT != iTend; ++iT) {
            const double dist = iT->second;
            const int amb = (dist< 0.0) ? -1 : 1;
            m_tempDeposits->push_back(new MCOTDeposit(aMCHit, iT->first, tTimeOffset, std::abs(dist), amb));
          }
        }
      } // iterHit
    } 
  } // loop spills
}

void MCOTDepositCreator::singleCellEff() 
{
  // apply single cell efficiency
  MCOTDepositVec::iterator iterDeposit    = m_tempDeposits->begin();
  MCOTDepositVec::iterator iterDepositEnd = m_tempDeposits->end();
  while (iterDeposit != iterDepositEnd) {
    MCOTDeposit* aDeposit = (*iterDeposit);
    
    // number of tool - want 0,1,2 and not 1,2,3
    int iEffTool = (aDeposit->channel()).station() - m_firstStation;

    bool accept = false;
    m_singleCellEffVector[iEffTool]->calculate(aDeposit, accept);
    if (!accept) {
      /// delete MCOTDeposit*
      delete *iterDeposit;
      /// set to 0
      (*iterDeposit) = static_cast<MCOTDeposit*>(0);
    }
    ++iterDeposit;
  } // loop m_tempDeposits
  /// remove zero deposits
  m_tempDeposits->erase(std::remove(m_tempDeposits->begin(), m_tempDeposits->end(), 
                                    static_cast<MCOTDeposit*>(0)), m_tempDeposits->end());
}

void MCOTDepositCreator::applySmear()
{
  // smear tmp deposits to simulate single cell resolution
  MCOTDepositVec::const_iterator iterDeposit    = m_tempDeposits->begin();
  MCOTDepositVec::const_iterator iterDepositEnd = m_tempDeposits->end();
  while (iterDeposit != iterDepositEnd) {    
    MCOTDeposit* aDeposit = (*iterDeposit);
    
    // number of tool - want 0,1,2 and not 1,2,3
    int iSmearTool = (aDeposit->channel()).station() - m_firstStation;
    // smear     
    if (m_smearTime == true){ 
      m_smearerVector[iSmearTool]->smearTime(aDeposit);
    } else {
      m_smearerVector[iSmearTool]->smearDistance(aDeposit);
      // hack as drift dist can be < 0 due to smearing
      // If < 0 then flip ambiguity
      double driftDist = aDeposit->driftDistance();
      if (driftDist < 0.0) {
        aDeposit->setDriftDistance( std::abs(driftDist) );
        int ambiguity = aDeposit->ambiguity();
        aDeposit->setAmbiguity(-1 * ambiguity);
      } // negative distance
    } 
    ++iterDeposit;
  } // loop m_tempDeposits
}

void MCOTDepositCreator::applyRTrelation()
{
  // apply r-t relation
  MCOTDepositVec::const_iterator iterDeposit    = m_tempDeposits->begin();
  MCOTDepositVec::const_iterator iterDepositEnd = m_tempDeposits->end();
  while (iterDeposit != iterDepositEnd) {
    MCOTDeposit* aDeposit = (*iterDeposit);

    // number of tool - want 0,1,2 and not 1,2,3
    int iRTTool = (aDeposit->channel()).station() - m_firstStation;
    // convert      
    m_rtRelationVector[iRTTool]->convertRtoT(aDeposit);
    
    ++iterDeposit;
  } // loop m_tempDeposits
}

void MCOTDepositCreator::addCrossTalk()
{
  // Add cross talk to deposits
  std::list<MCOTDeposit*> crossTalkList;
  
  MCOTDepositVec::const_iterator iterDeposit    = m_tempDeposits->begin();
  MCOTDepositVec::const_iterator iterDepositEnd = m_tempDeposits->end();
  while (iterDeposit != iterDepositEnd){
    const MCOTDeposit* aDeposit = (*iterDeposit);

    // channel
    OTChannelID aChannel = aDeposit->channel();

    // find right chan
    std::list<OTChannelID> neighbours;
    OTChannelID nextRight = m_tracker->nextChannelRight(aChannel);
    if ( nextRight.channelID() !=0 ) neighbours.push_back(nextRight);
      
    // find left chan
    OTChannelID nextLeft = m_tracker->nextChannelLeft(aChannel);
    if ( nextLeft.channelID() !=0 ) neighbours.push_back(nextLeft);  
    
    std::list<OTChannelID>::const_iterator iterChan    = neighbours.begin();
    std::list<OTChannelID>::const_iterator iterChanEnd = neighbours.end();
    while (iterChan != iterChanEnd) {
      const double testVal = m_flatDist();
      if ( testVal < m_crossTalkLevel) {
        // crosstalk in neighbour - copy hit - this is very ugly
        crossTalkList.push_back(new MCOTDeposit(0, *iterChan, aDeposit->time(), 
                                                aDeposit->driftDistance(), aDeposit->ambiguity()));
      } 
      ++iterChan;
    } //iterChan
    ++iterDeposit;
  } // iterDeposit

  // move hits from crosstalk list to deposit vector
  m_tempDeposits->insert(m_tempDeposits->end(), crossTalkList.begin(), crossTalkList.end());
}

// Add Pulse Reflection
void MCOTDepositCreator::addPulseReflect()
{
  std::list<MCOTDeposit*> DoublePulseList;
  MCOTDepositVec::const_iterator iterDeposit    = m_tempDeposits->begin();
  MCOTDepositVec::const_iterator iterDepositEnd = m_tempDeposits->end();
  while (iterDeposit != iterDepositEnd) {
    const MCOTDeposit* aDeposit = (*iterDeposit);
    
    const double randomNr = m_genDist->shoot();
    if (randomNr <  m_PulseProbability) {
      
      // Time - OTChannelID
      const OTChannelID aChan = aDeposit->channel();
      const double time = aDeposit->time() + m_PulseTime;     

      // New Deposit
      DoublePulseList.push_back(new MCOTDeposit(0, aChan, time, 0, 0));
    }
    ++iterDeposit;
  }
 
  // move hits from double pulse list to deposit vector
  m_tempDeposits->insert(m_tempDeposits->end(), DoublePulseList.begin(), DoublePulseList.end());
}

std::string MCOTDepositCreator::toolName(const std::string& aName, const int id) const
{
  // convert id to station id
  return (aName+boost::lexical_cast<std::string>(id + m_firstStation));
}
