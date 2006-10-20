// $Id: L0MuonProcessorDatasFromRaw.h,v 1.1 2006-10-20 10:37:29 jucogan Exp $
#ifndef L0UONPROCESSORDATASFROMRAW_H 
#define L0MUONPROCESSORDATASFROMRAW_H 1

// Include files
// from STL
#include <string>

// from Gaudi
// //#include "GaudiKernel/Algorithm.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/MuonTileID.h"

#include "Event/RawEvent.h"

/** @class L0MuonProcessorDatasFromRaw L0MuonProcessorDatasFromRaw.h
 *  
 *
 *  @author 
 *  @date
 */
class L0MuonProcessorDatasFromRaw : public GaudiAlgorithm {
public: 
  /// Standard constructor
  L0MuonProcessorDatasFromRaw( const std::string& name, ISvcLocator* pSvcLocator );

  ~L0MuonProcessorDatasFromRaw( ); ///< Destructor

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
};
#endif // L0MUONPROCESSORDATASFROMRAW_H
