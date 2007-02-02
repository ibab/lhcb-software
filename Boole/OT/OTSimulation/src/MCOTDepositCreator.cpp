// $Id: MCOTDepositCreator.cpp,v 1.20 2007-02-02 09:28:44 janos Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"

// OTDet
#include "OTDet/DeOTDetector.h" 
#include "OTDet/DeOTModule.h"
 
// Event
#include "Event/MCHit.h"
#include "Event/MCOTDeposit.h"

// Kernel
#include "Kernel/OTDataFunctor.h"
#include "Kernel/Point3DTypes.h"
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
  m_spillVector.push_back("/PrevPrev/");
  m_spillVector.push_back("/Prev/");
  m_spillVector.push_back("/");
  m_spillVector.push_back("/Next/");
  m_spillVector.push_back("/NextNext/");

  m_spillTimes.push_back(-50.0*Gaudi::Units::ns);
  m_spillTimes.push_back(-25.0*Gaudi::Units::ns);
  m_spillTimes.push_back(0.0*Gaudi::Units::ns);
  m_spillTimes.push_back(25.0*Gaudi::Units::ns);
  m_spillTimes.push_back(50.0*Gaudi::Units::ns);

  declareProperty("spillVector", m_spillVector);
  declareProperty("spillTimes", m_spillTimes);

  declareProperty("addCrosstalk", m_addCrossTalk = true);
  declareProperty("crossTalkLevel", m_crossTalkLevel = 0.025);
  declareProperty("addNoise", m_addNoise = true);

  declareProperty("addPulse", m_addPulse = true);
  declareProperty("PulseTime", m_PulseTime = 30*Gaudi::Units::ns);
  declareProperty("PulseProbability", m_PulseProbability = 0.3);

  declareProperty("noiseToolName",m_noiseToolName = "OTRandomDepositCreator" );

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
  StatusCode sc;

  // make initial list of deposits
  debug() << "make digitizations from MCHits" << endmsg;
  makeDigitizations();
  //if (sc.isFailure()) return Error("failed to make digitizations", sc);
  
  // single cell eff
  debug() << "deposits size before applying efficiency = "
          << m_tempDeposits->size() << endmsg;
  sc = singleCellEff();
  if (sc.isFailure()) return Error("problems applying single cell eff", sc);
  debug() << "deposits size after applying efficiency = " 
          << m_tempDeposits->size() << endmsg;

  // smear
  debug() << "apply single cell smearing" << endmsg;
  sc = applySmear();
  if (sc.isFailure()) return Error("problems applying single cell res smear", sc);

  // r-to-t
  debug() <<"convert r-to t" << endmsg;
  sc = applyRTrelation();
  if (sc.isFailure()) return Error("problems applying single cell rt relation", sc );  

  // add crosstalk
  if (m_addCrossTalk) {
    debug() << "deposits size before adding XTalk = "
            << m_tempDeposits->size() << endmsg;
    addCrossTalk();
    //if (sc.isFailure()) return Error("problems applying crosstalk", sc ); 
    debug() << "deposits size after XTalk = " 
            << m_tempDeposits->size() << endmsg;
  }
  
  // add random noise
  if (m_addNoise) {
    debug() << "deposits size before adding noise = "
            << m_tempDeposits->size() << endmsg;
    m_noiseTool->createDeposits(m_tempDeposits);
    //if (sc.isFailure()) return Error("problems applying noise", sc);
    debug() << "deposits size after adding noise = " 
            << m_tempDeposits->size() << endmsg;
  }

  // add Double Pulse Reflection
  if (m_addPulse) {
    debug() << "deposits size before adding Double Pulse Reflection = "
            << m_tempDeposits->size() << endmsg;
    addPulseReflect();
    //if (sc.isFailure()) return Error("problems applying  pulse reflect", sc);
    debug() << "deposits size after adding  Double Pulse Reflection = "
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
      debug() << "Spillover missing in the loop " +m_spillNames[iSpill] <<endmsg;
    } else {
      // found spill - create some digitizations and add them to deposits
      MCHits::const_iterator iHit = otMCHits->begin();
      for ( ; iHit != otMCHits->end(); ++iHit) {
        MCHit* aMCHit = (*iHit);
        // time offset
        double tTimeOffset = aMCHit->time() + m_spillTimes[iSpill];

        // make deposits
        std::vector<std::pair<OTChannelID, double> > chanAndDist;
        DeOTModule* aModule = m_tracker->findModule(aMCHit->midPoint());
        if (aModule) {
          aModule->calculateHits(aMCHit->entry(), aMCHit->exit(), chanAndDist);
        } else continue;
                
        if (chanAndDist.empty()) {
          continue;
        } else {
          // OK got some hits
          std::vector<std::pair<OTChannelID,double> >::const_iterator iT = chanAndDist.begin();
          for ( ; iT != chanAndDist.end(); ++iT) {
            const double dist = iT->second;
            const int amb = (dist< 0.0)?-1:1;
            m_tempDeposits->push_back(new MCOTDeposit(aMCHit, iT->first, tTimeOffset, std::abs(dist), amb));
          }
        }
      } // iterHit
    } 
  } // loop spills
}

