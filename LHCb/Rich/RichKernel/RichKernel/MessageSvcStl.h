// $Id: MessageSvcStl.h,v 1.1 2003-06-30 14:17:02 jonrob Exp $
#ifndef RICHKERNEL_MESSAGESVCSTL_H 
#define RICHKERNEL_MESSAGESVCSTL_H 1

#include "GaudiKernel/MsgStream.h"

/// Method to send std::vector to Gaudi Message Service
/// SHould be moved to standard Gaudi Location ?
template <class T> inline
MsgStream & operator << ( MsgStream & stream, 
                          const std::vector<T> & vector ) {
  for ( typename std::vector<T>::const_iterator i = vector.begin(); 
        i != vector.end(); ++i ) { stream << *i << " "; }
  return stream;
}

#endif // RICHKERNEL_MESSAGESVCSTL_H
