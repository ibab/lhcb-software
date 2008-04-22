// $Id: ICaloClusterization.h,v 1.2 2008-04-22 09:46:51 cattanem Exp $
#ifndef CALOINTERFACES_ICALOCLUSTERIZATION_H 
#define CALOINTERFACES_ICALOCLUSTERIZATION_H 1

// Include files
// from STL
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCb
#include "Kernel/CaloCellID.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"

// forward declarations
class DeCalorimeter;

static const InterfaceID IID_ICaloClusterization ( "ICaloClusterization", 1, 0 );

/** @class ICaloClusterization ICaloClusterization.h CaloInterfaces/ICaloClusterization.h
 *  
 *
 *  @author Victor Egorychev
 *  @date   2008-04-03
 */
class ICaloClusterization : virtual public IAlgTool 
{
public:
  // ==========================================================================
  /**  The main clusterization method
   * @ param pointer to CaloDigit
   * @ param pointer to DeCalorimeter object
   * @ param vector of Calo cell ID's
   * @ param number of neigbour levels around cell
   * @ return vector of pointers of Calo clusters
   */
  virtual StatusCode clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters   , 
    const LHCb::CaloDigits*               digits     , 
    const DeCalorimeter*                  detector   , 
    const std::vector<LHCb::CaloCellID>&  seeds      , 
    const unsigned int                    level      ) = 0 ;
  // ==========================================================================
  /**  The main clusterization method
   * @ param pointer to CaloDigit
   * @ param pointer to DeCalorimeter object
   * @ param one Calo cell ID
   * @ param number of neigbour levels around cell
   * @ return vector of pointers of Calo clusters
   */
  virtual StatusCode clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters   , 
    const LHCb::CaloDigits*               digits     , 
    const DeCalorimeter*                  detector   , 
    const LHCb::CaloCellID&               seed       , 
    const unsigned int                    level      ) = 0 ;
  // ==========================================================================
  /**  The main clusterization method
   * @ param pointer to CaloDigit
   * @ param pointer to DeCalorimeter object
   * @ return vector of pointers of Calo clusters
   */
  virtual StatusCode clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters   , 
    const LHCb::CaloDigits*               digits     , 
    const DeCalorimeter*                  detector   ) = 0 ;
  
public: 

  /** static interface identification
   *  @see IInterface
   *  @see IID_ICaloClusterization
   *  Return the interface ID
   */
  static const InterfaceID& interfaceID() { return IID_ICaloClusterization; }


protected:

private:

};
#endif // CALOINTERFACES_ICALOCLUSTERIZATION_H
