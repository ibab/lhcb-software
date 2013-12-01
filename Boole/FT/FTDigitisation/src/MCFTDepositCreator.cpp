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
  , m_nXSteps(2)
  , m_nYSteps(2)
  , m_deFT(NULL)
{
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
  
  declareProperty( "SpillVector"                , m_spillVector                 = tmp1);
  declareProperty( "SpillTimes"                 , m_spillTimes                  = tmp2);  
  declareProperty( "InputLocation"              , m_inputLocation  = LHCb::MCHitLocation::FT, "Path to input MCHits");
  declareProperty( "OutputLocation"             , m_outputLocation = LHCb::MCFTDepositLocation::Default, "Path to output MCDeposits");
  declareProperty( "FiberRefractionIndex"       , m_fiberRefractionIndex        = 1.59, "Scintillator refraction index");
  declareProperty( "ShortAttenuationLength"     , m_shortAttenuationLength      = 200 * Gaudi::Units::mm, 
                   "Distance along the fibre to divide the light amplitude by a factor e : short component");
  declareProperty( "LongAttenuationLength"      , m_longAttenuationLength       = 4700 * Gaudi::Units::mm, 
                   "Distance along the fibre to divide the light amplitude by a factor e : long component");
  declareProperty( "FractionShort"              , m_fractionShort = 0.18, "Fraction of short attenuation at SiPM" );
  declareProperty( "XMaxIrradiatedZone"         , m_xMaxIrradiatedZone          = 2000. * Gaudi::Units::mm );
  declareProperty( "YMaxIrradiatedZone"         , m_yMaxIrradiatedZone          =  500. * Gaudi::Units::mm );
  declareProperty( "IrradiatedAttenuationLength", m_irradiatedAttenuationLength);
  declareProperty( "ReflectionCoefficient"      , m_reflectionCoefficient       = 0.7, 
                   "Reflection coefficient of the fibre mirrored side, from 0 to 1");
  declareProperty( "BeginReflectionLossY"       , m_beginReflectionLossY        = 1000. * Gaudi::Units::mm );
  declareProperty( "EndReflectionLossY"         , m_endReflectionLossY          = 1500. * Gaudi::Units::mm );
  declareProperty( "XStepOfMap"                 , m_xStepOfMap                  = 200. * Gaudi::Units::mm );
  declareProperty( "YStepOfMap"                 , m_yStepOfMap                  = 100. * Gaudi::Units::mm );
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
  if ( 0 == m_irradiatedAttenuationLength.size() ) {
    m_irradiatedAttenuationLength.push_back( 3000. * Gaudi::Units::mm );
    m_irradiatedAttenuationLength.push_back( 3000. * Gaudi::Units::mm );
    m_irradiatedAttenuationLength.push_back( 3000. * Gaudi::Units::mm );
    m_irradiatedAttenuationLength.push_back( 1500. * Gaudi::Units::mm );
    m_irradiatedAttenuationLength.push_back(  300. * Gaudi::Units::mm );
  }  

  /// Retrieve and initialize DeFT (no test: exception in case of failure)
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );

  // construct container names once
  std::vector<std::string>::const_iterator iSpillName = m_spillVector.begin();
  while (iSpillName!=m_spillVector.end()){
    // path in Transient data store
    std::string mcHitPath = "/Event"+(*iSpillName)+m_inputLocation;//MCHitLocation::FT;
    m_spillNames.push_back(mcHitPath);
    ++iSpillName;
  } // iterSpillName
  
  //== Generate the transmission map. 
  //== Two attenuation length, then in the radiation area a different attenuation length
  //== Compute also the transmission of the reflected signal, attenuated by the two length
  //   and the irradiated area
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "[initialize] Retrieve the dimensions of the layer/fibremat for the transmission map" << endmsg;
  }
  
  //== Get the size of the area
  /* For backward compatibility between OLD and NEW geometry, 
     xMax and yMax variable affectation is made in each case
  */
  float xMax;
  if(m_deFT->version() == 20 ){
    xMax = m_deFT->fibremats()[0]->layerMaxX();
    if ( msgLevel( MSG::DEBUG) ) {
      debug() << "[initialize] RUN NEW GEOMETRY" << endmsg;
    }
  }
  else{
    xMax = m_deFT->layers()[0]->layerMaxX();
    if ( msgLevel( MSG::DEBUG) ) {
      debug() << "[initialize] RUN OLD GEOMETRY" << endmsg;
    }
  }
  
  float yMax;
  if(m_deFT->version() == 20 ) yMax = m_deFT->fibremats()[0]->layerMaxY();
  else  yMax = m_deFT->layers()[0]->layerMaxY();

  // Setup maps size
  info() << format( "layerMaxX()=%7.0f layerMaxY()=%7.0f \n",xMax,yMax );
  m_nXSteps = xMax / m_xStepOfMap + 2;   // add x=0 and x > max position
  m_nYSteps = yMax / m_yStepOfMap + 2;   // same for y
  xMax = (m_nXSteps - 1) * m_xStepOfMap;
  yMax = (m_nYSteps - 1) * m_yStepOfMap;
  m_transmissionMap.resize( m_nXSteps * m_nYSteps, 0. );
  m_transmissionRefMap.resize( m_nXSteps * m_nYSteps, 0. );

  info() << format( "m_xStepOfMap=%7.0f m_yStepOfMap=%7.0f m_nXSteps=%7.0i m_nYSteps=%7.0i xMax=%7.0f yMax=%7.0f \n",
                    m_xStepOfMap,m_yStepOfMap,m_nXSteps, m_nYSteps, xMax, yMax);

  // Loop on the x axis
  for ( int kx = 0; m_nXSteps > kx; ++kx ) {
    float x = kx * m_xStepOfMap;
    float radZoneSize = 2 * m_yMaxIrradiatedZone * ( 1 - x / m_xMaxIrradiatedZone );
    if ( 0. > radZoneSize ) radZoneSize = 0.;
    float yBoundaryRadZone = .5 * radZoneSize;

    // Loop on the y axis and calculate transmission map
    for ( int ky = 0; m_nYSteps > ky; ++ky ) {
      float y = yMax - ky * m_yStepOfMap;

      // Compute attenuation according to the crossed fibre length
      float att = ( m_fractionShort       * exp( -(yMax-y)/m_shortAttenuationLength ) + 
                    ( 1-m_fractionShort ) * exp( -(yMax-y)/m_longAttenuationLength ) );

      plot2D(x,y,"FibreAttenuationMap","Attenuation coefficient as a function of the position (Quarter 3); x [mm];y [mm]",
             0., m_nXSteps*m_xStepOfMap, 0.,m_nYSteps*m_yStepOfMap, m_nXSteps, m_nYSteps, att);
      if ( y < yBoundaryRadZone ){
        att = ( m_fractionShort       * exp( -(yMax-yBoundaryRadZone)/m_shortAttenuationLength ) + 
                ( 1-m_fractionShort ) * exp( -(yMax-yBoundaryRadZone)/m_longAttenuationLength ) );
        float lInRadiation = yBoundaryRadZone - y;
        for ( unsigned int kz = 0; m_irradiatedAttenuationLength.size() > kz; ++kz ) {
          if ( lInRadiation > m_yStepOfMap ) {
            att  *= exp( - m_yStepOfMap / m_irradiatedAttenuationLength[kz] );
          } else if ( lInRadiation > 0. ) {
            att  *= exp( - lInRadiation / m_irradiatedAttenuationLength[kz] );
          } else {
          }
          lInRadiation -= m_yStepOfMap;
        }
        plot2D(x,y,"RadiationAttMap",
               "Attenuation from radiation as a function of the position (Quarter 3); x [mm];y [mm]",0., 
               m_nXSteps*m_xStepOfMap, 0.,m_nYSteps*m_yStepOfMap, m_nXSteps, m_nYSteps, att);
	// !!! add plot attenuation from radiation / normal attenuation. !!! 
      }
      m_transmissionMap[ m_nYSteps * (kx+1) - ky - 1 ] = att;
    }

    // Loop on the y axis and calculate transmission of the reflected pulse
    // The energy from the reflected pulse is calculated as the energy coming from y = 0 and attenuated 
    // by the reflection coefficient, further attenuated depending on the y.
    if ( m_reflectionCoefficient > 0. ) {

      // Reflected energy from y = 0
      float reflected = m_transmissionMap[ m_nYSteps * kx ] * m_reflectionCoefficient;
      for ( int kk = 0; m_nYSteps > kk; ++kk ) {
        float y = kk * m_yStepOfMap;

	// Evaluate the attenuation factor. 
	// Rather than calculating it use information from the transmission map in the same point
        float att = m_transmissionMap[ m_nYSteps*kx + kk] / m_transmissionMap[ m_nYSteps*kx + kk + 1];

	// Fill the map
        m_transmissionRefMap[ m_nYSteps*kx + kk] = reflected;
        plot2D(x,y,"ReflectionContributionMap",
               "Additional signal from reflection (Quarter 3); x [mm];y [mm]",
               0., m_nXSteps*m_xStepOfMap, 0.,m_nYSteps*m_yStepOfMap, m_nXSteps, m_nYSteps, reflected);
        
	// Prepare the reflection ratio for next iteration
        reflected *= att;
      }
    }
    
    if ( msgLevel( MSG::DEBUG) ) {
      debug() << format( "x=%7.0f\n", x);
    }
        
    for ( int kk = 0; m_nYSteps > kk ; ++kk ) {
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << format( "y=%7.0f [%6.3f] [%6.3f]", kk * m_yStepOfMap , m_transmissionMap[kx*m_nYSteps+kk], m_transmissionRefMap[kx*m_nYSteps+kk]);
      }
      // plot attenuation map        
      plot2D(kx * m_xStepOfMap,kk * m_yStepOfMap,
	     "AttenuationMap", "Attenuation coefficient (direct) (Quarter 3); x [mm];y [mm]",
             0., m_nXSteps*m_xStepOfMap, 0.,m_nYSteps*m_yStepOfMap, 
             m_nXSteps, m_nYSteps, m_transmissionMap[kx*m_nYSteps+kk]);
      plot2D(kx * m_xStepOfMap , kk * m_yStepOfMap, 
             "FinalAttenuationMap", "Attenuation coefficient as a function of the position (Quarter 3); x [mm];y [mm]",
             0., m_nXSteps*m_xStepOfMap, 0.,m_nYSteps*m_yStepOfMap, 
	     m_nXSteps, m_nYSteps, m_transmissionMap[kx*m_nYSteps+kk]);
      // plot attenuation from reflection map
      plot2D(kx * m_xStepOfMap,kk * m_yStepOfMap,
	     "ReflectedAttenuationMap", "Attenuation coefficient (reflection) (Quarter 3); x [mm];y [mm]",
             0., m_nXSteps*m_xStepOfMap, 0.,m_nYSteps*m_yStepOfMap, 
             m_nXSteps, m_nYSteps, m_transmissionRefMap[kx*m_nYSteps+kk]);
      plot2D(kx * m_xStepOfMap , kk * m_yStepOfMap, 
             "FinalReflectedAttenuationMap","Attenuation coefficient as a function of the position (Quarter 3); x [mm];y [mm]",
             0., m_nXSteps*m_xStepOfMap, 0.,m_nYSteps*m_yStepOfMap, 
	     m_nXSteps, m_nYSteps, m_transmissionRefMap[kx*m_nYSteps+kk]);
    }       
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

  // register MCSTDeposits in the transient data store
  put(depositCont, m_outputLocation );

  // Loop over the spills
  // retrieve MCHits and make first list of deposits
  for (unsigned int iSpill = 0; iSpill < m_spillNames.size(); ++iSpill){
    SmartDataPtr<MCHits> ftMCHits( eventSvc(), m_spillNames[iSpill] );

    if (!ftMCHits) {
      if (msgLevel(MSG::DEBUG)) 
	debug() << "Spillover missing in the loop " + m_spillNames[iSpill] <<endmsg;
    } else {
      // found spill - create some digitizations and add them to deposits
      if ( msgLevel( MSG::DEBUG) )
	debug() << "ftMCHits->size() : " << ftMCHits->size()<< endmsg;

      // add the loop on the hits
      for ( MCHits::const_iterator iterHit = ftMCHits->begin(); 
	    iterHit!=ftMCHits->end();++iterHit){

	MCHit* ftHit = *iterHit;     //pointer to the Hit
	plot2D( ftHit->entry().x(), ftHit->entry().y(),
		"HitEntryPosition", "Entry position of hits ; x [mm]; y [mm]",  
		-500., 500., -500.,500., 100, 100 );

	plot(ftHit->energy(),"EnergyOfHit", "Energy of the Hit ; Energy [MeV];Number of hits" , 0 , 10 );
	plot(ftHit->energy(),"EnergyOfHitZOOM", "Energy of the Hit ; Energy [MeV];Number of hits" , 0 , 1 );
	// DEBUG printing
	if ( msgLevel( MSG::DEBUG) ) {
	  debug() << "[ HIT ] XYZ=[" << ftHit->entry() << "][" << ftHit->midPoint()
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
	if(m_deFT->version() == 20 ){
	  StatusCode sc = HitToChannelConversion_NewGeometry(ftHit,depositCont,iSpill);
	  if (  sc.isFailure() ){
	    info() << " HitToChannelConversion_NewGeometry() FAILED" << endmsg; 
	    return sc;
	  }
	} // new FT Geometry
	else{
	  StatusCode sc = HitToChannelConversion_OldGeometry(ftHit,depositCont,iSpill);
	  if ( sc.isFailure() ){
	    info() << " HitToChannelConversion_OldGeometry() FAILED" << endmsg; 
	    return sc;
	  }
	} // old FT Geometry
      } // loop on hits
    } // spill found
  } // loop on spills
  return StatusCode::SUCCESS;
}
//=========================================================================
StatusCode MCFTDepositCreator::HitToChannelConversion_OldGeometry(LHCb::MCHit* ftHit,LHCb::MCFTDeposits *depositCont, unsigned int iSpill) {

  const DeFTLayer* pL = m_deFT->findLayer(ftHit->midPoint());
  FTDoublePairs channels;
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "[HitToChannelConversion_OldGeometry] RUN OLD GEOMETRY" << endmsg;
  }
  if ( pL) {
    if( pL->calculateHits( ftHit, channels)){
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << "--- Hit index: " << ftHit->index() 
                << ", size of vector of channels: " << channels.size() << endmsg;
      }

      plot(pL->angle()*180/M_PI,"LayerStereoAngle","Layer Stereo Angle;Layer Stereo Angle [#degree];" ,-10 , 10);
      plot(pL->layerInnerHoleRadius(),"LayerHoleRadius","Layer Hole Radius ; Hole Radius  [mm];" ,50 , 150);
      plot(pL->layerMaxY(),"LayerHalfSizeY","Layer Half Size Y ; Layer Half Size Y  [mm];" ,0 , 3500);
      
      plot(channels.size(),"CheckNbChannel", 
           "Number of fired channels per Hit;Number of fired channels;Number of hits", 
           0 , 50);

      double fibrelength = 0 ;
      double fibrelengthfraction = 0;
      if(pL->hitPositionInFibre(ftHit, fibrelength,fibrelengthfraction)){
        if ( msgLevel( MSG::DEBUG) ) {
          debug() << format( "fibrelength=%10.3f fibrelengthfraction=%10.3f \n",fibrelength,fibrelengthfraction);
        }
      
        plot(fibrelength,"FibreLength","FibreLength; Sci. Fibre Lengh [mm]; Nber of Events" ,2000 ,3000);
        plot(fibrelengthfraction,"FibreFraction","FibreFraction; Sci. Fibre Fraction ; Nber of Events" ,0 ,1);
      }
        

      //== Compute attenuation by interpolation in the table
      int kx = fabs( ftHit->midPoint().x() ) / m_xStepOfMap;
      int ky = fabs( ftHit->midPoint().y() ) / m_yStepOfMap;
      float fracX = fabs( ftHit->midPoint().x() )/m_xStepOfMap - kx;
      float fracY = fabs( ftHit->midPoint().y() )/m_yStepOfMap - ky;
        
      float att = ( fracX     * ( fracY     * m_transmissionMap[m_nYSteps*(kx+1)+ky+1] + 
                                  (1-fracY) * m_transmissionMap[m_nYSteps*(kx+1)+ky]   ) +
                    (1-fracX) * ( fracY     * m_transmissionMap[m_nYSteps*kx+ky+1] + 
                                  (1-fracY) * m_transmissionMap[m_nYSteps*kx+ky]   ) );

      float attRef = ( fracX     * ( fracY     * m_transmissionRefMap[m_nYSteps*(kx+1)+ky+1] + 
				     (1-fracY) * m_transmissionRefMap[m_nYSteps*(kx+1)+ky]   ) +
		       (1-fracX) * ( fracY     * m_transmissionRefMap[m_nYSteps*kx+ky+1] + 
				     (1-fracY) * m_transmissionRefMap[m_nYSteps*kx+ky]   ) );
        
      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << format( "[ATTENUATION] x %9.2f y %9.2f kx %3d ky %3d fracx %8.3f fracY %8.3f att %7.4f",
                           ftHit->midPoint().x(), ftHit->midPoint().y(), kx, ky, fracX, fracY, att ) << endmsg;
      }
        
      plot(att,"AttenuationFactor","AttFactorDistrib; Attenuation factor ; Nber of Events" ,0 ,1);

      // Calculate the arrival time
      double yMax = m_deFT->fibremats()[0]->layerMaxY();
      double timeToSiPM = ftHit->time() + (yMax - fabs(ftHit->midPoint().y())) * m_fiberRefractionIndex / Gaudi::Units::c_light + m_spillTimes[iSpill]; // tilted angles... -> y of module
      double timeRefToSiPM = ftHit->time() + (yMax + fabs(ftHit->midPoint().y())) * m_fiberRefractionIndex / Gaudi::Units::c_light + m_spillTimes[iSpill]; 
      if ( msgLevel( MSG::DEBUG) ){
        debug()  << "[Pulse Arrival Time] Hit(y)=" << fabs(ftHit->midPoint().y())
                 << " DirectPulseArrTime="<< timeToSiPM 
                 << " ReflectedPulseArrTime="<< timeRefToSiPM << endmsg;
      }
      
      // Fill MCFTDeposit
      FTDoublePairs::const_iterator vecIter;
      for( vecIter = channels.begin(); vecIter != channels.end(); ++vecIter){

        double DirectEnergyInSiPM = vecIter->second * att;
        double ReflectedEnergyInSiPM = vecIter->second * attRef;
          
        if ( msgLevel( MSG::DEBUG) ){
          debug()  << "[FTCHANNEL] FTChannel=" << vecIter->first 
		   << " DirectEnergyHitFraction="<< DirectEnergyInSiPM 
		   << " ReflectedEnergyHitFraction="<< ReflectedEnergyInSiPM << endmsg;
        }

        plot(vecIter->second, "EnergyDepositedInCell","EnergyDepositedInCell; Energy Deposited [MeV]; Nber of Channels", 0., 2.);    
        plot(vecIter->second, "EnergyDepositedInCellZOOM","EnergyDepositedInCell; Energy Deposited [MeV]; Nber of Channels", 0., 1.);  
          
        plot(DirectEnergyInSiPM,"DirectEnergyRecordedInCell","DirectEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,2);
        plot(DirectEnergyInSiPM,"DirectEnergyRecordedInCellZOOM",
             "DirectEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,1);
        plot(ReflectedEnergyInSiPM,"ReflectedEnergyRecordedInCell","ReflectedEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,2);
        plot(ReflectedEnergyInSiPM,"ReflectedEnergyRecordedInCellZOOM",
             "ReflectedEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,1);

	//!!!  add zoom!


        // if reference to the channelID already exists, just add DepositedEnergy and arrival time
        if( depositCont->object(vecIter->first) != 0 ){
          (depositCont->object(vecIter->first))->addMCHit(ftHit,DirectEnergyInSiPM,ReflectedEnergyInSiPM,timeToSiPM,timeRefToSiPM);
        } else if ( vecIter->first.layer() < 12 ) { // Set to 12 instaed 15 because of a bug in new geometry
          // else, create a new fired channel but ignore fake cells, i.e. not readout, i.e. layer 15
          MCFTDeposit *energyDeposit = new MCFTDeposit(vecIter->first,ftHit,DirectEnergyInSiPM,ReflectedEnergyInSiPM,timeToSiPM,timeRefToSiPM);
          depositCont->insert(energyDeposit,vecIter->first);
        }
      }
    }else{
      if ( msgLevel( MSG::DEBUG) ){
        debug()  << "Call (if(pL->calculateHit)) returned FALSE" << endmsg;
      }
    }// end if(pL->calculateHit)

  }// end if(pL)

  return StatusCode::SUCCESS;
}

//=========================================================================
StatusCode MCFTDepositCreator::HitToChannelConversion_NewGeometry(LHCb::MCHit* ftHit,LHCb::MCFTDeposits *depositCont, unsigned int iSpill) {
  const DeFTFibreMat* pL = m_deFT->findFibreMat(ftHit->midPoint());
  FTDoublePairs channels;
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "[HitToChannelConversion_NewGeometry] RUN NEW GEOMETRY" << endmsg;
  }
  if ( pL) {
    if( pL->calculateHits( ftHit, channels)){
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << "--- Hit index: " << ftHit->index() 
                << ", size of vector of channels: " << channels.size() << endmsg;
      }

      plot(pL->angle()*180/M_PI,"LayerStereoAngle","Layer Stereo Angle;Layer Stereo Angle [#degree];" ,-10 , 10);
      plot(pL->layerInnerHoleRadius(),"LayerHoleRadius","Layer Hole Radius ; Hole Radius  [mm];" ,50 , 150);
      plot(pL->layerMaxY(),"LayerHalfSizeY","Layer Half Size Y ; Layer Half Size Y  [mm];" ,0 , 3500);
      
      plot(channels.size(),"CheckNbChannel", 
           "Number of fired channels per Hit;Number of fired channels;Number of hits", 
           0 , 50);

      double fibrelength = 0 ;
      double fibrelengthfraction = 0;
      if(pL->hitPositionInFibre(ftHit, fibrelength,fibrelengthfraction)){
        if ( msgLevel( MSG::DEBUG) ) {
          debug() << format( "fibrelength=%10.3f fibrelengthfraction=%10.3f \n",fibrelength,fibrelengthfraction);
        }
        plot(fibrelength,"FibreLength","FibreLength; Sci. Fibre Lengh [mm]; Nber of Events" ,2000 ,3000);
        plot(fibrelengthfraction,"FibreFraction","FibreFraction; Sci. Fibre Fraction ; Nber of Events" ,0 ,1);
      }
        

      //== Compute attenuation by interpolation in the table
      int kx = fabs( ftHit->midPoint().x() ) / m_xStepOfMap;
      int ky = fabs( ftHit->midPoint().y() ) / m_yStepOfMap;
      float fracX = fabs( ftHit->midPoint().x() )/m_xStepOfMap - kx;
      float fracY = fabs( ftHit->midPoint().y() )/m_yStepOfMap - ky;
      
      // A linear interpolation is assumed
      //
      //   X1    X2
      //   D-----C
      //   |     |         X2 = fy C + (1-fy) B
      //   | x---|-fy      X1 = fy D + (1-fy) A
      //   A-|---B
      //     fx            Att = fx X2 + (1-fx) X1
      //
      float att = ( fracX     * ( fracY     * m_transmissionMap[m_nYSteps*(kx+1)+ky+1] + 
                                  (1-fracY) * m_transmissionMap[m_nYSteps*(kx+1)+ky]   ) +
                    (1-fracX) * ( fracY     * m_transmissionMap[m_nYSteps*kx+ky+1] + 
                                  (1-fracY) * m_transmissionMap[m_nYSteps*kx+ky]   ) );

      float attRef = ( fracX     * ( fracY     * m_transmissionRefMap[m_nYSteps*(kx+1)+ky+1] + 
				     (1-fracY) * m_transmissionRefMap[m_nYSteps*(kx+1)+ky]   ) +
		       (1-fracX) * ( fracY     * m_transmissionRefMap[m_nYSteps*kx+ky+1] + 
				     (1-fracY) * m_transmissionRefMap[m_nYSteps*kx+ky]   ) );
        
      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << format( "[ATTENUATION] x %9.2f y %9.2f kx %3d ky %3d fracx %8.3f fracY %8.3f att %7.4f attRef %7.4f",
                           ftHit->midPoint().x(), ftHit->midPoint().y(), kx, ky, fracX, fracY, att, attRef ) << endmsg;
      }
        
      plot(att,"AttenuationFactor","AttFactorDistrib; Attenuation factor ; Nber of Events" ,0 ,1);
      plot(attRef,"AttenuationFactorRef","AttFactorDistribRef; Attenuation factor ref ; Nber of Events" ,0 ,1);

      // Calculate the arrival time
      double yMax = m_deFT->fibremats()[0]->layerMaxY();
      double timeToSiPM = ftHit->time() + (yMax - fabs(ftHit->midPoint().y())) * m_fiberRefractionIndex / Gaudi::Units::c_light + m_spillTimes[iSpill]; // tilted angles... -> y of module
      double timeRefToSiPM = ftHit->time() + (yMax + fabs(ftHit->midPoint().y())) * m_fiberRefractionIndex / Gaudi::Units::c_light + m_spillTimes[iSpill]; 
      if ( msgLevel( MSG::DEBUG) ){
        debug()  << "[Pulse Arrival Time] Hit(y)=" << fabs(ftHit->midPoint().y())
                 << " DirectPulseArrTime="<< timeToSiPM 
                 << " ReflectedPulseArrTime="<< timeRefToSiPM << endmsg;
      }

      // Fill MCFTDeposit
      FTDoublePairs::const_iterator vecIter;
      for( vecIter = channels.begin(); vecIter != channels.end(); ++vecIter){
        double DirectEnergyInSiPM = vecIter->second * att;
        double ReflectedEnergyInSiPM = vecIter->second * attRef;
          
        if ( msgLevel( MSG::DEBUG) ){
          debug()  << "[FTCHANNEL] FTChannel=" << vecIter->first 
                   << " DirectEnergyHitFraction="<< DirectEnergyInSiPM 
                   << " ReflectedEnergyHitFraction="<< ReflectedEnergyInSiPM << endmsg;
        }
         
        plot(vecIter->second, "EnergyDepositedInCell","EnergyDepositedInCell; Energy Deposited [MeV]; Nber of Channels", 0., 2.);    
        plot(vecIter->second, "EnergyDepositedInCellZOOM","EnergyDepositedInCell; Energy Deposited [MeV]; Nber of Channels", 0., 1.);        
        
        plot(DirectEnergyInSiPM,"DirectEnergyRecordedInCell","DirectEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,2);
        plot(DirectEnergyInSiPM,"DirectEnergyRecordedInCellZOOM",
             "DirectEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,1);
        plot(ReflectedEnergyInSiPM,"ReflectedEnergyRecordedInCell","ReflectedEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,2);
        plot(ReflectedEnergyInSiPM,"ReflectedEnergyRecordedInCellZOOM",
             "ReflectedEnergyRecordedInCell; EnergyReachingSiPM [MeV]; Nber of Channels" ,0. ,1);

	//!!!  add zoom!


        // if reference to the channelID already exists, just add DepositedEnergy and arrival time
        if( depositCont->object(vecIter->first) != 0 ){
          (depositCont->object(vecIter->first))->addMCHit(ftHit,DirectEnergyInSiPM,ReflectedEnergyInSiPM,timeToSiPM,timeRefToSiPM);
        } else if ( vecIter->first.layer() < 12 ) { // Set to 12 instaed 15 because of a bug in new geometry
          // else, create a new fired channel but ignore fake cells, i.e. not readout, i.e. layer 15
          MCFTDeposit *energyDeposit = new MCFTDeposit(vecIter->first,ftHit,DirectEnergyInSiPM,ReflectedEnergyInSiPM,timeToSiPM,timeRefToSiPM);
          depositCont->insert(energyDeposit,vecIter->first);
        }
      }
    }else{
      if ( msgLevel( MSG::DEBUG) ){
        debug()  << "Call (if(pL->calculateHit)) returned FALSE" << endmsg;
      }
    }// end if(pL->calculateHit)
      
  }// end if(pL)


  return StatusCode::SUCCESS;
}
