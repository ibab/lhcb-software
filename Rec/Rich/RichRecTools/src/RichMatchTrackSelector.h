
//-----------------------------------------------------------------------------
/** @file RichMatchTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::MatchTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichMatchTrackSelector.h,v 1.3 2007-03-10 13:19:20 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichMatchTrackSelector_H
#define RICHRECTOOLS_RichMatchTrackSelector_H 1

// base class
#include "RichBaseTrackSelector.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class MatchTrackSelector RichMatchTrackSelector.h
     *
     *  Extension to the RichBaseTrackSelector for Seed tracks
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2006-08-12
     */
    //-----------------------------------------------------------------------------

    class MatchTrackSelector : public BaseTrackSelector
    {

    public: // Gaudi methods

      /// Standard constructor
      MatchTrackSelector( const std::string& type,
                          const std::string& name,
                          const IInterface* parent );

      /// Destructor
      virtual ~MatchTrackSelector( );

    public: // interface methods

      /// Test if the given Track is selected under the current criteria
      virtual bool trackSelected( const LHCb::Track * track ) const;

      /// Test it the given RichRecTrack is selected
      virtual bool trackSelected( const LHCb::RichRecTrack * track ) const;

    protected: // methods

      /// Print the track selection
      virtual MsgStream & printSel( MsgStream & os ) const;

    private: // data

      double m_minMatchChi2; ///< minimum match track matching chi^2
      double m_maxMatchChi2; ///< maximum match track matching chi^2

    };

  }
} // RICH namespace

#endif // RICHRECTOOLS_RichMatchTrackSelector_H
