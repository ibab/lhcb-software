// $Id: CreateRawEvent.h,v 1.1 2004-06-28 11:41:03 cattanem Exp $
#ifndef CREATERAWEVENT_H 
#define CREATERAWEVENT_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CreateRawEvent CreateRawEvent.h
 *  Little algorithm to create the RawEvent from the RawBuffer
 *  Must be called after all modifications to RawBuffer have been made
 *
 *  @author Marco Cattaneo
 *  @date   2004-06-28
 */
class CreateRawEvent : public GaudiAlgorithm {
public:
  /// Standard constructor
  CreateRawEvent( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CreateRawEvent( ) {}; ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

};
#endif // CREATERAWEVENT_H
