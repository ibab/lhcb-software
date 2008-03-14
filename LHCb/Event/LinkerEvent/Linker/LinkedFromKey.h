// $Id: LinkedFromKey.h,v 1.1 2008-03-14 10:06:19 ocallot Exp $
#ifndef LINKER_LINKEDFROMKEY_H 
#define LINKER_LINKEDFROMKEY_H 1

// Include files
#include "Linker/LinkedFrom.h"

/** @class LinkedFromKey LinkedFromKey.h Linker/LinkedFromKey.h
 *  Simple helper class to handle LinkedFrom with no source, i.e. key linker
 *
 *  @author Olivier Callot
 *  @date   2008-03-14
 */

template <class TARGET, class KEY=int> class LinkedFromKey : public LinkedFrom<ContainedObject, TARGET, KEY> {
public:
  LinkedFromKey( IDataProviderSvc* eventSvc,
                 IMessageSvc* msgSvc,
                 std::string containerName ) 
    : LinkedFrom<ContainedObject, TARGET, KEY>( eventSvc, msgSvc, containerName ) { };
  ~LinkedFromKey() {};
};
#endif // LINKER_LINKEDFROMKEY_H