StatusCode MCOTDepositCreator::singleCellEff() 
{
  // apply single cell efficiency
  StatusCode sc = StatusCode::SUCCESS;

  MCOTDepositVec::iterator iterDeposit = m_tempDeposits->begin();
  while (iterDeposit != m_tempDeposits->end()) {
    MCOTDeposit* aDeposit = (*iterDeposit);
    
    // number of tool - want 0,1,2 and not 1,2,3
    int iEffTool = (aDeposit->channel()).station() - m_firstStation;

    bool iAccept = false;
    sc = m_singleCellEffVector[iEffTool]->calculate(aDeposit, iAccept);
    if (sc.isFailure()) return sc;
    if (!iAccept) {
      /// delete MCOTDeposit*
      delete *iterDeposit;
      /// set to 0
      (*iterDeposit) = (MCOTDeposit*)0;
    }
    ++iterDeposit;
  } // loop m_tempDeposits
  /// remove zero deposits
  m_tempDeposits->erase(std::remove(m_tempDeposits->begin(), m_tempDeposits->end(), 
                                    (MCOTDeposit*)0), m_tempDeposits->end());
  return sc;
}

StatusCode MCOTDepositCreator::applySmear()
{
  // smear m_tempDeposits to simulate single cell resolution
  StatusCode sc = StatusCode::SUCCESS;

  MCOTDepositVec::iterator iterDeposit = m_tempDeposits->begin();
  while (iterDeposit != m_tempDeposits->end()) {    
    MCOTDeposit* aDeposit = (*iterDeposit);
    
    // number of tool - want 0,1,2 and not 1,2,3
    int iSmearTool = (aDeposit->channel()).station() - m_firstStation;

    // smear      
    sc = m_smearerVector[iSmearTool]->smear(aDeposit);
    if (sc.isFailure()) return sc;

    // hack as drift dist can < 0 due to smearing
    double driftDist = aDeposit->driftDistance();
    if (driftDist < 0.0) {
      aDeposit->setDriftDistance( std::abs(driftDist) );
      int ambiguity = aDeposit->ambiguity();
      aDeposit->setAmbiguity(-1 * ambiguity);
    }

    ++iterDeposit;
  } // loop m_tempDeposits  

  return sc;
}

StatusCode MCOTDepositCreator::applyRTrelation()
{
  // apply r-t relation
  StatusCode sc = StatusCode::SUCCESS;

  MCOTDepositVec::iterator iterDeposit = m_tempDeposits->begin();
  while (iterDeposit != m_tempDeposits->end()) {
    MCOTDeposit* aDeposit = (*iterDeposit);

    // number of tool - want 0,1,2 and not 1,2,3
    int iRTTool = (aDeposit->channel()).station() - m_firstStation;

    // convert      
    sc = m_rtRelationVector[iRTTool]->convertRtoT(aDeposit);
    if (sc.isFailure()) return sc;

    ++iterDeposit;
  } // loop m_tempDeposits  

  return sc;
}

void MCOTDepositCreator::addCrossTalk()
{
  // Add cross talk to deposits
  std::list<MCOTDeposit*> crossTalkList;
  
  MCOTDepositVec::const_iterator iterDeposit = m_tempDeposits->begin();
  while (iterDeposit != m_tempDeposits->end()){
    MCOTDeposit* aDeposit = (*iterDeposit);

    // channel
    OTChannelID aChannel = aDeposit->channel();

    // find right chan
    std::list<OTChannelID> neighbours;
    OTChannelID nextRight = m_tracker->nextChannelRight(aChannel);
    if ( nextRight.channelID() !=0 ) neighbours.push_back(nextRight);
      
    // find left chan
    OTChannelID nextLeft = m_tracker->nextChannelLeft(aChannel);
    if ( nextLeft.channelID() !=0 ) neighbours.push_back(nextLeft);  
    
    std::list<OTChannelID>::iterator iterChan = neighbours.begin();
    while (iterChan != neighbours.end()) {
      double testVal = m_flatDist();
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
  m_tempDeposits->insert(m_tempDeposits->end(), crossTalkList.begin(), 
                         crossTalkList.end());
}

// Add random noise deposits. 
// void MCOTDepositCreator::addNoise()
// {
//   m_noiseTool->createDeposits(m_tempDeposits);
// }

// Add Pulse Reflection
void MCOTDepositCreator::addPulseReflect()
{
  std::list<MCOTDeposit*> DoublePulseList;
  MCOTDepositVec::const_iterator iterDeposit = m_tempDeposits->begin();
  while (iterDeposit != m_tempDeposits->end()) {
    MCOTDeposit* aDeposit = (*iterDeposit);

    double randomNr = m_genDist->shoot();
    if (randomNr <  m_PulseProbability) {

      // Time - OTChannelID
      OTChannelID aChan = aDeposit->channel();
      double time = aDeposit->time() + m_PulseTime;     

      // New Deposit
      DoublePulseList.push_back(new MCOTDeposit(0, aChan, time, 0, 0));
    }
    ++iterDeposit;
  }
 
  // move hits from double pulse list to deposit vector
  m_tempDeposits->insert(m_tempDeposits->end(), DoublePulseList.begin(), 
                         DoublePulseList.end());
}

std::string MCOTDepositCreator::toolName(const std::string& aName, const int id) const
{
  // convert id to station id
  return (aName+boost::lexical_cast<std::string>(id + m_firstStation));
}
