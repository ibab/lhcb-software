
//-----------------------------------------------------------------------------
/** @file RichPIDPlots.h
 *
 *  Header file for RICH tool : Rich::PIDPlots
 *
 *  CVS Log :-
 *  $Id: RichPIDPlots.h,v 1.1 2008-04-29 12:52:30 jonrob Exp $
 *
 *  @author Chris Jones     Christopher.Rob.Jones@cern.ch
 *  @date   2008-04-14
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPIDPLOTS_H
#define RICHRECTOOLS_RICHPIDPLOTS_H 1

// base class
#include "RichRecBase/RichRecHistoToolBase.h"

// interfaces
#include "RichRecBase/IRichPIDPlots.h"
#include "RichRecBase/IRichTrackSelector.h"

// Event
#include "Event/Track.h"
#include "Event/RichPID.h"

namespace Rich
{
  namespace Rec
  {

    /** @class PIDPlots RichPIDPlots.h
     *
     *  Tool which creates standardised PID plots for
     *  monitoring and calibration purposes.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2008-04-14
     */

    class PIDPlots : public HistoToolBase,
                     virtual public IPIDPlots
    {

    public:

      /// Standard constructor
      PIDPlots( const std::string& type,
                const std::string& name,
                const IInterface* parent );

      virtual ~PIDPlots( ); ///< Destructor

      virtual StatusCode initialize();  // Tool initialization

    public:

      // Fill the plots for the given RichPID and PID hypothesis
      virtual void plots( const LHCb::RichPID * pid,
                          const Rich::ParticleIDType reco_hypo,
                          const Rich::ParticleIDType true_hypo ) const;

      // Fill the plots for the given track and PID hypothesis
      virtual void plots( const LHCb::Track * track,
                          const Rich::ParticleIDType reco_hypo,
                          const Rich::ParticleIDType true_hypo ) const;

      // Set the track selector for this tool to use. Useful to allow this plotting
      virtual void setTrackSelector( const Rich::Rec::ITrackSelector * selector ) const;

    private:

      /// Get the track momentum (first State) in GeV
      inline double trackP( const LHCb::Track * track ) const
      {
        return ( track ? track->firstState().p()/Gaudi::Units::GeV : 0 );
      }

      /// Get the track momentum (first State) in GeV
      inline double trackP( const LHCb::RichPID * pid ) const
      {
        return ( pid ? trackP(pid->track()) : 0 );
      }

      /// Return the correct histogram path
      inline const std::string hPath( const Rich::ParticleIDType reco_hypo,
                                      const Rich::ParticleIDType true_hypo ) const
      {
        return "Reco"+Rich::text(reco_hypo)+"/True"+Rich::text(true_hypo)+"/";
      }

    private: // data

      mutable const ITrackSelector * m_trSelector;  ///< Track selector

      // Job Options
      int m_bins;                    ///< Number of bins
      bool m_extraHistos;            ///< Fill full set of histograms

    };

  }
}

#endif // RICHRECTOOLS_RICHPIDPLOTS_H
