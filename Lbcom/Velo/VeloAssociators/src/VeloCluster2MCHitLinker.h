// $Id: VeloCluster2MCHitLinker.h,v 1.1 2005-03-22 07:05:30 cattanem Exp $
#ifndef VELOCLUSTER2MCHITLINKER_H 
#define VELOCLUSTER2MCHITLINKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class VeloCluster2MCHitLinker VeloCluster2MCHitLinker.h
 *  Algorithm to create Linker table for VeloCluster to MCVeloHit association 
 *  Based on equivalent relations tool VeloCluster2MCHitAsct
 *
 *  @author Marco Cattaneo
 *  @date   2005-03-21
 */
class VeloCluster2MCHitLinker : public GaudiAlgorithm {
public: 
  /// Standard constructor
  VeloCluster2MCHitLinker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloCluster2MCHitLinker( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

};
#endif // VELOCLUSTER2MCHITLINKER_H
