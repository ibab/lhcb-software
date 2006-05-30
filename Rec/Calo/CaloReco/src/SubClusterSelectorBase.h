// $Id: SubClusterSelectorBase.h,v 1.3 2006-05-30 09:42:07 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2005/11/07 12:12:44  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.1.1.1  2002/11/13 20:46:43  ibelyaev
// new package 
//
// Revision 1.6  2002/04/07 18:15:03  ibelyaev
//  preliminary version ('omega'-release)
//
// Revision 1.5  2001/12/09 14:33:09  ibelyaev
//  update for newer version of Gaudi
//
// Revision 1.4  2001/11/25 15:50:41  ibelyaev
//  update for newer CaloKernel package
//
// Revision 1.3  2001/11/12 19:04:28  ibelyaev
//  update
//
// Revision 1.2  2001/11/08 20:07:05  ibelyaev
//  new tools are added into  the package
//
// Revision 1.1  2001/11/08 10:58:35  ibelyaev
//  new tools are added for selection of subclusters within the cluster
// 
// ============================================================================
#ifndef CALORECO_SUBCLUSTERSELECTORBASE_H 
#define CALORECO_SUBCLUSTERSELECTORBASE_H 1
// Include files
// from STL
// from CaloInterfaces 
#include "CaloInterfaces/ICaloSubClusterTag.h"
//
#include "GaudiAlg/GaudiTool.h"
// forwarde declaration 
class    CaloCluster   ; ///< from CaloEvent package

/** @class SubClusterSelectorBase SubClusterSelectorBase.h
 *  
 *  The common base clas for different "sub-cluster"
 *   selectors 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   07/11/2001
 */

class SubClusterSelectorBase : 
  public virtual  ICaloSubClusterTag ,
  public                  GaudiTool 
{
  
public:
  
  /** standard initialization method 
   *  @see GaudiTool
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode initialize ();
  
  /** standard finalization method 
   *  @see GaudiTool
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode finalize   ();
  
  /** The main processing method 
   *  @see ICaloClusterTool 
   *  @see ICaloSubClusterTag 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( LHCb::CaloCluster* cluster ) const  ;
  
  /** The main processing method (functor interface) 
   *  @see ICaloClusterTool 
   *  @see ICaloSubClusterTag 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( LHCb::CaloCluster* cluster ) const  ;
  
    
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
  
  mutable bool m_modify ;
  std::string m_detData;
  const DeCalorimeter* m_det;
};
// ============================================================================
#endif // SUBCLUSTERSELECTORBASE_H
