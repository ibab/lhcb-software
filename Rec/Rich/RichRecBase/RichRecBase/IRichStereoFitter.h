
//-----------------------------------------------------------------------------
/** @file IRichStereoFitter.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IStereoFitter
 *
 *  @author Luigi Delbuono   delbuono@in2p3.fr
 *  @author Chris Jones     Christopher.Rob.Jones@cern.ch
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
     *  Interface for tool to perform a fit to a group of photons, as used by the 
     *  Stereographic refitting PID algorithm
     *
     *  @author Luigi Delbuono  delbuono@in2p3.fr
     *  @author Chris Jones     Christopher.Rob.Jones@cern.ch
     *  @date   20/06/2007
     */
    //--------------------------------------------------------------------------------------

    class IStereoFitter : public virtual IAlgTool
    {

    public: // utility classes

      //--------------------------------------------------------------------------------------
      /** @class Configuration IRichStereoFitter.h RichRecBase/IRichStereoFitter.h
       *
       *  Utility class for Rich::Rec::IStereoFitter tools.
       *
       *  Used to pass configuration options to the fit.
       *
       *  @author Luigi Delbuono  delbuono@in2p3.fr
       *  @author Chris Jones     Christopher.Rob.Jones@cern.ch
       *  @date   20/06/2007
       */
      //--------------------------------------------------------------------------------------
      class Configuration
      {
      public:
        /// Default constructor
        explicit Configuration( const Rich::ParticleIDType _pid = Rich::Pion ) :
          pidType          ( _pid   ),
          minRingPhotons   ( 4      ),
          nthcPhotSigMax   ( 2      ),
          ncandsPerPixMax  ( 3      ),
          maxBkgProb       ( 999    ),
          updateSegment    ( false  ),
          updatePhotons    ( false  )
        { }
      public:
        Rich::ParticleIDType pidType; ///< The mass hypothesis for which the angle should be fitted
        int     minRingPhotons;       ///< Minimum allowed photons on the ring
        float   nthcPhotSigMax;       ///< Size of photons selection window in units of Cherenkov angle RMS
        int     ncandsPerPixMax;      ///< Maximum allowed number of candidate photons per pixel
        float   maxBkgProb;           ///< Maximum allowed background probability of photons
        bool    updateSegment;        ///< Update the track segment with the fitted information ?
        bool    updatePhotons;        ///< Update the selected photons with the fitted information ?
      public:
        /// Implement the ostream << printout method
        friend inline std::ostream& operator << ( std::ostream& s,
                                                  const Configuration& config )
        {
          return s << "[ pid= "   << config.pidType
                   << " minRingPhotons="   << config.minRingPhotons
                   << " nthcPhotSigMax="   << config.nthcPhotSigMax
                   << " ncandsPerPixMax="  << config.ncandsPerPixMax
                   << " maxBkgProb="       << config.maxBkgProb
                   << " ]";
        }
      };

      //--------------------------------------------------------------------------------------
      /** @class Result IRichStereoFitter.h RichRecBase/IRichStereoFitter.h
       *
       *  Utility class for Rich::Rec::IStereoFitter tools.
       *
       *  The result of the fit.
       *
       *  @author Luigi Delbuono  delbuono@in2p3.fr
       *  @author Chris Jones     Christopher.Rob.Jones@cern.ch
       *  @date   20/06/2007
       */
      //--------------------------------------------------------------------------------------
      class Result
      {
      public:
        /** @enum FitStatus
         *
         *  Enumeration for the status of the fit
         *
         *  @author Luigi Delbuono  delbuono@in2p3.fr
         *  @author Chris Jones     Christopher.Rob.Jones@cern.ch
         *  @date   20/06/2007
         */
        enum FitStatus
          {
            Undefined = -1, ///< Fit status is undefined
            Failed    = 0,  ///< The fit failed
            Succeeded = 1   ///< The fit was successful
          };
      public:
        /// Default constructor
        explicit Result( const FitStatus _stat = Undefined ) :
          status     ( _stat   ),
          chi2       ( 9999999 ),
          probChi2   ( 0       ),
          ndof       ( 0       ),
          thcFit     ( 0       ),
          thcFitErr  ( 0       ),
          thphotErr  ( 0       ) { }
      public:
        FitStatus status;  ///< The fit status
        float  chi2;       ///< The Chi^2 of the circle fit
        float  probChi2;   ///< The chi^2 probability of circle fit
        int    ndof;       ///< The number of degrees of freedom of circle fit
        float  thcFit;     ///< The fitted Cherenkov angle
        float  thcFitErr;  ///< The error on the fitted Cherenkov angle
        float  thphotErr;  ///< The individual photon Cherenkov angle error
      public:
        /// Implement the ostream << printout method
        friend inline std::ostream& operator << ( std::ostream& s,
                                                  const Result& result )
        {
          return s << "[ Status= "   << result.status
                   << " chi2="       << result.chi2
                   << " probChi2="   << result.probChi2
                   << " ndof="       << result.ndof
                   << " ckTheta="    << result.thcFit
                   << " ckThetaErr=" << result.thcFitErr
                   << " photErr="    << result.thphotErr
                   << " ]";
        }
      };

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichStereoFitter; }

      /** Projects (RichRecPhotons) photons in the stereo plane perpendicular to the
       *  segment direction vector.
       *
       *  @param[in]  richSegment The RichRecSegment for which photons have to be fitted
       *  @param[in]  config  The fit configuration
       *
       *  @return Fit result object
       */
      virtual IStereoFitter::Result
      Fit( LHCb::RichRecSegment *richSegment,
           const IStereoFitter::Configuration & config ) const = 0;

    };

  }
}

#endif // RICHRECTOOLS_IRICHSTEREOFITTER_H
