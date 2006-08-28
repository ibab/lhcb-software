
//-----------------------------------------------------------------------------
/** @file RichKsTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : RichKsTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichKsTrackSelector.h,v 1.1 2006-08-28 11:34:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichKsTrackSelector_H
#define RICHRECTOOLS_RichKsTrackSelector_H 1

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
  /** @class RichKsTrackSelector RichKsTrackSelector.h
   *
   *  Extension to the RichBaseTrackSelector for Ks tracks
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-08-12
   */
  //-----------------------------------------------------------------------------

  class RichKsTrackSelector : public RichBaseTrackSelector
  {

  public: // Gaudi methods

    /// Standard constructor
    RichKsTrackSelector( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

    /// Destructor
    virtual ~RichKsTrackSelector( );

  };

} // RICH namespace

#endif // RICHRECTOOLS_RichKsTrackSelector_H
