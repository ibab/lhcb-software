// $Id: createODIN.h,v 1.1 2008-01-15 10:11:23 cattanem Exp $
#ifndef CREATEODIN_H 
#define CREATEODIN_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IEventTimeDecoder;

/** @class createODIN createODIN.h
 *  Trivial algorithm to create DAQ/ODIN object from ODIN RawEvent bank
 *
 *  @author Marco Cattaneo
 *  @date   2008-01-15
 */
class createODIN : public GaudiAlgorithm {
public: 
  /// Standard constructor
  createODIN( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~createODIN( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  IEventTimeDecoder* m_decoder; ///< Pointer to tool to decode ODIN bank
};
#endif // CREATEODIN_H
