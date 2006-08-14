
//-----------------------------------------------------------------------------
/** @file RichMatchTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : RichMatchTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichMatchTrackSelector.h,v 1.1 2006-08-14 10:05:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichMatchTrackSelector_H
#define RICHRECTOOLS_RichMatchTrackSelector_H 1

// base class
#include "RichBaseTrackSelector.h"

/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class RichMatchTrackSelector RichMatchTrackSelector.h
   *
   *  Extension to the RichBaseTrackSelector for Seed tracks
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-08-12
   */
  //-----------------------------------------------------------------------------

  class RichMatchTrackSelector : public RichBaseTrackSelector
  {

  public: // Gaudi methods

    /// Standard constructor
    RichMatchTrackSelector( const std::string& type,
                            const std::string& name,
                            const IInterface* parent );

    /// Destructor
    virtual ~RichMatchTrackSelector( );

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

} // RICH namespace

#endif // RICHRECTOOLS_RichMatchTrackSelector_H
