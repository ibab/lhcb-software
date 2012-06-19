//-----------------------------------------------------------------------------
// Implementation file for class : MCFTDepositCreator
//
// 2012-04-04 : COGNERAS Eric
//-----------------------------------------------------------------------------
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from Event
#include "Event/MCHit.h"

// FTDet
#include "FTDet/DeFTDetector.h"

// from FTEvent
#include "Event/MCFTDeposit.h"
#include "FTDet/DeFTLayer.h"

// local
#include "MCFTDepositCreator.h"

using namespace LHCb;


// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCFTDepositCreator );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCFTDepositCreator::MCFTDepositCreator( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("InputLocation" , m_inputLocation = LHCb::MCHitLocation::FT, "Path to input MCHits");
  declareProperty("OutputLocation" , m_outputLocation =  LHCb::MCFTDepositLocation::Default, "Path to output MCDeposits");


  //declareProperty("NeighbourEnergyFraction" , m_neighbourFrac = 0.05, "Fraction of energy deposit lost in neighbour fibres");
  //declareProperty("EnergySharingMode" , m_energySharingMode = "Linear", "Energy sharing between consecutive fibres");
}
//=============================================================================
// Destructor
//=============================================================================
MCFTDepositCreator::~MCFTDepositCreator() {}

//=============================================================================
// Initialization"
//=============================================================================
StatusCode MCFTDepositCreator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "==> Initialize" << endmsg;
    debug() << ": InputLocation is " <<m_inputLocation << endmsg;
    debug() << ": OutputLocation is " <<m_outputLocation << endmsg;
  }
  
  /// Retrieve and initialize DeFT
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
  if (m_deFT) { debug() << "Successfully retrieved DeFT" << endmsg; }
  else { error() << "Error getting DeFT" << endmsg; }


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCFTDepositCreator::execute() {
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "==> Execute" << endmsg;
  }


  // retrieve Hits
  const MCHits* mcHitsCont = get<MCHits>(m_inputLocation);
  debug() <<"mcHitsCont->size() : " << mcHitsCont->size()<< endmsg;

  // define deposits container
  MCFTDeposits *depositCont = new MCFTDeposits();

  // register MCSTDeposits in the transient data store
  put(depositCont, m_outputLocation );

  // loop over MChits
  int debugloopCounter =0; // to be removed once code validated

  MCHits::const_iterator iterHit = mcHitsCont->begin();
  for (; iterHit!=mcHitsCont->end();++iterHit){

    //pointer to the Hit
    MCHit* ftHit = *iterHit;

    // DEBUG printing
    if ( msgLevel( MSG::DEBUG) ) {
      debug() <<"debugloopCounter="<<debugloopCounter<< " XYZ=[" << ftHit->entry() << "][" << ftHit->midPoint()
              << "][" << ftHit->exit ()<< "]\tE="<< ftHit->energy()
              << "\ttime="<< ftHit->time()<< "\tP="<<ftHit->p()
              << endmsg;
      if(ftHit->mcParticle()){
        debug()  << "--- Come from PDGId="<<(ftHit->mcParticle()->particleID())
                 << "\t p="<<(ftHit->mcParticle()->p())
                 << "\t pt="<<(ftHit->mcParticle()->pt())
                 << "\t midPoint.X="<< floor(fabs(ftHit->midPoint().x())/128.)
                 <<" remains="<<((int)fabs(ftHit->midPoint().x()))%128
                 << endmsg; 
      }
    }
    ++debugloopCounter;

    // Get the list of fired FTChannel from the (x,y,z) position of the hit, with, for each FTChannel, 
    // the relative distance to the middle of the cell of the barycentre of the (entry point, endpoint) system : 
    // ( call of calculateHits method) 
    const DeFTLayer* pL = m_deFT->findLayer(ftHit->midPoint());
    FTDoublePairs vectCF;
    if (pL) {
      pL->calculateHits(ftHit->entry(), ftHit->exit(), vectCF);
    }

    if ( msgLevel( MSG::DEBUG) ) {
      debug() << "--- Hit index: " << ftHit->index() << ", size of vector of channels: "
              << vectCF.size() << endmsg;
    }
    

    // Definition of the FTDoublePairs whose double-value corresponds to the energy fraction deposited in the FTChannel
    FTDoublePairs vectCE;

    // test if position fraction to energy fraction conversion succeeded. 
    // If true, create/update the energy deposited in FTChannels
    if(relativeXFractionToEnergyFraction(vectCF,vectCE)){
      FTDoublePairs::const_iterator vecIter;
      for(vecIter = vectCE.begin(); vecIter != vectCE.end(); ++vecIter){
        if ( msgLevel( MSG::DEBUG) ){
          debug()  << "FTChannel = " << vecIter->first << " EnergyHitFraction = " << vecIter->second << endmsg;
        }
        
        // if reference to the channelID already exists, just add (hit energy * energy fraction)
        if( depositCont->object(vecIter->first) != 0 ){
          (depositCont->object(vecIter->first))->addMCHit(ftHit,ftHit->energy()*vecIter->second);
        }
        // else, create a new fired channel but ignore fake cells, i.e. not readout, i.e. layer 15
        else if ( vecIter->first.layer() < 15 ) {
          MCFTDeposit *energyDeposit = new MCFTDeposit(vecIter->first,ftHit,ftHit->energy()*vecIter->second);
          depositCont->insert(energyDeposit,vecIter->first);
        }
      }
    } // end if(relativeXFractionToEnergyFraction(...
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCFTDepositCreator::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// relativeXFractionToEnergyFraction : this function converts ouput from 
// DeFTLayer::calculateHits method [relative distance to the FTChannel midpoint 
// of the barycentre of the (hit entry point, hit end point) system, in each FTChannel] 
// in the energy fraction deposited in each FTChannel
//=============================================================================
StatusCode MCFTDepositCreator::relativeXFractionToEnergyFraction(const FTDoublePairs& positionPair, FTDoublePairs& energyPair) {
  double positionSum = positionPair.size();

  for(FTDoublePairs::const_iterator vecIter = positionPair.begin(); vecIter != positionPair.end(); ++vecIter){
    positionSum -= 2*std::abs(vecIter->second);
  }
  for(FTDoublePairs::const_iterator vecIter = positionPair.begin(); vecIter != positionPair.end(); ++vecIter){
    if ( msgLevel( MSG::DEBUG) ) {
      debug()  << "FTChannel = " << vecIter->first << " RelativeXFraction = " << vecIter->second
               << " CellFraction = " << 1-2*std::abs(vecIter->second)
               << " positionSum=" << positionSum
               << endmsg;
    }
    energyPair.push_back(std::make_pair(vecIter->first,((1-2*std::abs(vecIter->second))/positionSum)));
  }

  // test on the vector size. Is there something more reliable ???
  return (energyPair.size() == positionPair.size());
}
