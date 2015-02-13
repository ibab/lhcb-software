// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
// local
#include "MCFTAttenuationTool.h"

// from Event
#include "Event/MCHit.h"


//-----------------------------------------------------------------------------
// Implementation file for class : MCFTAttenuationTool
//
// 2015-01-15 : Michel De Cian
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCFTAttenuationTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCFTAttenuationTool::MCFTAttenuationTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
{
  declareInterface<IMCFTAttenuationTool>(this);

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
// Destructor
//=============================================================================
MCFTAttenuationTool::~MCFTAttenuationTool() {

}
//=============================================================================
// Calculate the attenuation
//=============================================================================
void MCFTAttenuationTool::attenuation(const LHCb::MCHit* ftHit, double& att, double& attRef){
  
  //== Compute attenuation by interpolation in the table
  //== No difference made here between x-layers and (u,v)-layers : all layers assumed to be x-layers
  const double hitXPosition = ftHit->midPoint().x();
  const double hitYPosition = ftHit->midPoint().y();
  const int kx = std::min(m_nXSteps - 2,(int)(fabs( hitXPosition ) / m_xStepOfMap));
  const int ky = std::min(m_nYSteps - 2,(int)(fabs( hitYPosition ) / m_yStepOfMap));
  const double fracX = fabs( hitXPosition )/m_xStepOfMap - kx;
  const double fracY = fabs( hitYPosition )/m_yStepOfMap - ky;
  
  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << format( "[ATTENUATION]  kx %3d ky %3d fracx %8.3f fracY %8.3f ",
                       kx, ky, fracX, fracY ) << endmsg;
  }
  
  att    = calcAtt(    fracX, fracY, kx, ky );
  attRef = calcAttRef( fracX, fracY, kx, ky );
  
  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << format( "[ATTENUATION] x %9.2f y %9.2f kx %3d ky %3d fracx %8.3f fracY %8.3f att %7.4f attRef %7.4f",
                       ftHit->midPoint().x(), ftHit->midPoint().y(), kx, ky, fracX, fracY, att, attRef ) << endmsg;
  }
  
  plot(att,"AttenuationFactor","AttFactorDistrib; Attenuation factor ; Nber of Events" ,0 ,1);
  plot(attRef,"AttenuationFactorRef","AttFactorDistribRef; Attenuation factor ref ; Nber of Events" ,0 ,1);


}
//=========================================================================
// Calculate the transmission map
//=========================================================================
StatusCode MCFTAttenuationTool::initializeTool(){
  
  
  if (m_irradiatedAttenuationLength.empty() ) {
    m_irradiatedAttenuationLength = { 3000. * Gaudi::Units::mm, 
                                      3000. * Gaudi::Units::mm, 
                                      3000. * Gaudi::Units::mm, 
                                      1500. * Gaudi::Units::mm, 
                                      300. * Gaudi::Units::mm };
  }  

  /// Retrieve and initialize DeFT (no test: exception in case of failure)
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
  
  //== Generate the transmission map. 
  //== Two attenuation length, then in the radiation area a different attenuation length
  //== Compute also the transmission of the reflected signal, attenuated by the two length
  //   and the irradiated area
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "[initialize] Retrieve the dimensions of the layer/fibremat for the transmission map" << endmsg;
  }
  
  if(m_deFT->version() == 20 ){
    // THIS HAS TO BE CHECKED, WHAT HAPPENS IF FIBRE MAT IS EMPTY???
    m_xMax = m_deFT->fibremats()[0]->layerMaxX();
    m_yMax = m_deFT->fibremats()[0]->layerMaxY();
    if ( msgLevel( MSG::DEBUG) ) debug() << "[initialize] RUN NEW GEOMETRY" << endmsg;
  }else{
    error() << "Unknown FTDet version" << endmsg; 
    return  StatusCode::FAILURE;
	}
  
  // Setup maps size
  if ( msgLevel( MSG::DEBUG) ) debug() << format( "layerMaxX()=%7.0f layerMaxY()=%7.0f \n",m_xMax,m_yMax );

  m_nXSteps = m_xMax / m_xStepOfMap + 2;   // add x=0 and x > max position
  m_nYSteps = m_yMax / m_yStepOfMap + 2;   // same for y
  const double xMax = (m_nXSteps - 1) * m_xStepOfMap;
  const double yMax = (m_nYSteps - 1) * m_yStepOfMap;
  
  // m_transmissionMap set at 1E-10 initialisatino value to avoid 'division by zero' bug
  m_transmissionMap.resize( m_nXSteps * m_nYSteps, 1E-10 );
  m_transmissionRefMap.resize( m_nXSteps * m_nYSteps, 1E-10);
  
  if ( msgLevel( MSG::DEBUG) ) debug() << format( "m_xStepOfMap=%7.0f m_yStepOfMap=%7.0f m_nXSteps=%7.0i m_nYSteps=%7.0i xMax=%7.0f yMax=%7.0f \n",
                                                  m_xStepOfMap,m_yStepOfMap,m_nXSteps, m_nYSteps, xMax, yMax);
  
  // Loop on the x axis
  for ( int kx = 0; m_nXSteps > kx; ++kx ) {
    const double x = kx * m_xStepOfMap;
    
    // -- is this a model, a fact, an urban legend?
    double radZoneSize = 2 * m_yMaxIrradiatedZone * ( 1 - x / m_xMaxIrradiatedZone );
    if ( 0. > radZoneSize ) radZoneSize = 0.;
    const double yBoundaryRadZone = .5 * radZoneSize;
    

    // Loop on the y axis and calculate transmission map
    for ( int ky = 0; m_nYSteps > ky; ++ky ) {
      const double y = yMax - ky * m_yStepOfMap;

      // Compute attenuation according to the crossed fibre length
      double att = ( m_fractionShort       * exp( -(yMax-y)/m_shortAttenuationLength ) + 
                          ( 1 - m_fractionShort ) * exp( -(yMax-y)/m_longAttenuationLength ) );

      plot2D(x,y,"FibreAttenuationMap","Attenuation coefficient as a function of the position (Quarter 3); x [mm];y [mm]",
             0., m_nXSteps*m_xStepOfMap, 0.,m_nYSteps*m_yStepOfMap, m_nXSteps, m_nYSteps, att);
  
      if ( y < yBoundaryRadZone ){
        att = ( m_fractionShort       * exp( -(yMax-yBoundaryRadZone)/m_shortAttenuationLength ) + 
                ( 1-m_fractionShort ) * exp( -(yMax-yBoundaryRadZone)/m_longAttenuationLength ) );
        
        double lInRadiation = yBoundaryRadZone - y;
        
        for ( unsigned int kz = 0; m_irradiatedAttenuationLength.size() > kz; ++kz ) {
          if ( lInRadiation > m_yStepOfMap ) {
            att  *= exp( - m_yStepOfMap / m_irradiatedAttenuationLength[kz] );
          } else if ( lInRadiation > 0. ) {
            att  *= exp( - lInRadiation / m_irradiatedAttenuationLength[kz] );
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
      double reflected = m_transmissionMap[ m_nYSteps * kx ] * m_reflectionCoefficient;
      
      for ( int kk = 0; m_nYSteps > kk; ++kk ) {
        const double y = kk * m_yStepOfMap;

        // Evaluate the attenuation factor. 
        // Rather than calculating it use information from the transmission map in the same point
        double att = 0;
     
        if (m_nYSteps > (kk + 1)) att = m_transmissionMap[ m_nYSteps*kx + kk] / m_transmissionMap[ m_nYSteps*kx + kk + 1];

        // Fill the map
        m_transmissionRefMap[ m_nYSteps*kx + kk] = reflected;
        plot2D(x,y,"ReflectionContributionMap",
               "Additional signal from reflection (Quarter 3); x [mm];y [mm]",
               0., m_nXSteps*m_xStepOfMap, 0.,m_nYSteps*m_yStepOfMap, m_nXSteps, m_nYSteps, reflected);
        
        // Prepare the reflection ratio for next iteration
        reflected *= att;
      }
    }
    

    // -- This is only plotting and debug stuff.....
    if ( msgLevel( MSG::DEBUG) ) {
      debug() << format( "x=%7.0f\n", x)<< endmsg;
    }
        
    for ( int kk = 0; m_nYSteps > kk ; ++kk ) {
     
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << format( "y=%7.0f [%6.3f] [%6.3f]\n", kk * m_yStepOfMap , 
                           m_transmissionMap[kx*m_nYSteps+kk], m_transmissionRefMap[kx*m_nYSteps+kk])<< endmsg;
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

