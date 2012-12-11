// $Id: $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Point3DTypes.h"

// OTDet
#include "OTDet/DeOTDetector.h" 
#include "OTDet/DeOTModule.h"
#include "OTDet/RtRelation.h"
 
// Event
#include "Event/MCHit.h"
#include "Event/MCOTDeposit.h"

// Kernel
#include "Kernel/OTDataFunctor.h"
#include "Kernel/OTChannelID.h"

// OTSimulation
#include "MCOTDepositCreator.h"
#include "IOTEffCalculator.h"
#include "IOTRandomDepositCreator.h"
#include "IOTDoublePulseTool.h"

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
  m_deposits(),
  m_firstStation(0u),
  m_spillNames()
{
  // constructor
  std::vector<std::string> tmp1 = boost::assign::list_of("/PrevPrev/")
                                        ("/Prev/")
                                        ("/")
                                        ("/Next/")
                                        ("/NextNext/");

  std::vector<double> tmp2 = boost::assign::list_of(-50.0*Gaudi::Units::ns)
                                       (-25.0*Gaudi::Units::ns)
                                       (0.0*Gaudi::Units::ns)
                                       (25.0*Gaudi::Units::ns)
                                       (50.0*Gaudi::Units::ns);

  declareProperty("SpillVector"         , m_spillVector            = tmp1                     );
  declareProperty("SpillTimes"          , m_spillTimes             = tmp2                     );
  declareProperty("AddCrosstalk"        , m_addCrossTalk           = true                     );
  declareProperty("CrossTalkLevel"      , m_crossTalkLevel         = 0.005                    );
  declareProperty("AddNoise"            , m_addNoise               = true                     );
  declareProperty("AddDoublePulse"      , m_addDoublePulse         = true                     );
  declareProperty("DoublePulseToolName" , m_doublePulseToolName    = "OTSimpleDoublePulseTool");
}


MCOTDepositCreator::~MCOTDepositCreator()
{
  // destructor
}

StatusCode MCOTDepositCreator::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // sanity checks
  if ( m_spillVector.empty() ) return Error("No spills selected to be digitized !");
    
  // Get OT Geometry
  m_tracker              = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  m_firstStation         = m_tracker->firstStation();  
  unsigned int nStations = m_tracker->nStation();

  // retrieve efficiency calculator tool
  for (unsigned int iEff = 0 ; iEff < nStations; ++iEff) {
    // get tool
    std::string aName = toolName("EffCalculator", iEff);
    IOTEffCalculator* aSingleCellEff = tool<IOTEffCalculator>("OTEffCalculator", aName, this);
    // add tool to vector
    m_singleCellEffVector.push_back( aSingleCellEff );
  } //loop eff calculators
  
  /// Random number from a gauss distribution between 0 and 1
  /// for smearing
  sc = m_gauss.initialize( randSvc() , Rndm::Gauss( 0.0, 1.0) );
  if ( sc.isFailure() ) return Error( "Failed to initialize Gaussian random number generator", sc );
  
  /// Random number from a flat distribution between 0 and 1
  /// for cross talk and double pulse
  sc = m_flat.initialize( randSvc() , Rndm::Flat( 0.0, 1.0) );
  if ( sc.isFailure() ) return Error( "Failed to initialize flat random number generator", sc );

  // construct container names once
  std::vector<std::string>::const_iterator iSpillName = m_spillVector.begin();
  while (iSpillName!=m_spillVector.end()){
    // path in Transient data store
    std::string mcHitPath = "/Event"+(*iSpillName)+MCHitLocation::OT;
    m_spillNames.push_back(mcHitPath);
    ++iSpillName;
  } // iterSpillName
  m_noiseTool = tool<IOTRandomDepositCreator>( "OTRandomDepositCreator" );

  // Retrieve the double pulse generator tool
  m_pulseTool = tool<IOTDoublePulseTool>( m_doublePulseToolName, this );

  return StatusCode::SUCCESS;
}

