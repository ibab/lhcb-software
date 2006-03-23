// $Id: VeloRMeasurement.cpp,v 1.9 2006-03-23 12:38:58 mtobin Exp $
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
                                    const Gaudi::TrackVector& refVector )
{
  m_refVector = refVector; // reference trajectory
  this->init( cluster, det, true );
}

/// Standard constructor, initializes variables
VeloRMeasurement::VeloRMeasurement( const VeloCluster& cluster,
                                    const DeVelo& det ) 
{
  m_refVector = Gaudi::TrackVector(); // reference trajectory
  this->init( cluster, det, false );
}

void VeloRMeasurement::init( const VeloCluster& cluster,
                             const DeVelo& det,
                             bool refIsSet ) 
{
  // Fill the data members
  m_mtype = Measurement::VeloR;
  m_refIsSet  = refIsSet;
  m_cluster = &cluster;
  m_lhcbID = LHCbID( m_cluster->channelID() );
  const DeVeloRType* rDet=det.rSensor( m_cluster->channelID().sensor() );
  m_z = rDet->z();
  m_trajectory = det.trajectory( m_lhcbID, m_cluster->interStripFraction() );

  double sum   = 0.;
  double sum2  = 0.;
  double sums  = 0.;

  std::vector < VeloChannelID > channels = m_cluster->channels();
  std::vector< VeloChannelID >::const_iterator iChan;
  for( iChan = channels.begin() ; iChan !=  channels.end() ; ++iChan ) {
    double radius= rDet->rOfStrip( (*iChan).strip() );
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
    double pitch =   rDet->rPitch( m_cluster->channelID().strip() );
    m_errMeasure    = 0.8 * ( pitch / sum ) * sqrt( sum2 / 12 );
    // m_errMeasure = 0.254*pitch - 0.0049*mm;
    // MM-
  }
}


