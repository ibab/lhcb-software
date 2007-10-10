// $Id: StreamForGenerator.h,v 1.1 2007-10-10 20:06:47 robbep Exp $
#ifndef GENERATORS_STREAMFORGENERATOR_H 
#define GENERATORS_STREAMFORGENERATOR_H 1

// Include files
#include "GaudiKernel/MsgStream.h"

/** @class StreamForGenerator StreamForGenerator.h Generators/StreamForGenerator.h
 *  Simple class to interface with Gaudi Output MsgStream
 *
 *  @author Patrick Robbe
 *  @date   2007-10-10
 */
class StreamForGenerator {
public:

  static MsgStream *& getStream() { return s_gaudiStream ; }

protected:

private:
  static MsgStream * s_gaudiStream ;
};
#endif 
