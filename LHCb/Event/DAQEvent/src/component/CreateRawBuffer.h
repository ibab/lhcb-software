// $Id: CreateRawBuffer.h,v 1.1 2004-06-28 11:41:03 cattanem Exp $
#ifndef CREATERAWBUFFER_H 
#define CREATERAWBUFFER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CreateRawBuffer CreateRawBuffer.h
 *  Little algorithm to create an empty RawBuffer
 *
 *  @author Marco Cattaneo
 *  @date   2004-06-28
 */
class CreateRawBuffer : public GaudiAlgorithm {
public:
  /// Standard constructor
  CreateRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CreateRawBuffer( ) {}; ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

};
#endif // CREATERAWBUFFER_H
