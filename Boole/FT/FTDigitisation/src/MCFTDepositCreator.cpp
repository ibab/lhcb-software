// $Id$
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
//#include "Event/MCParticle.h"



// local
#include "MCFTDepositCreator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCFTDepositCreator
//
// 2012-04-04 : COGNERAS Eric
//-----------------------------------------------------------------------------

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


  declareProperty("NeighbourEnergyFraction" , m_neighbourFrac = 0.05, "Fraction of energy deposit lost in neighbour fibres");
  declareProperty("EnergySharingMode" , m_energySharingMode =  "Linear", "Energy sharing between consecutive fibres");
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

  debug() << "==> Initialize" << endmsg;
  debug() << ": InputLocation is " <<m_inputLocation << endmsg;
  debug() << ": OutputLocation is " <<m_outputLocation << endmsg;


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCFTDepositCreator::execute() {
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "==> Execute" << endmsg;
  }

  /// Retrieve and initialize DeFT
  DeFTDetector* deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
  if (deFT) { debug() << "Successfully retrieved DeFT" << endmsg; }
  else { error() << "Error getting DeFT" << endmsg; }

  // retrieve Hits
  const MCHits* mcHitsCont = get<MCHits>(m_inputLocation);
  debug() <<"mcHitsCont->size() : " << mcHitsCont->size()<< endmsg;
  // define deposits container
  MCFTDeposits *depositCont = new MCFTDeposits();

  // register MCSTDeposits in the transient data store
  put(depositCont, m_outputLocation );

  // loop over MChits
  int loopCounter =0;

  MCHits::const_iterator iterHit = mcHitsCont->begin();

  for (; iterHit!=mcHitsCont->end();++iterHit){
    MCHit* aHit = *iterHit;

    debug() <<"loopCounter="<<loopCounter<< " XYZ=[" << aHit->entry() << "][" << aHit->midPoint()
            << "][" << aHit->exit ()<< "]\tE="<< aHit->energy()
            << "\ttime="<< aHit->time()<< "\tP="<<aHit->p()
            << endmsg;
    if(aHit->mcParticle()){
      debug()  << "--- Come from PDGId="<<(aHit->mcParticle()->particleID())
               << "\t p="<<(aHit->mcParticle()->p())
               << "\t pt="<<(aHit->mcParticle()->pt())
               << "\t midPoint.X="<< floor(fabs(aHit->midPoint().x())/128.)
               <<" remains="<<((int)fabs(aHit->midPoint().x()))%128
               << endmsg;
    }
    ++loopCounter;

    //call the calculateHits method
    const DeFTLayer* pL = deFT->findLayer(aHit->midPoint());
    std::vector< std::pair<LHCb::FTChannelID, double> > vectCF;
    if (pL) {
      pL->calculateHits(aHit->entry(), aHit->exit(), vectCF);
    }


    debug() << "--- Hit index: " << aHit->index() << ", size of vector of channels: "
            << vectCF.size() << endmsg;

    std::vector< std::pair<LHCb::FTChannelID, double> > vectCE;

    if(RelativeXFractionToEnergyFraction(vectCF,vectCE)){
      std::vector< std::pair<LHCb::FTChannelID, double> >::const_iterator vecIter;
      for(vecIter = vectCE.begin(); vecIter != vectCE.end(); ++vecIter){
        debug()  << "FTChannel = " << vecIter->first << " EnergyHitFraction = " << vecIter->second << endmsg;
        if( depositCont->object(vecIter->first) != 0 ){
          // if reference to the channelID already exists
          (depositCont->object(vecIter->first))->addMCHit(aHit,aHit->energy()*vecIter->second);
        }
        else{
          // create new entry
          MCFTDeposit *aDeposit = new MCFTDeposit(vecIter->first,aHit,aHit->energy()*vecIter->second);
          depositCont->insert(aDeposit,vecIter->first);
        }
      }
    }
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
StatusCode MCFTDepositCreator::RelativeXFractionToEnergyFraction(const FTDoublePair positionPair, FTDoublePair& energyPair) {
  double positionSum = positionPair.size();

  for(FTDoublePair::const_iterator vecIter = positionPair.begin(); vecIter != positionPair.end(); ++vecIter){
    positionSum -= 2*std::abs(vecIter->second);
  }
  for(FTDoublePair::const_iterator vecIter = positionPair.begin(); vecIter != positionPair.end(); ++vecIter){
    //if(msg)
    debug()  << "FTChannel = " << vecIter->first << " RelativeXFraction = " << vecIter->second
             << " CellFraction = " << 1-2*std::abs(vecIter->second)
             << " positionSum=" << positionSum
             << endmsg;

    energyPair.push_back(std::make_pair(vecIter->first,((1-2*std::abs(vecIter->second))/positionSum)));
  }

  return (energyPair.size() == positionPair.size());
}
