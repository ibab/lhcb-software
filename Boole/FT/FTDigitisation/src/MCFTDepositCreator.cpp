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
#include "GaudiKernel/PhysicalConstants.h"

// FTDet
#include "FTDet/DeFTDetector.h"

// from FTEvent

#include "FTDet/DeFTLayer.h"
#include "FTDet/DeFTFibreMat.h"

// local
#include "MCFTDepositCreator.h"

// boost
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>


using namespace LHCb;
using namespace Gaudi;
using namespace boost;


// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCFTDepositCreator )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCFTDepositCreator::MCFTDepositCreator( const std::string& name,
                                        ISvcLocator* pSvcLocator)
: GaudiHistoAlg ( name , pSvcLocator )
  , m_spillNames()
  , m_deFT(nullptr)
  , m_yMax(0.0)
  , m_numLayers(12)
{


  std::vector<std::string> tmp1 = { "/PrevPrev/",
                                    "/Prev/",
                                    "/",
                                    "/Next/",
                                    "/NextNext/"};
                                    
  std::vector<double> tmp2 = { -50.0*Gaudi::Units::ns,
                               -25.0*Gaudi::Units::ns,
                               0.0*Gaudi::Units::ns,
                               25.0*Gaudi::Units::ns,
                               50.0*Gaudi::Units::ns };
  
  declareProperty( "SpillVector"                , m_spillVector                 = tmp1);
  declareProperty( "SpillTimes"                 , m_spillTimes                  = tmp2);  
  declareProperty( "InputLocation"              , m_inputLocation               = LHCb::MCHitLocation::FT, "Path to input MCHits");
  declareProperty( "OutputLocation"             , m_outputLocation              = LHCb::MCFTDepositLocation::Default, 
                   "Path to output MCDeposits");
  declareProperty( "FiberPropagationTime"       , m_fiberPropagationTime        = 6.0 * Gaudi::Units::ns / Gaudi::Units::m, 
                   "light propagation time in fiber");
  declareProperty( "ScintillationDecayTime"     , m_scintillationDecayTime      = 2.8 * Gaudi::Units::ns, 
                   "Scintillation decay time of photons");
  declareProperty( "AttenuationToolName"        , m_attenuationToolName         = "MCFTAttenuationTool"   );
  declareProperty( "UseAttenuation"             , m_useAttenuation              = true );
  
  declareProperty( "PutMCParticlePcut"          , m_putMCParticlePcut           = 0. , "apply lower P cut on MCParticles");
  


}
//=============================================================================
// Destructor
//=============================================================================
MCFTDepositCreator::~MCFTDepositCreator() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MCFTDepositCreator::initialize() {

  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "==> Initialize" << endmsg;
    debug() << ": InputLocation is " <<m_inputLocation << endmsg;
    debug() << ": OutputLocation is " <<m_outputLocation << endmsg;
  }
  
  m_attenuationTool = tool<IMCFTAttenuationTool>( m_attenuationToolName, this );
  if( m_attenuationTool == nullptr ){
    error() << "Could not find: " << m_attenuationToolName << endmsg;
  }
  

  // -- Setup the transmissionMap
  if( m_useAttenuation ){
    sc = m_attenuationTool->initializeTool();
    if( sc.isFailure() ) return Error( "Could not initialize attenuation tool", sc );
  }
  

  // Retrieve and initialize DeFT (no test: exception in case of failure)
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );

  m_numLayers = m_deFT->layers().size();
  
  if(m_deFT->version() == 20 ){
    // THIS HAS TO BE CHECKED, WHAT HAPPENS IF FIBRE MAT IS EMPTY???
    m_yMax = m_deFT->fibremats()[0]->layerMaxY();
    if ( msgLevel( MSG::DEBUG) ) debug() << "[initialize] RUN NEW GEOMETRY" << endmsg;
  }else{
    error() << "Unknown FTDet version" << endmsg; 
    return  StatusCode::FAILURE;
	}

  // Initialize random generator
  sc = m_flatDist.initialize( randSvc(), Rndm::Flat(0.0,1.0) );
  if ( sc.isFailure() ) return Error( "Failed to get Rndm::Flat generator", sc );
  
 

  // construct container names once
  for( const std::string spillName : m_spillVector){
    m_spillNames.push_back(  "/Event"+(spillName)+m_inputLocation );
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCFTDepositCreator::execute() {
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "[MCFTDepositCreator] ==> Execute NEW EVENT" << endmsg;
  }

  // define deposits container
  MCFTDeposits *depositCont = new MCFTDeposits();

  // register MCFTDeposits in the transient data store
  put(depositCont, m_outputLocation );

  // Loop over the spills
  // retrieve MCHits and make first list of deposits
  

  int iSpill = 0;
  for( const std::string spillName : m_spillNames ){
    
    MCHits* ftMCHits = getIfExists<MCHits>( spillName );
    
    if (!ftMCHits) {
      if (msgLevel(MSG::DEBUG)) debug() << "Spillover missing in the loop " + spillName << endmsg;
      iSpill++;
      continue;
    }
    
    // found spill - create some digitizations and add them to deposits
    if ( msgLevel( MSG::DEBUG) ) debug() << "ftMCHits->size() : " << ftMCHits->size()<< endmsg;
    
    for( MCHit* ftHit : *ftMCHits ){

      // check if we need to apply a cut on MCParticle properties first
      double MCP = ftHit -> mcParticle() -> p();
      if(m_putMCParticlePcut != 0.) {
        if( MCP < m_putMCParticlePcut ) { 
          plot( MCP, "MCParticle_P_cutout", "MCParticle Momentum (per MCHit) cutout; P [GeV/c]; N MCHits", 0., 200., 200 );
          continue;
        }
      }
      plot( MCP, "MCParticle_P", "MCParticle Momentum (per MCHit); P [GeV/c]; N MCHits", 0., 200., 200 );
      
      counter("NbOfMCHits")++;
      
      // -- Plot some useful quantities
      plot2D( ftHit->entry().x(), ftHit->entry().y(), "HitEntryPosition", "Entry position of hits ; x [mm]; y [mm]",  
              -500., 500., -500.,500., 100, 100 );
      plot(ftHit->energy(),"EnergyOfHit", "Energy of the Hit ; Energy [MeV];Number of hits" , 0 , 10 );
      plot(ftHit->energy(),"EnergyOfHitZOOM", "Energy of the Hit ; Energy [MeV];Number of hits" , 0 , 1 );
      // -----------------------------
      
      // -- DEBUG printing
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << "[ HIT ] XYZ=[" << ftHit->entry() << "][" << ftHit->midPoint()
                << "][" << ftHit->exit ()<< "]\tE="<< ftHit->energy()
                << "\ttime="<< ftHit->time()<< "\tP="<<ftHit->p()
                << "\n...DeltaX="<<std::abs(ftHit->entry().x()-ftHit->exit().x()) 
                << endmsg;
        if(ftHit->mcParticle()){
          debug()  << "... Come from PDGId="<<(ftHit->mcParticle()->particleID())
                   << "\t p="<<(ftHit->mcParticle()->p())
                   << "\t pt="<<(ftHit->mcParticle()->pt())
                   << "\t midPoint.X="<< floor(fabs(ftHit->midPoint().x())/128.)
                   <<" remains="<<((int)fabs(ftHit->midPoint().x()))%128
                   << endmsg; 
        }
      }
      // ----------------
      
      // Get the list of fired FTChannel from the (x,y,z) position of the hit, with, 
      // for each FTChannel, the relative distance to the middle of the cell of the barycentre 
      // of the (entry point, endpoint) system :
      if(m_deFT->version() == 20 ){
        
        StatusCode sc = hitToChannelConversion(ftHit,depositCont,iSpill);
        
        if (  sc.isFailure() ){
          error() << " HitToChannelConversion() FAILED" << endmsg; 
          return sc;
        }
      }else{
        error() << "Unknown FTDet version" << endmsg; 
        return  StatusCode::FAILURE;
      }
      
    } // loop on hits
    
    ++iSpill;
    
  } // loop on spills
  
  return StatusCode::SUCCESS;
}
//=========================================================================
// Convert the hit into channels
//=========================================================================
StatusCode MCFTDepositCreator::hitToChannelConversion(LHCb::MCHit* ftHit, LHCb::MCFTDeposits* depositCont, 
                                                      const unsigned int iSpill) {
  
  const DeFTFibreMat* pL = m_deFT->findFibreMat(ftHit->midPoint());
  FTDoublePairs channels;
  
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "[HitToChannelConversion] RUN NEW GEOMETRY" << endmsg;
  }


  if ( pL == nullptr) {
    if ( msgLevel( MSG::DEBUG) ) debug() << "Could not find FibreMat corresponding to MCHit with mid point " << ftHit->midPoint().X() << " " << 
                                   ftHit->midPoint().Y() << " " << ftHit->midPoint().Z() << endmsg;
    return StatusCode::SUCCESS;
  }
  
  // -- If this fails, it's mostly due to geometrical reasons (in FTDet v2.0)
  // -- so not dangerous
  if( pL->calculateListOfFiredChannels( ftHit, channels) == StatusCode::FAILURE){
    if ( msgLevel( MSG::DEBUG) ) debug() << "Could not calculate list of fired channels" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  counter("NbOfHitInActiveArea")++;     
  counter("NbOfFiredChannels") += channels.size();

  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "--- Hit index: " << ftHit->index() 
            << ", size of vector of channels: " << channels.size() << endmsg;
  }
      
  plotChannelProperties(pL, channels, ftHit);
  
  double att    = 1;
  double attRef = 1;
  
  // -- Calculate the attenuation using the attenuation tool
  if( m_useAttenuation) m_attenuationTool->attenuation(ftHit, att, attRef);
  
  
  // -- Calculate scintillation light release time
  // -- Here could use a fast log, precision should not matter that much
  const double releaseTime = -log( m_flatDist() ) * m_scintillationDecayTime; 
  
  // -- Calculate the arrival time
  const double timeToSiPM    = calcTimeToSiPM(   ftHit, releaseTime, iSpill);
  const double timeRefToSiPM = calcTimeRefToSiPM(ftHit, releaseTime, iSpill);
  
  
  if ( msgLevel( MSG::DEBUG) ){
    debug()  << "[Pulse Arrival Time] Hit(y)=" << fabs(ftHit->midPoint().y())
             << " DirectPulseArrTime="<< timeToSiPM 
             << " ReflectedPulseArrTime="<< timeRefToSiPM << endmsg;
  }
  
  
  // -- Fill MCFTDeposit 
  FTDoublePair negEnergyPair;
  // Temporary Patch [Eric 14/05/2014 : remove hits whose fired channels have negative energy!!]
  bool positiveEnergy = true;
  for( FTDoublePair ftPair : channels){
    if (ftPair.second < 0 ) {
      positiveEnergy = false;
      negEnergyPair = ftPair;
      counter("NegativeEnergyLayer")++;
    }
  }

  // -- End Temporary Patch
  if(positiveEnergy){

    for( FTDoublePair ftPair : channels ){
      const double directEnergyInSiPM    = ftPair.second * att;
      const double reflectedEnergyInSiPM = ftPair.second * attRef;
      
      
      if ( msgLevel( MSG::DEBUG) ){
        debug()  << "[FTCHANNEL] FTChannel=" << ftPair.first 
                 << " DirectEnergyHitFraction="<< directEnergyInSiPM 
                 << " ReflectedEnergyHitFraction="<< reflectedEnergyInSiPM << endmsg;
      }
      
      
      // -- plotting stuff
      plot(ftPair.second, "EnergyDepositedInCell",
           "EnergyDepositedInCell; Energy Deposited [MeV]; Nber of Channels", 0., 2.);    
      plot(ftPair.second, "EnergyDepositedInCellZOOM",
           "EnergyDepositedInCell; Energy Deposited [MeV]; Nber of Channels", 0., 1.);        
      plot(directEnergyInSiPM,"DirectEnergyRecordedInCell",
           "DirectEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,2);
      plot(directEnergyInSiPM,"DirectEnergyRecordedInCellZOOM",
           "DirectEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,1);
      plot(reflectedEnergyInSiPM,"ReflectedEnergyRecordedInCell",
           "ReflectedEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,2);
      plot(reflectedEnergyInSiPM,"ReflectedEnergyRecordedInCellZOOM",
           "ReflectedEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,1);
      // ---------------------------------------------------------------------------------------
      
      
      // -- if reference to the channelID already exists, just add DepositedEnergy and arrival time
      LHCb::MCFTDeposit* deposit = depositCont->object(ftPair.first);
      if( deposit != nullptr ){
        deposit->addMCHit(ftHit, directEnergyInSiPM, reflectedEnergyInSiPM, timeToSiPM, timeRefToSiPM);
        counter("NbOfAppendDeposit")++;
      } else {
        if ( ftPair.first.layer() < m_numLayers ) { // Set to 12 instead 15 because of a bug in new geometry
          // else, create a new fired channel but ignore fake cells, i.e. not readout, i.e. layer 15
          MCFTDeposit* energyDeposit = new MCFTDeposit(ftPair.first,ftHit,directEnergyInSiPM, 
                                                       reflectedEnergyInSiPM,timeToSiPM,timeRefToSiPM);
          depositCont->insert(energyDeposit, ftPair.first);
          counter("NbOfCreatedDeposit")++; 
        }else if ( ftPair.first.layer() >= m_numLayers ){
          counter("NbOfGhostDeposit")++;
          if ( msgLevel( MSG::DEBUG) ) debug() << "hit is not within range of physical layers" << endmsg;
        }
      }
    }
  }else{
    
    plot(negEnergyPair.first.layer(),"CheckNegativeEnergyLayer",
         "CheckNegativeEnergyLayer; Layer of negative energy hit; Nber of Channels" ,0.,20.,20); 
    
  }
  
  return StatusCode::SUCCESS;
  
}
//=========================================================================
// plot stuff
//=========================================================================
void MCFTDepositCreator::plotChannelProperties(const DeFTFibreMat* pL, FTDoublePairs channels, const MCHit* ftHit){
  
  // -- PRINTING -------------------------------------------------------------------------------------------------
  plot(pL->angle()*180/M_PI,"LayerStereoAngle","Layer Stereo Angle;Layer Stereo Angle [#degree];" ,-10 , 10);
  plot(pL->layerInnerHoleRadius(),"LayerHoleRadius","Layer Hole Radius ; Hole Radius  [mm];" ,50 , 150);
  plot(pL->layerMaxY(),"LayerHalfSizeY","Layer Half Size Y ; Layer Half Size Y  [mm];" ,0 , 3500);
  
  plot(channels.size(),"CheckNbChannel", 
       "Number of fired channels per Hit;Number of fired channels;Number of hits", 
       0. , 50., 50);
  
  
  const int NbOfFiredFibres = std::floor(std::abs((ftHit->entry().x()-ftHit->exit().x())*cos(pL->angle()) 
                                                  + (ftHit->entry().y()-ftHit->exit().y())*sin(pL->angle()))/.25+1
                                         );
  
  plot2D(channels.size(),NbOfFiredFibres,"NbChannelvsNbFiredFibres", 
         "Number of fired channels per Hit vs deposit length; Number of fired channels;Deposit Length (in channel units)", 
         0 , 50, 0, 50, 50, 50);
  
  plot2D(channels.size(),NbOfFiredFibres,"NbChannelvsNbFiredFibresZOOM", 
         "Number of fired channels per Hit vs deposit length; Number of fired channels;Deposit Length (in channel units)", 
         0 , 10, 0, 10, 10, 10);
  
  
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "--- Hit index: " << ftHit->index() 
            << ", size of vector of channels: " << channels.size() << endmsg;
    debug() << "[ HIT ] XYZ=[" << ftHit->entry() << "][" << ftHit->midPoint()
            << "][" << ftHit->exit ()<< "]\tE="<< ftHit->energy()
            << "\ttime="<< ftHit->time()<< "\tP="<<ftHit->p()
            << "\n...DeltaX="<<std::abs(ftHit->entry().x()-ftHit->exit().x()) 
            << " pL->angle()="<<pL->angle() << " DX=" << (ftHit->entry().x()-ftHit->exit().x()) 
            << " DY=" << (ftHit->entry().y()-ftHit->exit().y())
            << "==> #Channels="<<NbOfFiredFibres 
            << "+2 (light sharing) + 1 (positioning)"<< endmsg;
  }
  // -----------------------------------------------------------------------------------------------------
  
  double fibrelength = 0;
  double fibrelengthfraction = 0;
  if(pL->hitPositionInFibre(ftHit, fibrelength,fibrelengthfraction)){
        
    
    if ( msgLevel( MSG::DEBUG) ) {
      debug() << format( "fibrelength=%10.3f fibrelengthfraction=%10.3f \n",fibrelength,fibrelengthfraction);
    }
    
    plot(fibrelength,"FibreLength","FibreLength; Sci. Fibre Lengh [mm]; Nber of Events" ,2000 ,3000);
    plot(fibrelengthfraction,"FibreFraction","FibreFraction; Sci. Fibre Fraction ; Nber of Events" ,0 ,1);
  }
  // ----------------------------------------

}
