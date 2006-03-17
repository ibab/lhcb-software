// $Id: VeloCluster2MCHitLinker.h,v 1.3 2006-03-17 16:09:36 szumlat Exp $
#ifndef VELOCLUSTER2MCHITLINKER_H 
#define VELOCLUSTER2MCHITLINKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class VeloCluster2MCHitLinker VeloCluster2MCHitLinker.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-03-15
 */

class VeloCluster2MCHitLinker : public GaudiAlgorithm {
public: 
  /// Standard constructor
  VeloCluster2MCHitLinker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloCluster2MCHitLinker( ); ///< Destructor
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

  std::string asctName();
  
private:

  std::string m_asctName;
  std::string m_inputClusters;
  std::string m_inputFEs;

};
#endif // VELOCLUSTER2MCHITLINKER_H
//=========================================================================
inline std::string VeloCluster2MCHitLinker::asctName()
{
  return ( m_asctName );
}
