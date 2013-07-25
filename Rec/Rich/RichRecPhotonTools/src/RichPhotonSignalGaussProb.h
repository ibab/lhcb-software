
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

      /// Temporary local value for Radii of curvature
      double m_radiusCurv[Rich::NRiches];

      /// Temporary local value pixel area
      double m_pixelArea;

      // chaced parameters
      double m_minArg;
      double m_expMinArg;

    };

  }
}

#endif // RICHRECTOOLS_RICHPHOTONSIGNALGAUSSPROB_H
