
//-----------------------------------------------------------------------------
/** @file RichBaseTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::BaseTrackSelector
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTRACKTOOLS_RichBaseTrackSelector_H
#define RICHRECTRACKTOOLS_RichBaseTrackSelector_H 1

// STL
#include <sstream>

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichBaseTrackSelector.h"
#include "RichRecBase/IRichIsolatedTrack.h"

// Event model
#include "Event/Track.h"
#include "Event/RichRecTrack.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"
#include "boost/format.hpp"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class BaseTrackSelector RichBaseTrackSelector.h
     *
     *  Utility tool used by RichBaseTrackSelector which implements core Track
     *  selection criteria.
     *
     *  Users should not use directly but instead use a RichTrackSelector
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2006-08-12
     */
    //-----------------------------------------------------------------------------

    class BaseTrackSelector : public Rich::Rec::ToolBase,
                              virtual public IBaseTrackSelector
    {

    public: // Gaudi methods

      /// Standard constructor
      BaseTrackSelector( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

      /// Destructor
      virtual ~BaseTrackSelector( );

      /// Initialize method
      StatusCode initialize();

      /// Finalize method
      StatusCode finalize();

    public: // interface methods

      /// Test if the given Track is selected under the current criteria
      virtual bool trackSelected( const LHCb::Track * track ) const;

      /// Test it the given RichRecTrack is selected
      virtual bool trackSelected( const LHCb::RichRecTrack * track ) const;

      /// Returns the minimum momentum cut value
      virtual double minPCut() const;

      /// Returns the maximum momentum cut value
      virtual double maxPCut() const;

      /// Returns the minimum pt cut value
      virtual double minPtCut() const;

      /// Returns the maximum pt cut value
      virtual double maxPtCut() const;

      /// Returns the minimum chi^2 cut
      virtual double minChi2Cut() const;

      /// Returns the maximum chi^2 cut
      virtual double maxChi2Cut() const;

      /// Returns the minimum likelihood cut
      virtual double minLikelihoodCut() const;

      /// Returns the maximum likelihood cut
      virtual double maxLikelihoodCut() const;

      /// Returns the charge selection
      virtual int chargeSel() const;

      /// Returns the minimum clone distance rejection cut
      virtual double minCloneDistCut() const;

      /// Returns the maximum clone distance rejection cut
      virtual double maxCloneDistCut() const;

      /// Returns the minimum ghost probaility cut
      virtual double minGhostProbCut() const;

      /// Returns the maximum ghost probaility cut
      virtual double maxGhostProbCut() const;

    protected: // methods

      /// Print the track selection
      virtual void printSel( std::ostringstream & os ) const;

    private:

      /// Access on demand the isolated track tool
      inline const IIsolatedTrack * isoTrackTool() const
      {
        if ( !m_isoTrack ) { acquireTool( "RichIsolatedTrack", m_isoTrack ); }
        return m_isoTrack;
      }

    private: // data

      /// Isolated Track tool
      mutable const IIsolatedTrack * m_isoTrack = nullptr;

      double m_minChi2Cut; ///< Min chi^2 cut
      double m_maxChi2Cut; ///< Max chi^2 cut

      double m_minPCut;    ///< Min p cut
      double m_maxPCut;    ///< Max p cut

      double m_minPtCut;   ///< Min pt cut
      double m_maxPtCut;   ///< Max pt cut

      double m_minLL; ///< minimum track likelihood cut
      double m_maxLL; ///< maximum track likelihood cut

      int m_chargeSel;     ///< Charge selection (-1=negative,+1=positive,0=all)

      bool m_acceptClones; ///< Accept or reject clone tracks

      bool m_rejectNonIsolated; ///< Reject tracks that fail the isolation tool

      double m_minCloneCut;   ///< Minimum clone rejection cut
      double m_maxCloneCut;   ///< Maximum clone rejection cut

      double m_minGhostProb; ///< minimum ghost probability cut
      double m_maxGhostProb; ///< maximum ghost probability cut

      bool m_acceptFitFailures; ///< Accept tracks with a FAILED fit status

      mutable bool m_chiSqCutEnabled      = false;
      mutable bool m_ghostProbCutEnabled  = false;
      mutable bool m_cloneDistCutEnabled  = false;
      mutable bool m_likelihoodCutEnabled = false;
      mutable bool m_pCutEnabled          = false;
      mutable bool m_ptCutEnabled         = false;

    };

  }
} // RICH namespace

#endif // RICHRECTRACKTOOLS_RichBaseTrackSelector_H
