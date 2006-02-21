// $Id: VeloRMeasurement.cpp,v 1.7 2006-02-21 10:58:09 dhcroft Exp $
// Include files

// local
#include "Event/VeloRMeasurement.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VeloRMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 27-06-2000
//-----------------------------------------------------------------------------

/// Standard constructor, initializes variables
VeloRMeasurement::VeloRMeasurement( const VeloCluster& cluster,
                                    const DeVelo& det,
                                    double phi )
{
  m_mtype = Measurement::VeloR;
  
  m_phi = phi;
  m_cluster = &cluster;
  
  int sensor = m_cluster->channelID().sensor();
  m_z = det.zSensor( sensor );

  double sum   = 0.;
  double sum2  = 0.;
  double sums  = 0.;

  std::vector < VeloChannelID > channels = m_cluster->channels();
  std::vector< VeloChannelID >::const_iterator iChan;
  for( iChan = channels.begin() ; iChan !=  channels.end() ; ++iChan ) {
    double radius= det.rOfStrip( *iChan );
    double adc = static_cast<double>(m_cluster->
				     adcValue(iChan-channels.begin()));
    sum   += adc;
    sum2  += adc * adc;
    sums  += adc * radius ;
  }
  if ( 0 < sum ) {
    m_measure = sums / sum;
    // MM+
    // m_measure    = ( pitch / sum ) * sqrt( sum2 / 12 );
  // use center strip as representative
    double pitch =   det.rPitch( m_cluster->channelID() );
    m_errMeasure    = 0.8 * ( pitch / sum ) * sqrt( sum2 / 12 );
    // m_errMeasure = 0.254*pitch - 0.0049*mm;
    // MM-
  }

  // set the LHCbID
  setLhcbID ( LHCbID( m_cluster->channelID() ) );
}
