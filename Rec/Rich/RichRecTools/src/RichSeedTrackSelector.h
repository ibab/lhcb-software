
//-----------------------------------------------------------------------------
/** @file RichSeedTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : RichSeedTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichSeedTrackSelector.h,v 1.1 2006-08-13 19:07:30 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichSeedTrackSelector_H
#define RICHRECTOOLS_RichSeedTrackSelector_H 1

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
  /** @class RichSeedTrackSelector RichSeedTrackSelector.h
   *
   *  Extension to the RichBaseTrackSelector for Seed tracks
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-08-12
   */
  //-----------------------------------------------------------------------------

  class RichSeedTrackSelector : public RichBaseTrackSelector
  {

  public: // Gaudi methods

    /// Standard constructor
    RichSeedTrackSelector( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

    /// Destructor
    virtual ~RichSeedTrackSelector( );

  public: // interface methods

    /// Test if the given Track is selected under the current criteria
    virtual bool trackSelected( const LHCb::Track * track ) const;

    /// Test it the given RichRecTrack is selected
    virtual bool trackSelected( const LHCb::RichRecTrack * track ) const;

  protected: // methods

    /// Print the track selection
    virtual MsgStream & printSel( MsgStream & os ) const;

  private: // data

    double m_minSeedLikelihood; ///< minimum seed track likelihood cut
    double m_maxSeedLikelihood; ///< maximum seed track likelihood cut

  };

} // RICH namespace

#endif // RICHRECTOOLS_RichSeedTrackSelector_H
