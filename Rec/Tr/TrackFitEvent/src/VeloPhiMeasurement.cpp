// $Id: VeloPhiMeasurement.cpp,v 1.8 2006-02-21 10:58:09 dhcroft Exp $
// Include files 

// local
#include "Event/VeloPhiMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VeloPhiMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 13-07-2000
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
VeloPhiMeasurement::VeloPhiMeasurement( const VeloCluster& cluster,
                                        const DeVelo& det,
                                        double r )
{
  m_mtype = Measurement::VeloPhi;

  m_r = r ;
  m_cluster = &cluster;
  
  int sensor = m_cluster->channelID().sensor();
  m_z = det.zSensor( sensor );

  // Store only the 'position', which is the signed distance from strip to
  // the origin.
  m_measure = det.distToOrigin(  m_cluster->channelID() );
  // fix sign convention of d0 of strip
  if( ! det.isDownstreamSensor(sensor) ) {
    m_measure = -m_measure;
  }

  // get error on that
  double sum   = 0.;
  double sum2  = 0.;
  std::vector < VeloChannelID > channels = m_cluster->channels();
  std::vector< VeloChannelID >::const_iterator iChan;
  for( iChan = channels.begin() ; iChan !=  channels.end() ; ++iChan ) {
    double adc = static_cast<double>(m_cluster->
				     adcValue(iChan-channels.begin()));
    sum  += adc;
    sum2 += adc * adc;
  }

  if ( 0 < sum ) {
    // MM+
    // m_errMeasure  = ( pitch / sum) * sqrt( sum2 / 12 );
    double pitch =   det.phiPitch( m_cluster->channelID() );
    m_errMeasure  = 0.8 * ( pitch / sum) * sqrt( sum2 / 12 );
    // m_errMeasure = 0.254*pitch - 0.0049*mm;
    // MM-
  }


  // set the LHCbID
  setLhcbID ( LHCbID( m_cluster->channelID() ) );
}
