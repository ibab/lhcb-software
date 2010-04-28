
//-----------------------------------------------------------------------------
/** @file IRichPIDPlots.h
 *
 *  Header file for RICH tool interface : Rich::Rec::IPIDPlots
 *
 *  CVS Log :-
 *  $Id: IRichPIDPlots.h,v 1.4 2008-05-12 14:03:53 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2008-04-14
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_IRICHPIDPLOTS_H
#define RICHRECBASE_IRICHPIDPLOTS_H 1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "Kernel/RichParticleIDType.h"

// Event
namespace LHCb
{
  class Track;
  class RichPID;
  class ProtoParticle;
}

/// The interface ID
static const InterfaceID IID_IRichPIDPlots ( "IRichPIDPlots", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    /** @class IPIDPlots IRichPIDPlots.h RichKernel/IRichPIDPlots.h
     *
     *  Interface to tool which creates standardised PID plots for
     *  monitoring and calibration purposes.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2008-04-14
     */

    class IPIDPlots : virtual public IAlgTool
    {

    public:

      /** @class Configuration IRichPIDPlots.h RichKernel/IRichPIDPlots.h
       *
       *  Utility class used to pass some basic configuration options
       *  to the tool to use when creating the reference histograms.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   2008-04-14
       */
      class Configuration
      {
      public:
        /// Default constructor with default parameters
        Configuration() 
          : minP  (   2 * Gaudi::Units::GeV ),
            maxP  ( 100 * Gaudi::Units::GeV ),
            minPt (   0 * Gaudi::Units::GeV ),
            maxPt (  10 * Gaudi::Units::GeV ) { }
      public:
        double minP;  ///< The minimum track momentum (MeV/c)
        double maxP;  ///< The maximum track momentum (MeV/c)
        double minPt; ///< The minimum track transverse (to z-axis) momentum (MeV/c)
        double maxPt; ///< The maximum track transverse (to z-axis) momentum (MeV/c)
      };

    public:

      /// Return the interface ID
      static const InterfaceID& interfaceID() { return IID_IRichPIDPlots; }

      /** Fill the plots for the given RichPID data object and PID hypothesis
       *
       *  @param[in] pid    Pointer to the RichPID data object to fill plots for
       *  @param[in] hypo   The mass hypothesis to assume for this RichPID
       *  @param[in] config (Optional) Configuration object
       */
      virtual void plots( const LHCb::RichPID * pid,
                          const Rich::ParticleIDType hypo,
                          const Configuration & config = Configuration() ) const = 0;

      /** Fill the plots for the given track and PID hypothesis
       *
       *  @param[in] track  Pointer to the track to fill plots for
       *  @param[in] hypo   The mass hypothesis to assume for this track
       *  @param[in] config (Optional) Configuration object
       */
      virtual void plots( const LHCb::Track * track,
                          const Rich::ParticleIDType hypo,
                          const Configuration & config = Configuration() ) const = 0;

      /** Fill the plots for the given ProtoParticle and PID hypothesis
       *
       *  @param[in] proto  Pointer to the ProtoParticle to fill plots for
       *  @param[in] hypo   The mass hypothesis to assume for this track
       *  @param[in] config (Optional) Configuration object
       */
      virtual void plots( const LHCb::ProtoParticle * proto,
                          const Rich::ParticleIDType hypo,
                          const Configuration & config = Configuration() ) const = 0;

    };

  }
}

#endif // RICHRECBASE_IRICHPIDPLOTS_H
