// $Id: IRichStatusCreator.h,v 1.2 2004-02-02 14:23:03 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHSTATUSCREATOR_H
#define RICHRECTOOLS_IRICHSTATUSCREATOR_H 1

// Event
#include "Event/RichRecStatus.h"
#include "Event/RichRecPixel.h"

/** @class IRichStatusCreator IRichStatusCreator.h
 *
 *  Interface for tool for the creation and book-keeping of the RichRecStatus object
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichStatusCreator( "IRichStatusCreator" , 1 , 0 );

class IRichStatusCreator : public virtual IAlgTool {

public:

  /// Returns the unique interface identifier
  static const InterfaceID& interfaceID() {return IID_IRichStatusCreator;}

  /// Returns a pointer to the RichRecStatus
  virtual RichRecStatus * richStatus() const = 0;

};

#endif // RICHRECTOOLS_IRICHSTATUSCREATOR_H
