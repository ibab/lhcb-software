// $Id: CaloClusterSOMSelector.h,v 1.1 2002-06-14 11:38:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOEX_CALOClusterSOMSelector_H 
#define CALOEX_CALOClusterSOMSelector_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// CaloInterfaces
#include "CaloInterfaces/ICaloClusterSelector.h"

/** @class CaloClusterSOMSelector CaloClusterSOMSelector.h
 *
 *  Simple "Photon/Pi0 Selector" bases on analysis of 
 *  correlation of Cluster Second Order moments ("spread") 
 *  versus Cluster Energy 
 *  
 *  The Selector could be used to select "clean"/"isolated" 
 *  electromagnetic showers or to select merged pi0s 
 *
 *  The selector is based on Frederic Machefert's machefer@in2p3.fr 
 *  ideas and plots:  
 *  http://documents.cern.ch/age?a02129
 *
 *  We assume that isolated photons could be selected using 
 *  linear parametrisation into 2 separate regions in energy.
 *  Thus one needs values of SOM for 3 points in energy
 *  to make the parametrisation.
 *  And this parametrisation is valid only for certain 
 *  area in calorimeter. therefore in total one needs 
 *  (3+3)*nArea = (3+3)*3 = 6 * 3 = 18 constants 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/05/2002
 */

class CaloClusterSOMSelector : 
  public virtual ICaloClusterSelector ,
  public          CaloTool  
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloClusterSOMSelector>;
  
public:
  
  /** Main "select"/"preselect" method 
   *  @see ICaloClusterSelector 
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select     ( const CaloCluster* cluster ) const ;
  
  /** Main "select"/"preselect" method (functor interface)
   *  @see ICaloClusterSelector 
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const CaloCluster* cluster ) const ;

  /** standard initialization of the Tool 
   *  @see CaloTool
   *  @see  AlgTool
   *  @see IAlgTool 
   *  @return statsu code 
   */
  virtual StatusCode initialize () ;
  
  /** standard finalization of the Tool 
   *  @see CaloTool
   *  @see  AlgTool
   *  @see IAlgTool 
   *  @return statsu code 
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** return the value of "limit" function 
   *  (second order moment versus cluster energy)
   *  It is just a parametrisation by 2 straight lines
   *  @param energy of the cluster 
   *  @param area   calorimeter area index 
   *  @return limit value for Second order moment 
   */
  inline const double limit
  ( const double       energy , 
    const unsigned int area   ) const 
  {
    // check the area 
    Assert( area < m_e0.size() , "Invalid area in the calorimeter");
    
    const double e0 = m_e0 [ area ] ;
    const double e1 = m_e1 [ area ] ;
    const double e2 = m_e2 [ area ] ;
    
    const double s0 = m_s0 [ area ] ;
    const double s1 = m_s1 [ area ] ;
    const double s2 = m_s2 [ area ] ;
    
    return  
      energy < e1                                      ? 
      s1 + ( energy - e1 ) / ( e0 - e1 ) * ( s0 - s1 ) :
      s1 + ( energy - e1 ) / ( e2 - e1 ) * ( s2 - s1 ) ;
  };
  
protected:
  
  /** Standard constructor
   *  @see   CaloTool
   *  @see    AlgTool 
   *  @param type    type of the tool  (?)
   *  @param name    name of the concrete instance 
   *  @param parent  pointer to parent object (algorithm, service or tool)
   */
  CaloClusterSOMSelector
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor (virtual and protected)
  virtual ~CaloClusterSOMSelector() ;
  
private:

  ///  default  constructor   is   private
  CaloClusterSOMSelector();
  ///  copy     constructor   is   private
  CaloClusterSOMSelector( const CaloClusterSOMSelector& );
  ///  assignement operator   is   private
  CaloClusterSOMSelector& operator=( const CaloClusterSOMSelector& );
  
private:
  
  typedef std::vector<double> Numbers;
  
  /// vectors of energy points  (index is an calo area index)
  Numbers  m_e0   ;
  Numbers  m_e1   ;
  Numbers  m_e2   ;
  
  /// vector of second order moments points (index is an calo area index)
  Numbers  m_s0   ;
  Numbers  m_s1   ;
  Numbers  m_s2   ;
  
  /** tolerance parameters 
   *  instead of cut:    s < s_m
   *  we use             s < ( s_m + a ) * b 
   */
  double   m_aTol ;
  double   m_bTol ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOClusterSOMSelector_H
// ============================================================================
