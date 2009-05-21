
//-----------------------------------------------------------------------------
/** @file RichBaseTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::BaseTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichBaseTrackSelector.h,v 1.2 2009-05-21 17:29:16 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTRACKTOOLS_RichBaseTrackSelector_H
#define RICHRECTRACKTOOLS_RichBaseTrackSelector_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichBaseTrackSelector.h"

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

      /// Returns the clone rejection cut
      virtual double cloneCut() const;

    protected: // methods

      /// Print the track selection
      virtual MsgStream & printSel( MsgStream & os ) const;

    protected: // data

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

      double m_cloneCut;   ///< Clone rejection cut

    };

  }
} // RICH namespace

#endif // RICHRECTRACKTOOLS_RichBaseTrackSelector_H
