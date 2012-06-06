#ifndef VELOSTRIPCLUSTER2MCHITLINKER_H 
#define VELOSTRIPCLUSTER2MCHITLINKER_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

/** @class VeloStripCluster2MCHitLinker VeloStripCluster2MCHitLinker.h
 *  
 */

class VeloStripCluster2MCHitLinker : public GaudiAlgorithm {
public: 
  /// Constructor
  VeloStripCluster2MCHitLinker(const std::string& name, 
                               ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VeloStripCluster2MCHitLinker() {}

  virtual StatusCode execute();    ///< Algorithm execution

protected:

  std::string asctName() {return m_asctName;}
  
private:

  std::string m_asctName;
  std::string m_inputClusters;
  std::string m_inputDigits;

};
#endif 
