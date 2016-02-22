// $Id: STClusterSelectorAND.h,v 1.1 2009-02-10 09:18:38 mneedham Exp $
#ifndef STCLUSTER_STCLUSTERSELECTORAND_H 
#define STCLUSTER_STCLUSTERSELECTORAND_H 1
// Include files
// from STL
#include <string>
// from GaudiAlg
#include "GaudiAlg/GaudiTool.h"
// From CaloInterfaces
#include "Kernel/ISTClusterSelector.h"

/** @class STClusterSelectorAND STClusterSelectorAND.h
 *   
 *  Helper concrete tool for selection of stcluster objects
 *  This selector selects the cluster if 
 *  all of its daughter selector select it!
 *
 *  @author M Needhams
 *  @date   03/02/2009
 */
class STClusterSelectorAND : 
  public virtual ISTClusterSelector ,
  public          GaudiTool 
{
public:
  /// container of types&names 
  typedef std::vector<std::string>           Names     ;
  /// container of selectors 
  typedef std::vector<ISTClusterSelector*> Selectors ;

public:

  /** Standard constructor
   *  @see GaudiTool
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @param type   tool type (?)
   *  @param name   tool name 
   *  @param parent tool parent   
   */
  STClusterSelectorAND
  ( const std::string& type, 
    const std::string& name,
    const IInterface* parent);
  
  /** "select"/"preselect" method 
   *  @see ISTClusterSelector
   *  @param  cluster pointer to st cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select     
  ( const LHCb::STCluster* cluster ) const  ;

  /** "select"/"preselect" method (functor interface)
   *  @see ICaloClusterSelector
   *  @param  cluster pointer to st cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator () 
    ( const LHCb::STCluster* cluster ) const  ;
  
  /** standard initialization of the tool 
   *  @see IAlgTool 
   *  @see AlgTool 
   *  @see GaudiTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
protected:
  
  /// destructor (virtual and protected)
  virtual ~STClusterSelectorAND() ; 
  
private:

  ///   default  constructor  is  private 
  STClusterSelectorAND();
  ///   copy     constructor  is  private 
  STClusterSelectorAND
  (const STClusterSelectorAND& );
  ///   assignement operator  is  private 
  STClusterSelectorAND& operator=
  (const STClusterSelectorAND& );  
  
private:
  
  Names     m_selectorsTypeNames ;
  Selectors m_selectors          ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // STTTOOLS_STCLUSTERSELECTORAND_H
// ============================================================================
