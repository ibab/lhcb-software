
//-----------------------------------------------------------------------------
/** @file RichVeloTTTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : RichVeloTTTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichVeloTTTrackSelector.h,v 1.1 2006-08-28 11:34:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichVeloTTTrackSelector_H
#define RICHRECTOOLS_RichVeloTTTrackSelector_H 1

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
  /** @class RichVeloTTTrackSelector RichVeloTTTrackSelector.h
   *
   *  Extension to the RichBaseTrackSelector for VeloTT tracks
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2006-08-12
   */
  //-----------------------------------------------------------------------------

  class RichVeloTTTrackSelector : public RichBaseTrackSelector
  {

  public: // Gaudi methods

    /// Standard constructor
    RichVeloTTTrackSelector( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

    /// Destructor
    virtual ~RichVeloTTTrackSelector( );

  };

} // RICH namespace

#endif // RICHRECTOOLS_RichVeloTTTrackSelector_H
