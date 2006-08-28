
//-----------------------------------------------------------------------------
/** @file RichForwardTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : RichForwardTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichForwardTrackSelector.h,v 1.1 2006-08-28 11:34:41 jonrob Exp $
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
  /** @class RichForwardTrackSelector RichForwardTrackSelector.h
   *
   *  Extension to the RichBaseTrackSelector for Forward tracks
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-08-12
   */
  //-----------------------------------------------------------------------------

  class RichForwardTrackSelector : public RichBaseTrackSelector
  {

  public: // Gaudi methods

    /// Standard constructor
    RichForwardTrackSelector( const std::string& type,
                            const std::string& name,
                            const IInterface* parent );

    /// Destructor
    virtual ~RichForwardTrackSelector( );

  };

} // RICH namespace

#endif // RICHRECTOOLS_RichForwardTrackSelector_H
