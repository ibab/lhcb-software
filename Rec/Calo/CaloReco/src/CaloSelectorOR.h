// $Id: CaloSelectorOR.h,v 1.5 2010-03-08 01:19:40 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2008/06/30 15:36:33  odescham
// prepare for HLT processing
//
// Revision 1.3  2006/05/30 09:42:05  odescham
// first release of the CaloReco migration
//
// Revision 1.2  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.1.1.1  2002/11/13 20:46:42  ibelyaev
// new package 
//
// Revision 1.1  2002/04/27 19:21:30  ibelyaev
//  several 'technical' tools are added
//
// ============================================================================
#ifndef CALORECO_CALOSELECTOROR_H 
#define CALORECO_CALOSELECTOROR_H 1
// Include files
// from STL
#include <string>
// from GaudiAlg
#include "GaudiAlg/GaudiTool.h"
// From CaloInterfaces
#include "CaloInterfaces/ICaloClusterSelector.h"

/** @class CaloSelectorOR CaloSelectorOR.h
 *   
 *  Helper concrete tool for selection of calocluster objects 
 *  This selector selects the cluster if 
 *  at least one  of its daughter selector select it!
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   27/04/2002
 */
class CaloSelectorOR : 
  public virtual ICaloClusterSelector ,
  public          GaudiTool 
{
  /// friend factory for instantiation
  friend struct ToolFactory<CaloSelectorOR>;
public:
  /// container of types&names 
  typedef std::vector<std::string>           Names     ;
  /// container of selectors 
  typedef std::vector<ICaloClusterSelector*> Selectors ;

public:

  /** "select"/"preselect" method 
   *  @see ICaloClusterSelector
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select     
  ( const LHCb::CaloCluster* cluster ) const  ;

  /** "select"/"preselect" method (functor interface)
   *  @see ICaloClusterSelector
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator () 
    ( const LHCb::CaloCluster* cluster ) const  ;
  
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
   *  @see GaudiTool
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @param type   tool type (?)
   *  @param name   tool name 
   *  @param parent tool parent   
   */
  CaloSelectorOR
  ( const std::string& type, 
    const std::string& name,
    const IInterface* parent);
  
  /// destructor (virtual OR protected)
  virtual ~CaloSelectorOR() ; 
  
private:

  ///   default  constructor  is  private 
  CaloSelectorOR();
  ///   copy     constructor  is  private 
  CaloSelectorOR
  (const CaloSelectorOR& );
  ///   assignement operator  is  private 
  CaloSelectorOR& operator=
  (const CaloSelectorOR& );  
  
private:
  
  Names     m_selectorsTypeNames ;
  Selectors m_selectors          ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALORECO_CALOSELECTOROR_H
// ============================================================================
