
//-----------------------------------------------------------------------------
/** @file RichPhotonSignal.h
 *
 *  Header file for tool : Rich::Rec::PhotonSignal
 *
 *  CVS Log :-
 *  $Id: RichPhotonSignal.h,v 1.15 2007-02-02 10:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPHOTONSIGNAL_H
#define RICHRECTOOLS_RICHPHOTONSIGNAL_H 1

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

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class PhotonSignal RichPhotonSignal.h
     *
     *  Tool to calculate for a given photon the probabilities of it
     *  being a signal or scattered photon, and its predicted contribution
     *  to its associated RichRecPixel, under a certain mass hypothesis.
     *  This version is tuned for HPDs
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class PhotonSignal : public Rich::Rec::ToolBase,
                         virtual public IPhotonSignal
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      PhotonSignal( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

      /// Destructor
      virtual ~PhotonSignal(){}

      // Initialize method
      StatusCode initialize();

      // Finalize method
      StatusCode finalize();

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

    };

  }
}

#endif // RICHRECTOOLS_RICHPHOTONSIGNAL_H
