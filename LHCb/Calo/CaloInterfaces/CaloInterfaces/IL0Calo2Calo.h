#ifndef CALOINTERFACES_IL0CALO2CALO_H 
#define CALOINTERFACES_IL0CALO2CALO_H 1

// Include files
// from STL
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCb
#include "Event/CaloCluster.h"
#include "Event/L0CaloCandidate.h"

static const InterfaceID IID_IL0Calo2Calo ( "IL0Calo2Calo", 1, 0 );

/** @class IL0Calo2Calo IL0Calo2Calo.h CaloInterfaces/IL0Calo2Calo.h
 *  
 * Tool to get a list of CaloClusters (owned by TES) in the vicinity of the input L0CaloCandidate(s),
 * if necessary invoking decoding and clusterization.
 *
 *  @author Dmitry Golubkov
 *  @date   2009-07-27
 */
class IL0Calo2Calo : virtual public IAlgTool 
{
public:
  // ==========================================================================
  /** obtain CaloClusters corresponding to one L0CaloCandidate
   *
   * Get a list of CaloClusters in the vicinity of the L0CaloCandidate,
   * if necessary invoke decoding and clusterization.
   * 
   * The obtained clusters are owned by TES.
   *
   * @param clusters (OUTPUT) vector of pointers of Calo clusters 
   * @param candidate(INPUT)  pointer to L0CaloCandidate
   * @param level    (INPUT)  number of neigbour levels around the cell for the ICaloClusterization tool
   */
  virtual StatusCode clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters , 
    const LHCb::L0CaloCandidate*          candidate, 
    const unsigned int                    level     ) const = 0;
  // ==========================================================================
  /** obtain CaloClusters corresponding to one L0CaloCandidate
   *
   * Get a list of CaloClusters in the vicinity of the L0CaloCandidate,
   * if necessary invoke decoding and clusterization.
   *
   * @param clusters (OUTPUT) vector of pointers of Calo clusters 
   * @param candidate(INPUT)  pointer to L0CaloCandidate
   */
  virtual StatusCode clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters, 
    const LHCb::L0CaloCandidate*          candidate ) const = 0;
  // ==========================================================================
  /** obtain CaloClusters corresponding to vector L0CaloCandidates
   *
   * Get a list of CaloClusters in the vicinity of a vector of L0CaloCandidates,
   * invoke decoding and clusterization if necessary.
   *
   * @param clusters  (OUTPUT) vector of pointers of Calo clusters 
   * @param candidates(INPUT)  container of pointers to L0CaloCandidate
   * @param level     (INPUT)  number of neigbour levels around the cell for the ICaloClusterization tool
   */
  virtual StatusCode clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters , 
    const LHCb::L0CaloCandidates*         candidate, 
    const unsigned int                    level     ) const = 0;
  // ==========================================================================
  /** obtain CaloClusters corresponding to vector of L0CaloCandidates
   *
   * Get a list of CaloClusters in the vicinity of a vector of L0CaloCandidates,
   * invoke decoding and clusterization if necessary.
   *
   * @param clusters  (OUTPUT) vector of pointers of Calo clusters 
   * @param candidates(INPUT)  container of pointers to L0CaloCandidate
   */
  virtual StatusCode clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters, 
    const LHCb::L0CaloCandidates*         candidate ) const = 0;
  // ==========================================================================
  /** static interface identification
   *
   * \return the interface ID
   */
  static const InterfaceID& interfaceID() { return IID_IL0Calo2Calo; }
};
#endif // CALOINTERFACES_IL0CALO2CALO_H
