// $Id: MaterialBudgetAlg.h,v 1.1 2002-05-04 13:09:14 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef DETDESC_MaterialBudgetALG_H 
#define DETDESC_MaterialBudgetALG_H 1
// Include files
// from STL
#include <string>
// CLHEP
#include "CLHEP/Geometry/Point3D.h"
// from Gaudi
#include "GaudiKernel/Algorithm.h"
// from AIDA 
class IHistogram2D;

/** @class MaterialBudgetAlg MaterialBudgetAlg.h
 *  
 *  Simple algorithm, which illustartes the usage of 
 *  Transport Service ("GEANE for LHCb") and produce 
 *  2D-plot of material budget at certain Z-position
 *  by "shooting" pseudo-particles from the vertex.
 *  2D-Plot need to be normalized properly!
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   23/04/2002
 */

class MaterialBudgetAlg : 
  public Algorithm 
{
  /// friend factory for instantiation  
  friend class AlgFactory<MaterialBudgetAlg>;
  
public:

  /** standard initialization of the algorithm
   *  @see  Algorithm
   *  @see IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize () ;

  /** standard execution of the algorithm
   *  @see  Algorithm
   *  @see IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute    () ;

  /** standard finalization of the algorithm
   *  @see  Algorithm
   *  @see IAlgorithm
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** Standard constructor
   *  @param name name of the algorithm
   *  @param svc service locator 
   */
  MaterialBudgetAlg 
  ( const std::string& name , 
    ISvcLocator*       svc  ) ;
  
  /// destructor (virtual and protected)
  virtual ~MaterialBudgetAlg() ;

private:

  /// default constructor    is private 
  MaterialBudgetAlg() ;
  ///  copy  constructor     is  private 
  MaterialBudgetAlg
  ( const MaterialBudgetAlg& );
  ///  assignement operator  is  private 
  MaterialBudgetAlg* operator=
  ( const MaterialBudgetAlg& );
  
private:
  
  // transport service 
  std::string         m_trSvcName     ;
  ITransportSvc*      m_trSvc         ;
  // random numbers 
  std::string         m_rndmSvcName   ;  
  IRndmGenSvc*        m_rndmSvc       ;  
  // point of shooting 
  std::vector<double> m_vrtx          ;
  HepPoint3D          m_vertex        ;
  // number of shoots per event
  int                 m_shoots        ;
  // z-position of reference plane 
  double              m_z             ;
  // x- and y- size of shooting area
  double              m_xsize         ;
  double              m_ysize         ;
  // material budget histogram itself 
  IHistogram2D*       m_budget        ;
  // Normalization histogram  
  IHistogram2D*       m_normalization ;
  
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif // DETDESC_MaterialBudgetALG_H
// ============================================================================
