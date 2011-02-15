#ifndef _ICountContainedObjects_H
#define _ICountContainedObjects_H
 
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ObjectContainerBase.h"

static const InterfaceID IID_ICountContainedObjects( "ICountContainedObjects", 0, 0 );
 
/** @class ICountContainedObjects ICountContainedObjects.h
 *
 *  Interface for counting objects in a container according to some 
 *  criteria (e.g. counting Velo tracks in the Best container)
 *
 *  @author David Hutchcroft
 *  @date   14/2/2011
 */

class ICountContainedObjects: virtual public IAlgTool{
 
public:
        
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ICountContainedObjects ; }
                                                                               
  /** Returns number of objects in the container
  *
  *  @param cont is the container (dynamic cast to the real type in the implementations)
  *
  *  @return number of objects in the container based on some criteria 
  */
  virtual unsigned int nObj ( const ObjectContainerBase * cont ) const = 0;
                                                                       
};
                                                                               
#endif //_ICountContainedObjects_H
