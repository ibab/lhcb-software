// $Id: CreateL1Buffer.h,v 1.1 2004-06-28 11:41:03 cattanem Exp $
#ifndef CREATEL1BUFFER_H 
#define CREATEL1BUFFER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CreateL1Buffer CreateL1Buffer.h
 *  Little algorithm to create an empty L1Buffer
 *
 *  @author Marco Cattaneo
 *  @date   2004-06-28
 */
class CreateL1Buffer : public GaudiAlgorithm {
public:
  /// Standard constructor
  CreateL1Buffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CreateL1Buffer( ) {}; ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

};
#endif // CREATEL1BUFFER_H
