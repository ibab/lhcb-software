/** @file MCFTDepositCreator.cpp
 *
 *  Implementation of class : MCFTDepositCreator
 *
 *  @author COGNERAS Eric
 *  @date   2012-06-05
 */

// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"



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
  declareProperty("FibreAttenuationLengh" , m_attenuationLengh = 3300 , 
                  "Distance along the fibre to divide the light amplitude by a factor e");
}
//=============================================================================
// Destructo
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




    // Get the list of fired FTChannel from the (x,y,z) position of the hit, with, 
    // for each FTChannel, the relative distance to the middle of the cell of the barycentre 
    // of the (entry point, endpoint) system : 
    // ( call of calculateHits method from DeFTLayer) 
    const DeFTLayer* pL = m_deFT->findLayer(ftHit->midPoint());
    FTDoublePairs ChannelEnergy;
    double fibrelengh = 0;
    double fibrelenghfraction = 0;
   
    if (pL) {

      if(pL->calculateHits(ftHit, ChannelEnergy)){
        
        pL->hitPositionInFibre(ftHit, fibrelengh,fibrelenghfraction);
      
    

        if ( msgLevel( MSG::DEBUG) ) {
          debug() << "--- Hit index: " << ftHit->index() << ", size of vector of channels: "
                  << ChannelEnergy.size() << endmsg;
        }

        // Apply attenuation factor on the deposited energy according to the light-path along the fibre
        ApplyAttenuationAlongFibre(fibrelengh,fibrelenghfraction,ChannelEnergy);

        // Fill MCFTDeposit
        FTDoublePairs::const_iterator vecIter;
        for(vecIter = ChannelEnergy.begin(); vecIter != ChannelEnergy.end(); ++vecIter){
          double EnergyInSiPM = vecIter->second;

          if ( msgLevel( MSG::DEBUG) ){
            debug()  << "FTChannel=" << vecIter->first << " EnergyHitFraction="<< EnergyInSiPM << endmsg;
          }
        

          // if reference to the channelID already exists, just add DepositedEnergy
          if( depositCont->object(vecIter->first) != 0 ){
            (depositCont->object(vecIter->first))->addMCHit(ftHit,EnergyInSiPM);
          }
          // else, create a new fired channel but ignore fake cells, i.e. not readout, i.e. layer 15
          else if ( vecIter->first.layer() < 15 ) {
            MCFTDeposit *energyDeposit = new MCFTDeposit(vecIter->first,ftHit,EnergyInSiPM);
            depositCont->insert(energyDeposit,vecIter->first);
          }
        }
      }// end if(pL->calculateHit)
      
    }// end if(pL)
    
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
//:ApplyAttenuationAlongFibre : this function simulates the attenuation of the 
// light through the fibre appling a correction factor on the deposited energy.
// This correction is based on the lengh of the fibre and the relative position
// of the hit wrt the SiPM position.
//
//=============================================================================
StatusCode MCFTDepositCreator::ApplyAttenuationAlongFibre(const double fibrelengh, 
                                                          const double fibrefraction, 
                                                          FTDoublePairs& fibreenergy){
  for(FTDoublePairs::iterator i = fibreenergy.begin(); i != fibreenergy.end(); ++i){
    double AttCoeff = 0.5 * ( 1 + exp( (-2.) * fibrelengh * (1 - fibrefraction) / m_attenuationLengh))
      / exp(  fibrelengh * fibrefraction / m_attenuationLengh);
    i->second *= AttCoeff;
  }
  return StatusCode::SUCCESS;
}


