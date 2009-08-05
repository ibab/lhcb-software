// $Id: CaloSelectClusterWithSpd.h,v 1.1 2009-08-05 17:38:30 ibelyaev Exp $
// ============================================================================
#ifndef CALOSELECTCLUSTERWITHSPD_H 
#define CALOSELECTCLUSTERWITHSPD_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloClusterSelector.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"
// ============================================================================
/** @class CaloSelectClusterWithSpd 
 *  Helper base class for "cluster selectioin with Spd" tools 
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-07-18
 */
// ============================================================================
class CaloSelectClusterWithSpd : 
  public virtual ICaloClusterSelector ,
  public          GaudiTool
{
public:
  // ==========================================================================
  /// initialize the tool 
  virtual StatusCode initialize () ;
  // ==========================================================================
public:
  // ==========================================================================
  /// number of hits in SPD
  int n_hits ( const LHCb::CaloCluster& cluster ) const ;  
  /// number of hits in SPD
  int n_hit  ( const LHCb::CaloCluster& cluster ) const 
  { return n_hits ( cluster ) ; }
  // ==========================================================================
protected:
  // ==========================================================================
  /// standard constructor 
  CaloSelectClusterWithSpd 
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent ) ;
  /// destructor 
  virtual ~CaloSelectClusterWithSpd(); 
  // ==========================================================================
private:  
  // ==========================================================================
  /// default constructor is disabled 
  CaloSelectClusterWithSpd(); // default constructor is disabled 
  /// copy constructor is disabled 
  CaloSelectClusterWithSpd ( const CaloSelectClusterWithSpd& ); 
  /// assignemet operator is disabled 
  CaloSelectClusterWithSpd& operator=(const CaloSelectClusterWithSpd& );
  // ==========================================================================  
protected:
  // ==========================================================================
  /// get number of hits 
  int cut () const { return m_cut ; }
  /// get the tool 
  ICaloHypo2Calo* calo2calo () const { return m_toSpd ; }
  /// get the calorimeter 
  const std::string& det() const { return m_det   ; }
  // ==========================================================================  
private:
  // ==========================================================================
  /// number of hits in spd 
  int             m_cut   ;                            // number of hits in spd 
  /// Calo -> Calo tool 
  ICaloHypo2Calo* m_toSpd ;                            //     Calo -> Calo tool 
  /// Calorimeter 
  std::string     m_det   ;                            //           Calorimeter
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // CALOSELECTCLUSTERWITHSPD_H
// ============================================================================
