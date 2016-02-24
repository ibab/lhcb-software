// $Id: CaloSelector.h,v 1.9 2010-03-08 01:19:40 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// #Log:$
// ============================================================================
#ifndef CALORECO_CALOSELECTOR_H 
#define CALORECO_CALOSELECTOR_H 1
// Include files
// from STL
#include <string>
// from GaudiAlg
#include "GaudiAlg/GaudiTool.h"
// from CaloInterfaces 
#include "CaloInterfaces/ICaloClusterSelector.h"
// forward declaratiosn 
class ICaloLikelihood ; // from CaloInterfaces ;

/** @class CaloSelector CaloSelector.h
 *  
 *  Concrete cluster-selector tool, to select CaloClusters
 *  which satisfy the given calorimeter hypothesis 
 *  Tool in turn uses the tool with ICaloLikelihood
 *  interaface to calculate the likelihood.
 *  @see ICaloClusterSelector
 *  @see ICaloLikelyhood 
 *  @see CaloCluster 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   31/03/2002
 */
class CaloSelector : 
  public virtual   ICaloClusterSelector , 
  public                       GaudiTool 
{
  /// friend factory for instantiation`
  friend struct ToolFactory<CaloSelector>;
public:
  
  /** "select"/"preselect" method 
   *  @see ICaloClusterSelector 
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select     ( const LHCb::CaloCluster* cluster ) const ;
  
  /** "select"/"preselect" method (functor interface)
   *  @see ICaloClusterSelector 
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const LHCb::CaloCluster* cluster ) const ;
  
  /** standard initialization of the tool 
   *  @see IAlgTool 
   *  @see AlgTool 
   *  @see GaudiTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;

  /** standard finalization  of the tool 
   *  @see IAlgTool 
   *  @see AlgTool 
   *  @see GaudiTool 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;

protected: 
  
  /** Standard constructor
   *  @param    type   tool type (?)
   *  @param    name   tool name 
   *  @param    parent tool parent 
   */
  CaloSelector
  ( const std::string&  type   ,  
    const std::string&  name   ,
    const IInterface*   parent );
  
  /// destructor (virtual and protected)
  virtual ~CaloSelector(); 

private:

  /// default constructor is private
  CaloSelector();
  /// copy constructor is private 
  CaloSelector
  ( const CaloSelector& );
  /// assignement operator is private 
  CaloSelector& operator=
  ( const CaloSelector& );
  
private:
  
  // type of Likelyhood tool to be used 
  std::string      m_lhType     ; ///< type of Likelyhood tool to be used 
  // name of Likelyhood tool to be used 
  std::string      m_lhName     ; ///< name of Likelyhood tool to be used 
  //         Likelyhood tool to be used 
  ICaloLikelihood* m_likelihood ; ///<         Likelyhood tool to be used 
  // cut on likelyhood 
  double           m_cut        ; ///< value of likelihood cut

};

// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
#endif // CALORECO_CALOSELECTOR_H
// ============================================================================
