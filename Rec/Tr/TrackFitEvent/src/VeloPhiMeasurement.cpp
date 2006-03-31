// $Id: VeloPhiMeasurement.cpp,v 1.11 2006-03-31 17:13:14 jvantilb Exp $
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
                                        const Gaudi::TrackVector& refVector )
{
  m_refVector = refVector; // reference trajectory
  this->init( cluster, det, true );
}

/// Standard constructor, initializes variables
VeloPhiMeasurement::VeloPhiMeasurement( const VeloCluster& cluster,
                                        const DeVelo& det)
{
  m_refVector = Gaudi::TrackVector(); // reference trajectory
  this->init( cluster, det, false );
}

void VeloPhiMeasurement::init( const VeloCluster& cluster,
                               const DeVelo& det,
                               bool refIsSet ) 
{
  // Fill the data members
  m_mtype = Measurement::VeloPhi;
  m_refIsSet  = refIsSet;
  m_cluster = &cluster;
  m_lhcbID = LHCbID( m_cluster->channelID() );
  const DeVeloPhiType* phiDet=det.phiSensor( m_cluster->channelID().sensor() );
  m_z = phiDet->z();
  m_trajectory = det.trajectory( m_lhcbID, m_cluster->interStripFraction() );

  // TODO: Get this from the VeloDet
  m_origin = Gaudi::XYZPoint(0,0,0);

  // Store only the 'position', which is the signed distance from strip to
  // the origin.
  m_measure = phiDet->distToOrigin(  m_cluster->channelID().strip() );
  // fix sign convention of d0 of strip
  if( ! phiDet->isDownstream() ) {
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
    double pitch =   phiDet->phiPitch( m_cluster->channelID().strip() );
    m_errMeasure  = 0.8 * ( pitch / sum) * sqrt( sum2 / 12 );
    // m_errMeasure = 0.254*pitch - 0.0049*mm;
    // MM-
  }

}

double VeloPhiMeasurement::resolution( const Gaudi::XYZPoint& point,
                                       const Gaudi::XYZVector& /*vec*/) const
{
  // Retrieve the measurement error
  double radius = (m_origin - point).R();
  return radius * m_errMeasure;
}

double VeloPhiMeasurement::resolution2( const Gaudi::XYZPoint& point,
                                        const Gaudi::XYZVector& /*vec*/) const
{
  // Retrieve the measurement error
  double radius = (m_origin - point).R();
  return radius * m_errMeasure * radius * m_errMeasure;
}
