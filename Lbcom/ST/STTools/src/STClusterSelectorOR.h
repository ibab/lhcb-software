// $Id: STClusterSelectorOR.h,v 1.1 2009-02-10 09:18:38 mneedham Exp $
#ifndef STTTOOLS_STCLUSTERSELECTOR_H 
#define STTTOOLS_STCLUSTERSELECTOR_H 1
// Include files
// from STL
#include <string>
// from GaudiAlg
#include "GaudiAlg/GaudiTool.h"
// From CaloInterfaces
#include "Kernel/ISTClusterSelector.h"

/** @class CaloSelectorOR CaloSelectorOR.h
 *   
 *  Helper concrete tool for selection of STCluster objects 
 *  This selector selects the cluster if 
 *  at least one  of its daughter selector select it!
 *
 *  @author M Needham
 *  @date   03/02/2009
 */
class STClusterSelectorOR : 
  public virtual ISTClusterSelector ,
  public          GaudiTool 
{
  /// friend factory for instantiation
  friend class ToolFactory<STClusterSelectorOR>;
public:
  /// container of types&names 
  typedef std::vector<std::string> Names     ;
  /// container of selectors 
  typedef std::vector<ISTClusterSelector*> Selectors ;

public:

  /** "select"/"preselect" method 
   *  @see ISTClusterSelector
   *  @param  cluster pointer to ST cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select     
  ( const LHCb::STCluster* cluster ) const  ;

  /** "select"/"preselect" method (functor interface)
   *  @see ISTClusterSelector
   *  @param  cluster pointer to ST cluster object to be selected 
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
  
  /** Standard constructor
   *  @see GaudiTool
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @param type   tool type (?)
   *  @param name   tool name 
   *  @param parent tool parent   
   */
  STClusterSelectorOR
  ( const std::string& type, 
    const std::string& name,
    const IInterface* parent);
  
  /// destructor (virtual OR protected)
  virtual ~STClusterSelectorOR() ; 
  
private:

  ///   default  constructor  is  private 
  STClusterSelectorOR();
  ///   copy     constructor  is  private 
  STClusterSelectorOR
  (const STClusterSelectorOR& );
  ///   assignement operator  is  private 
  STClusterSelectorOR& operator=
  (const STClusterSelectorOR& );  
  
private:
  
  Names     m_selectorsTypeNames ;
  Selectors m_selectors          ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // STTTOOLS_STCLUSTERSELECTOROR_H
// ============================================================================
