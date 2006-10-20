
//----------------------------------------------------------------------------------------
/** @file RichFunctionalCKResForRecoTracks.h
 *
 *  Header file for tool : RichFunctionalCKResForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichFunctionalCKResForRecoTracks.h,v 1.2 2006-10-20 13:17:00 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/10/2004
 */
//----------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichFunctionalCKResForRecoTracks_H
#define RICHRECTOOLS_RichFunctionalCKResForRecoTracks_H 1

// base class
#include "RichRecBase/RichRecHistoToolBase.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichKernel/IRichRefractiveIndex.h"
#include "RichKernel/IRichParticleProperties.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

// DetDesc
#include "DetDesc/ITransportSvc.h"

// GSL
#include "gsl/gsl_math.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// kernel
#include "RichKernel/RichGeomFunctions.h"

//----------------------------------------------------------------------------------------
/** @class RichFunctionalCKResForRecoTracks RichFunctionalCKResForRecoTracks.h
 *
 *  Provides an analytical calculation of the expected Cherenkoc angle resolution.
 *
 *  Based on results discussed in LHCb-2004-057
 *  see http://cdsweb.cern.ch/search.py?recid=815494&ln=en
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-04-07
 *
 *  @todo Remove hardcoded scattering treatment of effectiveLength once TransportService is fast
 */
//----------------------------------------------------------------------------------------

class RichFunctionalCKResForRecoTracks : public RichRecHistoToolBase,
                                         virtual public IRichCherenkovResolution
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichFunctionalCKResForRecoTracks( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent);

  /// Destructor
  virtual ~RichFunctionalCKResForRecoTracks() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Photon resolution
  double ckThetaResolution( LHCb::RichRecSegment * segment,
                            const Rich::ParticleIDType id = Rich::Pion ) const;

private: // methods

  /// find the position of the last measured point
  bool findLastMeasuredPoint( LHCb::RichRecSegment * segment,
                              Gaudi::XYZPoint & point ) const;


  /// Access transport service on-demand
  ITransportSvc * transSvc() const
  {
    if (!m_transSvc) { m_transSvc = svc<ITransportSvc>( "TransportSvc", true  ); }
    return m_transSvc;
  }

  /// Access track extrapolator on-demand
  ITrackExtrapolator * trackExtrap() const
  {
    if (!m_trExt) { m_trExt = tool<ITrackExtrapolator>( m_Ext ); }
    return m_trExt;
  }

private: // data

  /// Pointer to RichCherenkovAngle interface
  const IRichCherenkovAngle * m_ckAngle;

  /// Pointer to refractive index tool
  const IRichRefractiveIndex * m_refIndex;

  /// Pointer to RichParticleProperties interface
  const IRichParticleProperties * m_richPartProp;

  mutable ITrackExtrapolator * m_trExt; ///< Track extrapolation tool
  std::string m_Ext; ///< Track extrapolation tool name

  /// Transport Service
  mutable ITransportSvc * m_transSvc;

  std::vector<double> m_chromFact; ///< Chromatic error factor for each radiator

  std::vector<double> m_matThickness; ///< Material thickness infront of each radiator

  double m_scatt; ///< Scattering coefficent

  /// Asymtopic Errors
  std::vector<double> m_asmpt[Rich::NRadiatorTypes];

  /// Flag to turn on the use of the full TransportService in calculating the effective length of a track
  bool m_useTS;

};

#endif // RICHRECTOOLS_RichFunctionalCKResForRecoTracks_H
