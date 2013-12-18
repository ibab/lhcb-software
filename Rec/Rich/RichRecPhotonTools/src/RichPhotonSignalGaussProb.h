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

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

// VDT
#include "vdt/exp.h"

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
                                  virtual public IPhotonSignal
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

    protected: // methods

      // compute the acutal signal probability (Gaussian)
      virtual double signalProbFunc( const double thetaDiff,
                                     const double thetaExpRes ) const;

    private: // private data

      /// Pointer to RichExpectedTrackSignal interface
      const IExpectedTrackSignal * m_signal;

      /// Pointer to RichCherenkovAngle interface
      const ICherenkovAngle * m_ckAngle;

      /// Pointer to RichCherenkovResolution interface
      const ICherenkovResolution * m_ckRes;

      /// Cached Radii of curvature
      double m_radiusCurv[Rich::NRiches];
      
      /// Cached pixel area
      double m_pixelArea[Rich::NRiches];

      /// Cached Scale factors
      double m_scaleFactor[Rich::NRiches];

      // Pixel Signal scale factor for RICH2
      double m_rich2PixelSignalScaleFactor;
      
      
      // chaced parameters
      double m_minArg;
      double m_expMinArg;

      double m_stdPixelArea;
      double m_grandPixelArea;

      DeRichPDPanel * m_aRichPDPanel;
      bool m_pmtActivate;
      bool m_useGrandPmtInRich2;
      bool m_useMixedPmtInRich2;
    };

  }
}

#endif // RICHRECTOOLS_RICHPHOTONSIGNALGAUSSPROB_H
