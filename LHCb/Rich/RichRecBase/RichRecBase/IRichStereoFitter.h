
//-----------------------------------------------------------------------------
/** @file IRichStereoFitter.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IStereoFitter
 *
 *  CVS Log :-
 *  $Id: IRichStereoFitter.h,v 1.1 2007-09-04 16:04:45 jonrob Exp $
 *
 *  @author Luigi Delbuono   delbuono@in2p3.fr
 *  @date   27/06/2007
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHSTEREOFITTER_H
#define RICHRECTOOLS_IRICHSTEREOFITTER_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event model
namespace LHCb
{
  class RichRecSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichStereoFitter( "IRichStereoFitter", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    //--------------------------------------------------------------------------------------
    /** @class IStereoFitter IRichStereoFitter.h RichRecBase/IRichStereoFitter.h
     *
     *  Interface for tool to perform a fit to a group of photons, as used by the Stereographic
     *  PID algorithm
     *
     *  @author Luigi Delbuono  delbuono@in2p3.fr
     *  @date   20/06/2007
     */
    //--------------------------------------------------------------------------------------

    class IStereoFitter : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichStereoFitter; }

      /** Projects (RichRec) photons in the stereo plane perpendicular to the segment direction
       *
       *  @param[in] segment The RichRecSegment which photons have to be fitted
       *  @param[in] pidType The mass hypothesis for which the angle should be fitted
       *  @param[in] MinRingPhotons minimum allowed photons on the ring
       *  @param[in] MaxRingPhotons maximum allowed photons on the ring
       *  @param[in] nthcPhotSigMax size of photons selection window in units of Cerenkov angle RMS
       *  @param[in] ncandsPerPixMax Maximum allowed number of candidate photons per pixel
       *  @param[in] maxBkgProb Maximum allowed background probability of photons
       *  @param[out] Chi2 returned chi square of circle fit
       *  @param[out] probChi2 returned chi square probability of circle fit
       *  @param[out] ndof returned number of degrees of freedom of circle fit
       *  @param[out] thcFit returned fitted Cerenkov angle
       *  @param[out] thcFitErr returned fitted Cerenkov angle error
       *  @param[out] thphotErr returned individual photon Cerenkov angle error
       *
       *  @return status flag
       *  @retval 0 means abnormal termination
       *  @retval 1 means normal completion
       */
      virtual int Fit( LHCb::RichRecSegment *richSegment, 
                       const Rich::ParticleIDType pidType,
                       int MinRingPhotons, double nthcPhotSigMax, 
                       int ncandsPerPixMax, double maxBkgProb,
                       double &Chi2, double &probChi2, int &ndof, double &thcFit, 
                       double &thcFitErr, double &thphotErr ) const = 0;


    };

  }
}

#endif // RICHRECTOOLS_IRICHSTEREOFITTER_H
