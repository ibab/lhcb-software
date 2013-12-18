//----------------------------------------------------------------------------------------
/** @file RichFunctionalCKResForRecoTracks.h
 *
 *  Header file for tool : Rich::Rec::FunctionalCKResForRecoTracks
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/10/2004
 */
//----------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichFunctionalCKResForRecoTracks_H
#define RICHRECTOOLS_RichFunctionalCKResForRecoTracks_H 1

// base class
#include "RichRecBase/RichRecHistoToolBase.h"
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichPDPanel.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichKernel/IRichParticleProperties.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "RichRecBase/IRichTrackEffectiveRefractiveIndex.h"
#include "RichKernel/IRichDetParameters.h"

// DetDesc
#include "DetDesc/ITransportSvc.h"
#include "DetDesc/TransportSvcException.h"

// GSL
#include "gsl/gsl_math.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// kernel
#include "RichKernel/RichGeomFunctions.h"

// boost
#include "boost/assign/list_of.hpp"

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

      /// Set flags for geometry, pmt etc.
      void setUseOfPmtFlags();
      
    private: // data

      /// Pointer to RichCherenkovAngle interface
      const ICherenkovAngle * m_ckAngle;

      /// Pointer to refractive index tool
      const ITrackEffectiveRefractiveIndex * m_refIndex;

      /// Pointer to RichParticleProperties interface
      const IParticleProperties * m_richPartProp;

      /// Detector parameters
      const IDetParameters * m_detParams;

      mutable ITrackExtrapolator * m_trExt; ///< Track extrapolation tool
      std::string m_Ext; ///< Track extrapolation tool name

      /// Transport Service
      mutable ITransportSvc * m_transSvc;

      double m_scatt; ///< Scattering coefficent

      /// Asymtopic Errors
      std::vector<double> m_asmpt[Rich::NRadiatorTypes];

      /// Particle ID types to consider in the photon creation checks
      Rich::Particles m_pidTypes;

      /// Absolute max CK theta resolution per radiator
      std::vector<float> m_maxRes;

      /// RICH HPD contributions to CK theta resolution
      std::vector<double> m_hpdErr;

      /// Use last measured point when calculating material traversed
      std::vector<bool> m_useLastMP;

      /// Overall factors for each radiator
      std::vector<double> m_scale;
     /// Overall factors for Rich2 radiator for MixedPMT
      std::vector<double> m_scaleR2Pmt;
      

      /// Possible uses of Grand Pixel for pmts
      bool m_useOfGrandPixPmt;
      bool m_rich2UseMixedPmt;
      bool m_usePDWithPMT;
      bool m_useUpgradeOptics;
      DeRich1 * m_rich1DE;
      DeRichPDPanel * m_aRichPDPanel;
      double m_grandPmtPixelSizeFactor;
    };

  }
}

#endif // RICHRECTOOLS_RichFunctionalCKResForRecoTracks_H
