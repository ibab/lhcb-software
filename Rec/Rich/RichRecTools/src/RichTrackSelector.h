
//-----------------------------------------------------------------------------
/** @file RichTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::TrackSelector
 *
 *  CVS Log :-
 *  $Id: RichTrackSelector.h,v 1.5 2007-02-02 10:10:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichTrackSelector_H
#define RICHRECTOOLS_RichTrackSelector_H 1

// base class
#include "RichRecBase/RichTrackSelectorBase.h"

/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
namespace Rich
{

  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class TrackSelector RichTrackSelector.h
     *
     *  A utility tool for the RICH reconstruction providing generic
     *  track selection methods.
     *
     *  Simply implements a RichTrackSelectorBase as a component tool
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2006-08-12
     */
    //-----------------------------------------------------------------------------

    class TrackSelector : public RichTrackSelectorBase
    {

    public: // Gaudi methods

      /// Standard constructor
      TrackSelector( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

      /// Destructor
      virtual ~TrackSelector( );

    };

  }
} // RICH namespace

#endif // RICHRECTOOLS_RichTrackSelector_H
