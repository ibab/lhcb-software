// $Id: CaloClusterizationTool.h,v 1.1 2008-04-21 13:53:18 vegorych Exp $
#ifndef CALOCLUSTERIZATIONTOOL_H 
#define CALOCLUSRERIZATIONTOOL_H 1
// ============================================================================
// STD and STL 
// ============================================================================
#include <string>
#include <iostream>
// ============================================================================
/// GaudiKernel
// ============================================================================
#include  "GaudiKernel/ToolFactory.h"
// ============================================================================
// STD and STL 
// ============================================================================
#include <string>
#include <iostream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// CaloKernel 
// ============================================================================
#include "CaloKernel/CaloVector.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloClusterization.h"
// ============================================================================
// CaloDet 
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// local
// ============================================================================
#include "CelAutoTaggedCell.h"
// ============================================================================


/** @class CellAutomaton CellAutomaton.h
 *  
 *
 *  @author Victor Egorychev
 *  @date   2008-04-03
 */
class CaloClusterizationTool 
  : virtual public ICaloClusterization
  ,  public GaudiTool {

public:
  friend class ToolFactory<CaloClusterizationTool>;
  
  /// container to tagged  cells with sequential access 
  typedef std::vector<CelAutoTaggedCell*> SeqVector;
  /// container to tagged  cells with direct (by CaloCellID key)  access 
  typedef CaloVector<CelAutoTaggedCell*>  DirVector ;

  // Return the interface ID
  //  static const InterfaceID& interfaceID();
  static const InterfaceID& interfaceID() { return IID_ICaloClusterization; }

  /// Standard constructor
  CaloClusterizationTool( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  
  virtual StatusCode clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters   , 
    const LHCb::CaloDigits*               hits       , 
    const DeCalorimeter*                  detector   ,
    const std::vector<LHCb::CaloCellID>&  seeds      , 
    const unsigned int                    level      )  ;
  // ==========================================================================
  virtual StatusCode clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters   , 
    const LHCb::CaloDigits*               hits       , 
    const DeCalorimeter*                  detector   , 
    const LHCb::CaloCellID&               seed       , 
    const unsigned int                    level      )  
  {
    return clusterize ( clusters , hits , detector , 
                        std::vector<LHCb::CaloCellID>(1,seed) , level ) ; 
  };
  // ==========================================================================
  virtual StatusCode clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters   , 
    const LHCb::CaloDigits*               hits       , 
    const DeCalorimeter*                  detector   )
  {
    const unsigned int level = 0;
    std::vector<LHCb::CaloCellID>  seeds;
    seeds.clear();
    return clusterize ( clusters , hits , detector, seeds, level) ; 
  } ;

  virtual StatusCode initialize() ; 

  virtual ~CaloClusterizationTool( ); ///< Destructor

protected:

  inline bool isLocMax
  ( const LHCb::CaloDigit*     digit ,
    const DirVector&     hits  ,
    const DeCalorimeter* det ) ;
  
  inline void appliRulesTagger
  ( CelAutoTaggedCell*   taggedCell,
    DirVector&           taggedCellsDirect,
    const DeCalorimeter* detector );
  
  inline void setEXYCluster
  ( LHCb::CaloCluster*         cluster,
    const DeCalorimeter* detector );

  void look_neig(std::set<LHCb::CaloCellID> in_cells ,
                 const DeCalorimeter* m_detector,
                 std::set<LHCb::CaloCellID>& out_cells);

private:

  bool m_withET ;
  double m_ETcut;
};
#endif // CALOCLUSTERIZATIONTOOL_H
