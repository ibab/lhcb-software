
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

// STL
#include <sstream>

// base class
#include "RichRecBase/RichRecHistoToolBase.h"

// RichDet
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

// VDT
#include "vdt/log.h"

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
      StatusCode setUseOfPmtFlags();

      /// Loads on demand the alternate geometry
      inline IGeometryInfo * altGeom() const 
      {
        if ( m_useAltGeom && !m_altGeom ) 
        { m_altGeom = getDet<IDetectorElement>(m_altGeomLoc); }
        return ( m_altGeom ? m_altGeom->geometry() : NULL );
      }
      
    private: // data

      /// Pointer to RichCherenkovAngle interface
      const ICherenkovAngle * m_ckAngle = nullptr;

      /// Pointer to refractive index tool
      const ITrackEffectiveRefractiveIndex * m_refIndex = nullptr;

      /// Pointer to RichParticleProperties interface
      const IParticleProperties * m_richPartProp = nullptr;

      /// Detector parameters
      const IDetParameters * m_detParams = nullptr;

      /// Track extrapolation tool
      mutable ITrackExtrapolator * m_trExt = nullptr; 
      /// Track extrapolation tool name
      std::string m_Ext; 

      /// Transport Service
      mutable ITransportSvc * m_transSvc = nullptr;

      /// Scattering coefficent. should be used with p in GeV
      const double m_scatt = 13.6e-03; 

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

      /// Pointer to Rich1 detector element
      DeRich1 * m_rich1DE = nullptr;

      /// Pointer to a PD Panel
      DeRichPDPanel * m_aRichPDPanel = nullptr;

      /// Flag to turn on the use of an alternative geometry for the radiation length calculation
      bool m_useAltGeom;
      
      /// The location of the fast geometry to use
      std::string m_altGeomLoc;

      /// Pointer to the DetElm for the alternate geometry
      mutable IDetectorElement * m_altGeom = nullptr;

      /// Possible uses of Grand Pixel for pmts
      bool m_useOfGrandPixPmt = false;
      bool m_rich2UseMixedPmt = false;
      bool m_usePDWithPMT     = false;
      bool m_useUpgradeOptics = false;
      double m_grandPmtPixelSizeFactor;

    };

  }
}

#endif // RICHRECTOOLS_RichFunctionalCKResForRecoTracks_H
