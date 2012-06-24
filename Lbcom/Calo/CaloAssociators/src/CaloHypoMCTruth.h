#ifndef CALOHYPOMCTRUTH_H 
#define CALOHYPOMCTRUTH_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "CaloUtils/Calo2MC.h"


/** @class CaloHypoMCTruth CaloHypoMCTruth.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2012-06-24
 */
class CaloHypoMCTruth : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloHypoMCTruth( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloHypoMCTruth( ); ///< Destructor
  virtual StatusCode execute   ();    ///< Algorithm execution
protected:

private:
  std::vector<std::string>  m_inputs    ; ///< input data
  std::string m_cluster2MCLoc;
};
#endif // CALOHYPOMCTRUTH_H
