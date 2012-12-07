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
  declareProperty( "InputLocation" ,  m_inputLocation  = LHCb::MCHitLocation::FT, "Path to input MCHits");
  declareProperty( "OutputLocation" , m_outputLocation = LHCb::MCFTDepositLocation::Default, "Path to output MCDeposits");
  declareProperty( "ShortAttenuationLength" ,     m_shortAttenuationLength      = 1000 * Gaudi::Units::mm, 
                   "Distance along the fibre to divide the light amplitude by a factor e : short component");
  declareProperty( "LongAttenuationLength" ,      m_longAttenuationLength       = 7000 * Gaudi::Units::mm, 
                   "Distance along the fibre to divide the light amplitude by a factor e : long component");
  declareProperty( "FractionShort",               m_fractionShort = 0.50, "Fraction of short attenuation at SiPM" );
  declareProperty( "XMaxIrradiatedZone",          m_xMaxIrradiatedZone          = 1500. * Gaudi::Units::mm );
  declareProperty( "YMaxIrradiatedZone",          m_yMaxIrradiatedZone          =  500. * Gaudi::Units::mm );
  declareProperty( "IrradiatedAttenuationLength", m_irradiatedAttenuationLength =  600. * Gaudi::Units::mm );
  declareProperty( "ReflexionCoefficient" ,       m_reflexionCoefficient = 0.9, 
                   "Reflexion coefficient of the fibre mirrored side, from 0 to 1");
  declareProperty( "BeginReflexionLossY",         m_beginReflexionLossY         = 1000. * Gaudi::Units::mm );
  declareProperty( "EndReflexionLossY",           m_endReflexionLossY           = 1500. * Gaudi::Units::mm );
  declareProperty( "XStepMap",                    m_xStepMap                    = 200. * Gaudi::Units::mm );
  declareProperty( "YStepMap",                    m_yStepMap                    = 100. * Gaudi::Units::mm );
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
  
  /// Retrieve and initialize DeFT (no test: exception in case of failure)
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );

  //== Generate the transmission map. 
  //== Two attenuation length, then in the radiaton area a different attenuation length
  //== Compute also the transmission of the reflected signal, attenuated by the two length
  //   and the irradiated area

  //== Get the size of the area
  float xMax = m_deFT->layers()[0]->layerMaxX();
  float yMax = m_deFT->layers()[0]->layerMaxY();

  m_nXSteps = xMax / m_xStepMap + 2;   // add x=0 and x > max position
  m_nYSteps = yMax / m_yStepMap + 2;   // same for y
  xMax = (m_nXSteps - 1) * m_xStepMap;
  yMax = (m_nYSteps - 1) * m_yStepMap;
  m_transmissionMap.resize( m_nXSteps * m_nYSteps, 0. );

  for ( int kx = 0; m_nXSteps > kx; ++kx ) {
    float x = kx * m_xStepMap;
    float radZoneSize = m_yMaxIrradiatedZone * ( 1 - x / m_xMaxIrradiatedZone );
    if ( 0. > radZoneSize ) radZoneSize = 0.;
    for ( int ky = 0; m_nYSteps > ky; ++ky ) {
      float y = yMax - ky * m_yStepMap;
      float att = ( m_fractionShort       * exp( -(yMax-y)/m_shortAttenuationLength ) + 
                    ( 1-m_fractionShort ) * exp( -(yMax-y)/m_longAttenuationLength ) );
      //== transmission of the reflected part.
      float attR = ( m_fractionShort       * exp( -(yMax+y)/m_shortAttenuationLength ) + 
                     ( 1-m_fractionShort ) * exp( -(yMax+y)/m_longAttenuationLength ) );
      if ( y > m_endReflexionLossY ) {
        attR = 0.;
      } else if ( y > m_beginReflexionLossY ) {
        attR = attR * ( m_endReflexionLossY - y ) / (m_endReflexionLossY - m_beginReflexionLossY );
      }
      float fractionInRadiation = 1 - x / m_xMaxIrradiatedZone - y / m_yMaxIrradiatedZone;
      if ( 0 < fractionInRadiation ){
        float lInRadiation = (1 - x / m_xMaxIrradiatedZone - fractionInRadiation ) * m_yMaxIrradiatedZone;
        att  = att * exp( - lInRadiation / m_irradiatedAttenuationLength );
        attR = att * exp( ( lInRadiation - radZoneSize ) / m_irradiatedAttenuationLength );
      } else if ( 0. < radZoneSize ) {
        attR = attR * exp( - radZoneSize / m_irradiatedAttenuationLength );
      }
      att = att + m_reflexionCoefficient * attR;
      m_transmissionMap[ m_nYSteps * (kx+1) - ky - 1 ] = att;
    }
    info() << format( "x%7.0f ", x );
    for ( int kk = 0; m_nYSteps > kk ; ++kk ) {
      info() << format( "%6.3f", m_transmissionMap[kx*m_nYSteps+kk] );
    }
    info() << endmsg;  
  }

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
  debug() << "mcHitsCont->size() : " << mcHitsCont->size()<< endmsg;

  // define deposits container
  MCFTDeposits *depositCont = new MCFTDeposits();

  // register MCSTDeposits in the transient data store
  put(depositCont, m_outputLocation );

  for ( MCHits::const_iterator iterHit = mcHitsCont->begin(); iterHit!=mcHitsCont->end();++iterHit){

    MCHit* ftHit = *iterHit;     //pointer to the Hit

    // DEBUG printing
    if ( msgLevel( MSG::DEBUG) ) {
      debug() << " XYZ=[" << ftHit->entry() << "][" << ftHit->midPoint()
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

    // Get the list of fired FTChannel from the (x,y,z) position of the hit, with, 
    // for each FTChannel, the relative distance to the middle of the cell of the barycentre 
    // of the (entry point, endpoint) system : 
    // ( call of calculateHits method from DeFTLayer) 
    const DeFTLayer* pL = m_deFT->findLayer(ftHit->midPoint());
    FTDoublePairs channels;
    double fibreLength = 0;
    double fibreLengthfraction = 0;
   
    if ( pL) {

      if( pL->calculateHits( ftHit, channels)){
        
        pL->hitPositionInFibre(ftHit, fibreLength, fibreLengthfraction );    

        if ( msgLevel( MSG::DEBUG) ) {
          debug() << "--- Hit index: " << ftHit->index() << ", size of vector of channels: "
                  << channels.size() << endmsg;
        }

        //== Compute attenuation by interpolation in the table
        int kx = fabs( ftHit->midPoint().x() ) / m_xStepMap;
        int ky = fabs( ftHit->midPoint().y() ) / m_yStepMap;
        float fracX = fabs( ftHit->midPoint().x() )/m_xStepMap - kx;
        float fracY = fabs( ftHit->midPoint().y() )/m_yStepMap - ky;

        float att = ( fracX     * ( fracY     * m_transmissionMap[m_nYSteps*(kx+1)+ky+1] + 
                                    (1-fracY) * m_transmissionMap[m_nYSteps*(kx+1)+ky]   ) +
                      (1-fracX) * ( fracY     * m_transmissionMap[m_nYSteps*kx+ky+1] + 
                                    (1-fracY) * m_transmissionMap[m_nYSteps*kx+ky]   ) );
        if ( msgLevel( MSG::DEBUG ) ) {
          debug() << format( "x %9.2f y %9.2f kx %3d ky %3d fracx %8.3f fracY %8.3f att %7.4f",
                             ftHit->midPoint().x(), ftHit->midPoint().y(), kx, ky, fracX, fracY, att ) << endmsg;
        }
        
        // Fill MCFTDeposit
        FTDoublePairs::const_iterator vecIter;
        for( vecIter = channels.begin(); vecIter != channels.end(); ++vecIter){
          double EnergyInSiPM = vecIter->second * att;

          if ( msgLevel( MSG::DEBUG) ){
            debug()  << "FTChannel=" << vecIter->first << " EnergyHitFraction="<< EnergyInSiPM << endmsg;
          }
        

          // if reference to the channelID already exists, just add DepositedEnergy
          if( depositCont->object(vecIter->first) != 0 ){
            (depositCont->object(vecIter->first))->addMCHit(ftHit,EnergyInSiPM);
          } else if ( vecIter->first.layer() < 15 ) {
            // else, create a new fired channel but ignore fake cells, i.e. not readout, i.e. layer 15
            MCFTDeposit *energyDeposit = new MCFTDeposit(vecIter->first,ftHit,EnergyInSiPM);
            depositCont->insert(energyDeposit,vecIter->first);
          }
        }
      }// end if(pL->calculateHit)
      
    }// end if(pL)
    
  }

  return StatusCode::SUCCESS;
}
//=========================================================================
