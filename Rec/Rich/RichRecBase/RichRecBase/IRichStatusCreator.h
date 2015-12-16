
//-----------------------------------------------------------------------------
/** @file IRichStatusCreator.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IStatusCreator
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHSTATUSCREATOR_H
#define RICHRECTOOLS_IRICHSTATUSCREATOR_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/RichRecPixel.h"

/// Static Interface Identification
static const InterfaceID IID_IRichStatusCreator( "Rich::Rec::IStatusCreator" , 1 , 0 );

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class IStatusCreator IRichStatusCreator.h
     *
     *  Interface for tool for the creation and book-keeping of the RichRecStatus object
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class IStatusCreator : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichStatusCreator; }

      /** Access to the RichRecStatus object
       *
       *  @return Pointer to the RichRecStatus object
       */
      virtual LHCb::RichRecStatus * richStatus() const = 0;

    };

  }
}

#endif // RICHRECTOOLS_IRICHSTATUSCREATOR_H
