// $Id: VeloRMeasurement.cpp,v 1.6 2006-02-16 17:04:02 erodrigu Exp $
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
  
  int sensor = m_cluster->sensor();
  double sum   = 0.;
  double sum2  = 0.;
  double sums  = 0.;

  std::vector< std::pair<long,double> > sign = m_cluster->stripSignals();
  std::vector< std::pair<long,double> >::const_iterator strIt;
  VeloChannelID channelOne(sensor,(*sign.begin()).first);
  double radius = det.rOfStrip( channelOne );
  double pitch =   det.rPitch( channelOne );

  for( strIt = sign.begin() ; sign.end() != strIt ; ++strIt ) {
    VeloChannelID channel(sensor,(*strIt).first);
    radius= det.rOfStrip( channel );
    sum   += (*strIt).second;
    sum2  += pow((*strIt).second,2) ;
    sums  += (*strIt).second * radius ;
  }
  if ( 0 < sum ) {
    m_measure = sums / sum;
    // MM+
    // m_measure    = ( pitch / sum ) * sqrt( sum2 / 12 );
    m_errMeasure    = 0.8 * ( pitch / sum ) * sqrt( sum2 / 12 );
    // m_errMeasure = 0.254*pitch - 0.0049*mm;
    // MM-
  }

  m_z = det.zSensor( m_cluster->sensor() );

  // set the LHCbID
  setLhcbID ( LHCbID( channelOne ) );
}
