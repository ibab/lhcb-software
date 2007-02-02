
//-----------------------------------------------------------------------------
/** @file RichForwardTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::ForwardTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichForwardTrackSelector.h,v 1.2 2007-02-02 10:10:40 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichForwardTrackSelector_H
#define RICHRECTOOLS_RichForwardTrackSelector_H 1

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
  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class ForwardTrackSelector RichForwardTrackSelector.h
     *
     *  Extension to the RichBaseTrackSelector for Forward tracks
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2006-08-12
     */
    //-----------------------------------------------------------------------------

    class ForwardTrackSelector : public BaseTrackSelector
    {

    public: // Gaudi methods

      /// Standard constructor
      ForwardTrackSelector( const std::string& type,
                            const std::string& name,
                            const IInterface* parent );

      /// Destructor
      virtual ~ForwardTrackSelector( );

    };

  }
} // RICH namespace

#endif // RICHRECTOOLS_RichForwardTrackSelector_H