StatusCode MCOTDepositCreator::execute(){


  // execute once per event
  // reserve some space
  m_deposits.reserve(6000);
 
  // make initial list of deposits
  if (msgLevel(MSG::DEBUG)) debug() << "Making deposits from hits" << endmsg;
  makeDeposits();

  // add crosstalk
  if (m_addCrossTalk) {
    if (msgLevel(MSG::DEBUG)) debug() << "Number of deposits before adding XTalk = "
                                      << m_deposits.size() << endmsg;
    
    addCrossTalk();
    //if (sc.isFailure()) return Error("problems applying crosstalk", sc ); 
    if (msgLevel(MSG::DEBUG))debug() << "Number of deposits after adding XTalk = "
                                   << m_deposits.size() << endmsg;
  }


  // add random noise
  if (m_addNoise) {
    if (msgLevel(MSG::DEBUG)) debug() << "Number of deposits before adding noise = "
                                      << m_deposits.size() << endmsg;
    m_noiseTool->createDeposits(m_deposits);
    if (msgLevel(MSG::DEBUG)) debug() << "Number of deposits after adding noise = "
                                      << m_deposits.size() << endmsg;
  }

  // add Double Pulse Reflection
  if (m_addDoublePulse) {
    if (msgLevel(MSG::DEBUG)) debug() << "Number of deposits before adding double pulse = "
                                      << m_deposits.size() << endmsg;
    addDoublePulse();
    //m_pulseTool
    if (msgLevel(MSG::DEBUG)) debug() << "Number of deposits after adding double pulse = "
                                      << m_deposits.size() << endmsg;
  }
  
  // sort - first by channel
  std::stable_sort( m_deposits.begin(),
                    m_deposits.end()  ,
                    OTDataFunctor::Less_by_ChannelAndTime<const MCOTDeposit*>() );

  /// Fill the MCOTDeposits container and put it in  the TES
  MCOTDeposits* deposits = new MCOTDeposits();
  deposits->reserve(m_deposits.size());
  for_each( m_deposits.begin() , 
            m_deposits.end()   ,
            bind( &MCOTDeposits::add, 
                  deposits,
                  _1 ) );
  if (msgLevel(MSG::DEBUG)) 
    debug() << "Going to put " << deposits->size() << " in the TES" << endmsg;
  put(deposits, MCOTDepositLocation::Default);

  /// Clear deposits vector
  m_deposits.clear();

  return StatusCode::SUCCESS;
}


