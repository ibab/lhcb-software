#ifndef VELOCLUSTER2MCHITALG_H
#define VELOCLUSTER2MCHITALG_H 1

#include <string>
#include <map>

#include "GaudiKernel/Algorithm.h"
#include "VeloAssociators/VeloCluster2MCHitAsct.h"

/** @class VeloCluster2MCHitAlg VeloCluster2MCHitAlg.h
 *  
 *
 *  @author Chris Parkes
 *  @date   21/05/2002
 */
class VeloCluster2MCHitAlg : public Algorithm {

  friend class AlgFactory<VeloCluster2MCHitAlg>;
  
public:

  /// Standard constructor
  VeloCluster2MCHitAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~VeloCluster2MCHitAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// finalize
  virtual StatusCode finalize();    

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  std::string m_outputData;

};

inline std::string VeloCluster2MCHitAlg::outputData() const {
  return  m_outputData;
}


#endif // VELOCLUSTER2MCHITALG_H







