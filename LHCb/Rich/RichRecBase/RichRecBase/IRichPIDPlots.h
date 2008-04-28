
//-----------------------------------------------------------------------------
/** @file IRichPIDPlots.h
 *
 *  Header file for RICH tool interface : Rich::Rec::IPIDPlots
 *
 *  CVS Log :-
 *  $Id: IRichPIDPlots.h,v 1.1 2008-04-28 10:00:15 jonrob Exp $
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
}
namespace Rich 
{ 
  namespace Rec 
  { 
    class ITrackSelector; 
  } 
}

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

      /// Return the interface ID
      static const InterfaceID& interfaceID() { return IID_IRichPIDPlots; }

      /** Fill the plots for the given RichPID data object and PID hypothesis
       *
       *  @param[in] pid        Pointer to the RichPID data object to fill plots for
       *  @param[in] reco_hypo  The recontructed mass hypothesis to assume for this track
       *  @param[in] true_hypo  The 'true' (e.g. via MC or D*'s) mass hypothesis to assume for this track
       */
      virtual void plots( const LHCb::RichPID * pid,
                          const Rich::ParticleIDType reco_hypo,
                          const Rich::ParticleIDType true_hypo ) const = 0;

      /** Fill the plots for the given track and PID hypothesis
       *
       *  @param[in] track      Pointer to the track to fill plots for
       *  @param[in] reco_hypo  The recontructed mass hypothesis to assume for this track
       *  @param[in] true_hypo  The 'true' (e.g. via MC or D*'s) mass hypothesis to assume for this track
       */
      virtual void plots( const LHCb::Track * track,
                          const Rich::ParticleIDType reco_hypo,
                          const Rich::ParticleIDType true_hypo ) const = 0;

      /** Set the track selector for this tool to use. Useful to allow this plotting
       *  to use the same selection as for its parent tool or algorithm that is calling it.
       *
       *  @param selector Pointer to the track selector tool to use
       *
       *  @attention This is a bit of a hack, to get things working. Would like to remove this eventually.
       */
      virtual void setTrackSelector( const Rich::Rec::ITrackSelector * selector ) const = 0;

    };

  }
}

#endif // RICHRECBASE_IRICHPIDPLOTS_H
