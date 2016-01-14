
//-----------------------------------------------------------------------------
/** @file RichPhotonSignalGaussProb.h
 *
 *  Header file for tool : Rich::Rec::PhotonSignalGaussProb
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPHOTONSIGNALGAUSSPROB_H
#define RICHRECTOOLS_RICHPHOTONSIGNALGAUSSPROB_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event
#include "Event/RichRecPhoton.h"

// Detector Description
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichPDPanel.h"

// Interfaces
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichKernel/IRichParticleProperties.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

// VDT
#include "vdt/exp.h"
#include "vdt/sincos.h"

// GSL
#include "gsl/gsl_math.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class PhotonSignalGaussProb RichPhotonSignalGaussProb.h
     *
     *  Tool to calculate for a given photon the probabilities of it
     *  being a signal or scattered photon, and its predicted contribution
     *  to its associated RichRecPixel, under a certain mass hypothesis.
     *  
     *  This version uses a simple Gaussian probability function for the signal
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class PhotonSignalGaussProb : public Rich::Rec::ToolBase,
                                  virtual public IPhotonSignal,
                                  virtual public IIncidentListener
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      PhotonSignalGaussProb( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

      /// Destructor
      virtual ~PhotonSignalGaussProb() {}

      // Initialize method
      StatusCode initialize();

      /// Implement the handle method for the Incident service.
      void handle( const Incident& incident );

    public: // methods (and doxygen comments) inherited from public interface

      // Predicted pixel signal for a given reconstructed photon under a given mass hypothesis
      double predictedPixelSignal( LHCb::RichRecPhoton * photon,
                                   const Rich::ParticleIDType id ) const;

      // Signal Probability for a given reconstructed photon under a given mass hypothesis
      double signalProb( LHCb::RichRecPhoton * photon,
                         const Rich::ParticleIDType id ) const;

      // Scatter Probability for a given reconstructed photon under a given mass hypothesis
      double scatterProb( LHCb::RichRecPhoton * photon,
                          const Rich::ParticleIDType id ) const;

      /// Method to pre-fill the predicted pixel signal values for all photons
      void prefillPredictedPixelSignal() const;

    protected: // methods

      // compute the actual signal probability (Gaussian)
      virtual double signalProbFunc( const double thetaDiff,
                                     const double thetaExpRes ) const;

    private: // methods

      /// Compute all hypothesis independent term of the predictedPixelSignal calculation
      inline double _predictedPixelSignal( LHCb::RichRecPhoton * photon ) const
      {
        // Which detector
        const Rich::DetectorType det = photon->richRecSegment()->trackSegment().rich();
        // Reconstructed Cherenkov theta angle
        const double thetaReco = photon->geomPhoton().CherenkovTheta();
        // Get the appropriate pixel area
        const double aPixelArea = ( m_useMixedPmtInRich2 && m_pmtActivate && (det == Rich::Rich2) ?
                                    ( m_aRichPDPanel->pdGrandSize(photon->richRecPixel()->hpd()) ?
                                      m_grandPixelArea : m_stdPixelArea ) : m_pixelArea[det] );
        // return the ID independent term
        return ( photon->geomPhoton().activeSegmentFraction() * m_scaleFactor[det] * aPixelArea / 
                 ( m_radiusCurv[det] * m_radiusCurv[det] * (thetaReco>1e-10 ? thetaReco : 1e-10) ) );
      }

      /// Compute the hypothesis dependent term of the predictedPixelSignal calculation
      inline double _predictedPixelSignal( LHCb::RichRecPhoton * photon,
                                           const Rich::ParticleIDType id ) const
      {
        double sig = ( signalProb(photon,id) *
                       m_signal->nSignalPhotons(photon->richRecSegment(),id) );
        if ( Rich::Aerogel == photon->richRecSegment()->trackSegment().radiator() )
        {
          sig += ( scatterProb(photon,id) *
                   m_signal->nScatteredPhotons(photon->richRecSegment(),id) );
        }
        return sig;
      }

      /// The exponential function to use
      template< class TYPE >
      inline TYPE _exp( const TYPE& x ) const 
      { 
        return (TYPE)vdt::fast_exp(x); 
      }

    private: // private data

      /// Pointer to RichExpectedTrackSignal interface
      const IExpectedTrackSignal * m_signal = nullptr;

      /// Pointer to RichCherenkovAngle interface
      const ICherenkovAngle * m_ckAngle = nullptr;

      /// Pointer to RichCherenkovResolution interface
      const ICherenkovResolution * m_ckRes = nullptr;

      /// Pointer to RichParticleProperties interface
      const IParticleProperties * m_richPartProp = nullptr;

      /// Cached Radii of curvature
      double m_radiusCurv[Rich::NRiches];
      
      /// Cached pixel area
      double m_pixelArea[Rich::NRiches];

      /// Cached Scale factors
      double m_scaleFactor[Rich::NRiches];

      // Pixel Signal scale factor for RICH2
      double m_rich2PixelSignalScaleFactor;
      
      /// Particle ID types to consider
      Rich::Particles m_pidTypes;

      // cached parameters

      /// Flag to indicate if the pre-filling of information has been performed.
      mutable bool m_prefillDone = false; 

      double m_minArg    = 0.0;
      double m_expMinArg = 0.0;

      double m_stdPixelArea   = 0.0;
      double m_grandPixelArea = 0.0;

      DeRichPDPanel * m_aRichPDPanel = nullptr;
      bool m_pmtActivate        = false;
      bool m_useGrandPmtInRich2 = false;
      bool m_useMixedPmtInRich2 = false;

    };

  }
}

#endif // RICHRECTOOLS_RICHPHOTONSIGNALGAUSSPROB_H
