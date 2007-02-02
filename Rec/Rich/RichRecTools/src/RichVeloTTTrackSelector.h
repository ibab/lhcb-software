
//-----------------------------------------------------------------------------
/** @file RichVeloTTTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::VeloTTTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichVeloTTTrackSelector.h,v 1.2 2007-02-02 10:10:42 jonrob Exp $
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
    /** @class VeloTTTrackSelector RichVeloTTTrackSelector.h
     *
     *  Extension to the RichBaseTrackSelector for VeloTT tracks
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2006-08-12
     */
    //-----------------------------------------------------------------------------

    class VeloTTTrackSelector : public BaseTrackSelector
    {

    public: // Gaudi methods

      /// Standard constructor
      VeloTTTrackSelector( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

      /// Destructor
      virtual ~VeloTTTrackSelector( );

    };

  }
} // RICH namespace

#endif // RICHRECTOOLS_RichVeloTTTrackSelector_H
