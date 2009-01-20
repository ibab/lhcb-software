// $Id: StreamForGenerator.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
// Include files

// local
#include "Generators/StreamForGenerator.h"

MsgStream * StreamForGenerator::s_gaudiStream = 0 ;

MsgStream *& StreamForGenerator::getStream() {
  return s_gaudiStream ;
}
