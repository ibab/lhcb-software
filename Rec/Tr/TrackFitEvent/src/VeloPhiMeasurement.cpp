// $Id: VeloPhiMeasurement.cpp,v 1.1.1.1 2005-04-07 20:24:28 erodrigu Exp $
// Include files 



// local
#include "Event/VeloPhiMeasurement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloPhiMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 13-07-2000
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloPhiMeasurement::VeloPhiMeasurement( VeloCluster& cluster,
                                        DeVelo& det ) {

  m_cluster = &cluster;
  
  int sensor = m_cluster -> sensor();
  double sum   = 0.;
  double sum2  = 0.;
  double sums  = 0.;
  double phi   = -999.;

  // Compute the average phi direction, and the total width in phi unit
  // Store also the 'position', which is the signed distance from strip to
  // the origin.

  std::vector< std::pair<long,double> > sign = m_cluster->stripSignals();
  std::vector< std::pair<long,double> >::const_iterator strIt;
  int strip    = (*sign.begin()).first;
  VeloChannelID channel(sensor,strip);
  double pitch  = det.phiPitch( channel );
  for ( strIt = sign.begin() ; sign.end() != strIt ; strIt++ ) {
    strip      = (*strIt).first;
    m_measure = det.distToOrigin( channel );
    // fix sign convention of d0 of strip
    if( ! det.isDownstreamSensor(sensor)   ) {
      m_measure = -m_measure;
    }
    phi        =  det.trgPhiDirectionOfStrip( channel );
    sum       += (*strIt).second;
    sum2      += pow((*strIt).second,2) ;
    sums      += (*strIt).second * phi ;
  }

  if ( 0 < sum ) {
    // double phi = sums / sum;
    // m_cosPhi = cos( phi );
    // m_sinPhi = sin( phi );
    // MM+
    // m_errMeasure  = ( pitch / sum) * sqrt( sum2 / 12 );
    m_errMeasure  = 0.8 * ( pitch / sum) * sqrt( sum2 / 12 );
    // m_errMeasure = 0.254*pitch - 0.0049*mm;
    // MM-
  }

  m_z = det.zSensor( m_cluster->sensor() );

}
//=============================================================================
