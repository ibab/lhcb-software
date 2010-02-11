
//----------------------------------------------------------------------------------------
/** @file RichFunctionalCKResForRecoTracks.h
 *
 *  Header file for tool : Rich::Rec::FunctionalCKResForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichFunctionalCKResForRecoTracks.h,v 1.3 2010-02-11 20:08:22 jonrob Exp $
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
#include "RichKernel/IRichParticleProperties.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "RichRecBase/IRichTrackEffectiveRefractiveIndex.h"

// DetDesc
#include "DetDesc/ITransportSvc.h"

// GSL
#include "gsl/gsl_math.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// kernel
#include "RichKernel/RichGeomFunctions.h"

namespace Rich
{
  namespace Rec
  {

    //----------------------------------------------------------------------------------------
    /** @class FunctionalCKResForRecoTracks RichFunctionalCKResForRecoTracks.h
     *
     *  Provides an analytical calculation of the expected Cherenkov angle resolution.
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

    class FunctionalCKResForRecoTracks : public Rich::Rec::HistoToolBase,
                                         virtual public ICherenkovResolution
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      FunctionalCKResForRecoTracks( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent);

      /// Destructor
      virtual ~FunctionalCKResForRecoTracks() { }

      // Initialize method
      StatusCode initialize();

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
      const ICherenkovAngle * m_ckAngle;

      /// Pointer to refractive index tool
      const ITrackEffectiveRefractiveIndex * m_refIndex;

      /// Pointer to RichParticleProperties interface
      const IParticleProperties * m_richPartProp;

      mutable ITrackExtrapolator * m_trExt; ///< Track extrapolation tool
      std::string m_Ext; ///< Track extrapolation tool name

      /// Transport Service
      mutable ITransportSvc * m_transSvc;

      std::vector<double> m_matThickness; ///< Material thickness infront of each radiator

      double m_scatt; ///< Scattering coefficent

      /// Asymtopic Errors
      std::vector<double> m_asmpt[Rich::NRadiatorTypes];

      /// Flag to turn on the use of the full TransportService in calculating the effective length of a track
      bool m_useTS;

      /// Particle ID types to consider in the photon creation checks
      Rich::Particles m_pidTypes;

    };

  }
}

#endif // RICHRECTOOLS_RichFunctionalCKResForRecoTracks_H