void MCOTDepositCreator::makeDeposits() const
{
  // retrieve MCHits and make first list of deposits
  for (unsigned int iSpill = 0; iSpill < m_spillNames.size(); ++iSpill){
    /// Retrieve MCHits for this spill
    SmartDataPtr<MCHits> otMCHits( eventSvc(), m_spillNames[iSpill] );

    /// Can't assume that there are hits in spills
    if (!otMCHits) {
      if (msgLevel(MSG::DEBUG)) 
        debug() << "Spillover missing in the loop " + m_spillNames[iSpill] <<endmsg;
    } else {
      // found spill - create some digitizations and add them to deposits
      for (MCHits::const_iterator iHit = otMCHits->begin(), iHitEnd = otMCHits->end(); 
           iHit != iHitEnd; ++iHit) {
        MCHit* aMCHit = (*iHit);
        if (msgLevel(MSG::VERBOSE)) verbose() << "MCHit " << *aMCHit << endmsg;
        
        // time offset
        const double tTimeOffset = aMCHit->time() + m_spillTimes[iSpill];
        
        // make deposits
        std::vector<std::pair<OTChannelID, double> > chanAndDist;
        const DeOTModule* module = m_tracker->findModule(aMCHit->midPoint());
        if (msgLevel(MSG::VERBOSE)) verbose() << " module " << module << endmsg;
        if ( module ) {
          if (msgLevel(MSG::VERBOSE)) 
            verbose() << "MCHit entry: " << aMCHit->entry()
                      << " MCHit exit: " << aMCHit->exit() << endmsg;
          module->calculateHits(aMCHit->entry(), aMCHit->exit(), chanAndDist);
        } else continue;
                
        if ( !chanAndDist.empty() ) {
          // OK got some hits
          for (std::vector< std::pair<OTChannelID,double> >::const_iterator 
                 iT = chanAndDist.begin(), iTend = chanAndDist.end(); iT != iTend; ++iT) {
            const unsigned int toolIndex = (iT->first).station() - m_firstStation;
            const double dist            = iT->second;
            const int amb                = (dist < 0.0) ? -1 : 1;
            /// create deposit
            MCOTDeposit* deposit = new MCOTDeposit(MCOTDeposit::Signal, aMCHit, iT->first, 
                                                   tTimeOffset, std::abs(dist), amb);
            if (msgLevel(MSG::VERBOSE)) verbose() << *deposit << endmsg;
            /// Apply single cell efficiency cut
            /// Currently per station; but I can imagine we want to do it per module or straw
            bool accept = false;
            
            // Calculate the path through a cell in 3D, using the slope
            Gaudi::XYZVector slopes = 
              module->geometry()->toLocal(Gaudi::XYZVector(aMCHit->dxdz(),
                                                           aMCHit->dydz(), 1.0));

            if (msgLevel(MSG::VERBOSE)) verbose() << "Slopes " << aMCHit->dxdz()
                                                  << " " << aMCHit->dydz() 
                                                  << " " << slopes << endmsg;

            m_singleCellEffVector[toolIndex]->calculate( deposit, (slopes.y()/slopes.z()), accept );

            if ( accept ) {
              /// Smear deposit in time
              const double sigmaT   = 
                (module->rtRelation()).drifttimeError(deposit->driftDistance());
              const double smearedT = m_gauss()*sigmaT;
              deposit->addTime(smearedT);
              /// R-T relation per module
              const double tFromR   = (module->rtRelation()).drifttime(deposit->driftDistance());
              /// propagation time per module is the distance traveled along the wire divided
              /// by the propagation velocity
              const double propTime = 
                module->distanceAlongWire( ( aMCHit->midPoint() ).x(),
                                    ( aMCHit->midPoint() ).y() ) / module->propagationVelocity();
              
              deposit->addTime( tFromR + propTime );
              m_deposits.push_back(deposit);
            } else delete deposit;
          }
        }
      } // iterHit
    } 
  } // loop spills
}

void MCOTDepositCreator::addCrossTalk() const
{
  // Add cross talk to deposits
  std::list<MCOTDeposit*> crossTalkList;
  
  OTDeposits::const_iterator iterDeposit    = m_deposits.begin();
  OTDeposits::const_iterator iterDepositEnd = m_deposits.end();
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
      const double testVal = m_flat();
      if ( testVal < m_crossTalkLevel ) {
        // crosstalk in neighbour - copy hit - this is very ugly
        crossTalkList.push_back(new MCOTDeposit(MCOTDeposit::XTalk, 0 , *iterChan, aDeposit->time(), 
                                                aDeposit->driftDistance(), aDeposit->ambiguity()));
      } 
      ++iterChan;
    } //iterChan
    ++iterDeposit;
  } // iterDeposit

  // move hits from crosstalk list to deposit vector
  m_deposits.insert(m_deposits.end(), crossTalkList.begin(), crossTalkList.end());
}


// Add Double Pulse
void MCOTDepositCreator::addDoublePulse() const
{
  std::list<MCOTDeposit*> doublePulses;
  for ( OTDeposits::const_iterator dep = m_deposits.begin(), 
          depEnd = m_deposits.end(); dep != depEnd; ++dep ) {
    const MCOTDeposit* aDeposit = (*dep);
    /// We only want to do this for particles
    if ( aDeposit->mcHit() == 0  ) continue;  

    double timeOffset = m_pulseTool->timeOffset( aDeposit );
    if( timeOffset < 0.0 ) continue;
    double time = aDeposit->time() + timeOffset;
    // Time - OTChannelID
    const OTChannelID aChan = aDeposit->channel();
    doublePulses.push_back(new MCOTDeposit(MCOTDeposit::DoublePulse, 0, aChan, time, 0, 0));
  }
  // move hits from double pulse list to deposit vector
  m_deposits.insert( m_deposits.end(), doublePulses.begin(), doublePulses.end() );
}

std::string MCOTDepositCreator::toolName(const std::string& aName, const int id) const
{
  // convert id to station id
  return (aName+boost::lexical_cast<std::string>(id + m_firstStation));
}
