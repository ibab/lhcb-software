// $Id: InternalVeloCluster2MCHitLinker.h,v 1.1 2006-02-09 13:13:51 szumlat Exp $
#ifndef INTERNALVELOCLUSTER2MCHITLINKER_H 
#define INTERNALVELOCLUSTER2MCHITLINKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class InternalVeloCluster2MCHitLinker InternalVeloCluster2MCHitLinker.h
 *  algorithm creates Table with linked InternalVeloClusters (source) and
 *  MCHits (target), compatible with new LHCb v20r0
 *
 *  @author Tomasz Szumlak
 *  @date   2006-02-11
 */

class InternalVeloCluster2MCHitLinker : public GaudiAlgorithm {
public: 

  /// Standard constructor
  InternalVeloCluster2MCHitLinker(
  const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~InternalVeloCluster2MCHitLinker( ); ///< Destructor
  virtual StatusCode execute();    ///< Algorithm execution

protected:

  std::string asctName();

private:

  std::string m_asctName;
  std::string m_inputClusters;
  std::string m_inputFEs;

};
#endif // VELOCLUSTER2MCHITLINKER_H
//==========================================================================
inline std::string InternalVeloCluster2MCHitLinker::asctName()
{
  return (m_asctName);
}
