
//-----------------------------------------------------------------------------
/** @file RichKsTrackSelector.h
 *
 *  Header file for RICH reconstruction tool : Rich::Rec::KsTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichKsTrackSelector.h,v 1.3 2007-03-10 13:19:20 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichKsTrackSelector_H
#define RICHRECTOOLS_RichKsTrackSelector_H 1

// base class
#include "RichBaseTrackSelector.h"

namespace Rich
{
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
