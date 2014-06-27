// $Id: SubClusterSelectorBase.h,v 1.5 2010-03-08 01:19:40 odescham Exp $

#ifndef CALORECO_SUBCLUSTERSELECTORBASE_H 
#define CALORECO_SUBCLUSTERSELECTORBASE_H 1
// Include files
#include "CaloInterfaces/ICaloSubClusterTag.h"
#include "Event/CaloDigitStatus.h"
#include "GaudiAlg/GaudiTool.h"
class    CaloCluster   ; 


class SubClusterSelectorBase : public virtual  ICaloSubClusterTag , public GaudiTool{
  
public:

  static const LHCb::CaloDigitStatus::Status defaultStatus =                                   
    LHCb::CaloDigitStatus::UseForEnergy |LHCb::CaloDigitStatus::UseForPosition | LHCb::CaloDigitStatus::UseForCovariance ;

  
  virtual StatusCode initialize ();
  virtual StatusCode finalize   ();
  virtual StatusCode process(LHCb::CaloCluster* cluster)const;  
  virtual StatusCode operator()(LHCb::CaloCluster* cluster)const;
  virtual void setMask(const LHCb::CaloDigitStatus::Status mask)const{
    m_mask=mask;
    debug() << "The default status tag is changed to " << m_mask 
              << " -> use for Energy   : " << ((mask & LHCb::CaloDigitStatus::UseForEnergy) != 0) 
              << " | for Position : " << ((mask & LHCb::CaloDigitStatus::UseForPosition) != 0) 
              << " | for Covariance : " << ((mask & LHCb::CaloDigitStatus::UseForCovariance) != 0)
              << endmsg;
  }
  virtual unsigned int mask()const {return m_mask;};
  
    
    
protected: 

  
  /**  return  flag to modify the fractions 
   *   @return flag to modify the fractions 
   */
  inline bool modify() const { return m_modify ; }
  
  /** set new value for "modify" parameter
   *  @param value new value of modify parameter 
   */
  inline void setModify( const bool value ) const { m_modify = value ; }

  inline const DeCalorimeter* det()  {return m_det; }


  protected:
  
  /** Standard Tool Constructor
   *  @param type type of the tool (useless ? )
   *  @param name name of the tool 
   *  @param parent the tool parent 
   */
  SubClusterSelectorBase( const std::string& type   , 
                          const std::string& name   ,
                          const IInterface*  parent );
  
  virtual ~SubClusterSelectorBase( ); ///< Destructor
  
private:
  
  /// default constructor is private 
  SubClusterSelectorBase();
  /// copy    constructor is private 
  SubClusterSelectorBase( const SubClusterSelectorBase& );
  /// assignement operator is private 
  SubClusterSelectorBase& operator=( const SubClusterSelectorBase& );  
  
private:
  mutable LHCb::CaloDigitStatus::Status m_mask;
  mutable bool m_modify ;
  std::string m_detData;
  const DeCalorimeter* m_det;
  
};
// ============================================================================
#endif // SUBCLUSTERSELECTORBASE_H
