
//-----------------------------------------------------------------------------
/** @file RichKsTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::KsTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichKsTrackSelector.h,v 1.2 2007-02-02 10:10:41 jonrob Exp $
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
    /** @class KsTrackSelector RichKsTrackSelector.h
     *
     *  Extension to the RichBaseTrackSelector for Ks tracks
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2006-08-12
     */
    //-----------------------------------------------------------------------------

    class KsTrackSelector : public BaseTrackSelector
    {

    public: // Gaudi methods

      /// Standard constructor
      KsTrackSelector( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

      /// Destructor
      virtual ~KsTrackSelector( );

    };

  }
} // RICH namespace

#endif // RICHRECTOOLS_RichKsTrackSelector_H
