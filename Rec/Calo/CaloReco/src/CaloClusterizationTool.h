// $Id: CaloClusterizationTool.h,v 1.5 2009-11-05 22:37:14 dgolubko Exp $
#ifndef CALOCLUSTERIZATIONTOOL_H 
#define CALOCLUSRERIZATIONTOOL_H 1
// ============================================================================
#include <string>
#include <iostream>
#include <string>
#include <iostream>
#include "GaudiAlg/GaudiTool.h"
#include "CaloKernel/CaloVector.h"
#include "CaloInterfaces/ICaloClusterization.h"
#include "CaloDet/DeCalorimeter.h"
#include "CelAutoTaggedCell.h"
#include "CaloUtils/CellSelector.h"

/** @class CaloClusterizationTool CaloClusterizationTool.h
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
    const unsigned int                    level      )  
  {
    return _clusterize ( clusters , *hits , detector , seeds , level ) ;
  }

  // ==========================================================================

  virtual StatusCode clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters   , 
    const CaloVector<LHCb::CaloDigit*>&   hits       , 
    const DeCalorimeter*                  detector   ,
    const std::vector<LHCb::CaloCellID>&  seeds      , 
    const unsigned int                    level      )  
  {
    return _clusterize ( clusters , hits , detector , seeds , level ) ;
  }
  
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
  unsigned int iterations(){return m_pass;};
  virtual StatusCode initialize() ; 

  virtual ~CaloClusterizationTool( ); ///< Destructor

protected:
  
  template<class TYPE> StatusCode _clusterize
  ( std::vector<LHCb::CaloCluster*>&      clusters   , 
    const TYPE&                           data       , 
    const DeCalorimeter*                  detector   ,
    const std::vector<LHCb::CaloCellID>&  seeds      , 
    const unsigned int                    level      )  ;

protected:

  inline bool isLocMax
  ( const LHCb::CaloDigit*     digit ,
    const DirVector&     hits  ,
    const DeCalorimeter* det ) ;
  
  inline void appliRulesTagger
  ( CelAutoTaggedCell*   taggedCell,
    DirVector&           taggedCellsDirect,
    const DeCalorimeter* detector );
  
  inline StatusCode setEXYCluster( LHCb::CaloCluster*         cluster,const DeCalorimeter* detector );

private:

  bool m_withET ;
  double m_ETcut;
  bool m_release;
  std::string m_usedE;
  std::string m_usedP;
  CellSelector m_cellSelectorE;
  CellSelector m_cellSelectorP;
  unsigned int m_pass;
  int m_passMax;
};
#endif // CALOCLUSTERIZATIONTOOL_H
