// $Id: MaterialBudgetAlg.h,v 1.3 2002-05-13 11:35:20 ibelyaev Exp $
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
 *  Simple algorithm, which illustrates the usage of 
 *  Transport Service ("GEANE for LHCb") and produce 
 *  2D-plot of material budget at certain Z-position
 *  by "shooting" pseudo-particles from the vertex
 *
 *  2D-plot need to be normalized properly!
 *
 *  The ratio of histogram #1 to normalization 
 *  histogram #2 provides the proper material budget profile.  
 *
 *  @code 
 *  // instantiate material budget algorithm
 *  ApplicationMgr.TopAlg += { "MaterialBudgetAlg/MaterialBudget" };
 *  @endcode 
 *
 *  The algorithm has following properties:
 *   
 *  - @p Shoots   : number of 'shoots' per event     ( defalt is @p 1000 )
 *  @code 
 *  // number of 'shoots' per event 
 *  MaterialBudget.Shoots = 1000 ;
 *  @endcode 
 *
 *  - @p ShotingPoint : position of shooting vertex  ( defaul is @p (0,0,0) )
 *  @code 
 *  // position of shooting vertex [mm]    x     y     z 
 *  MaterialBudget.ShootingPoint      =  { 0.0 , 0.0 , 0.0 };
 *  @endcode 
 * 
 *  - @p zPlane   : z-position of reference plane    ( defualt is @p 12m )
 *  @code 
 *  // z-position of reference plane [mm] 
 *  MaterialBudget.zPlane = 12000 ;
 *  @endcode 
 *
 *  - @p xMax     :  max x of shooting area at reference plane 
 *                                                   ( defualt is @p  4m )
 *  @code 
 *  // maximal x-value of shooting area at reference plane [mm] 
 *  MaterialBudget.xMax = 4000 ;
 *  @endcode 
 *
 *  - @p yMax     :  max y  of shooting area at reference plane 
 *                                                   ( defualt is @p  4m )
 *  @code 
 *  // maximal y-value of shooting area at reference plane [mm] 
 *  MaterialBudget.yMax = 3000 ;
 *  @endcode 
 *
 *  - @p xMin     :  min x of shooting area at reference plane 
 *                                                   ( defualt is @p  0m )
 *  @code 
 *  // minimal x-value of shooting area at reference plane [mm] 
 *  MaterialBudget.xMin = -4000 ;
 *  @endcode 
 *
 *  - @p yMin     :  min y  of shooting area at reference plane 
 *                                                   ( defualt is @p  0m )
 *  @code 
 *  // minimal y-value of shooting area at reference plane [mm] 
 *  MaterialBudget.yMin = -3000 ;
 *  @endcode 
 *
 *  - @p nBx      : number of bins in x-direction    ( default is @p  50 )
 *  @code 
 *  // number of x-bins  
 *  MaterialBudget.nBx = 50 ;
 *  @endcode 
 *
 *  - @p nBy      : number of bins in y-direction    ( default is @p  50 )
 *  @code 
 *  // number of x-bins  
 *  MaterialBudget.nBy = 50 ;
 *  @endcode 
 *
 *  - @p TransportService : name of Transport Service 
 *                                        ( default is @p TransportSvc )
 *  @code 
 *  // name of Transport Service 
 *  MaterialBudget.TransportService = "TransportSvc" ;
 *  @endcode 
 *
 *  - @p RndmService      : name of Random Numbers Sevice 
 *                                        ( default is @p RndmGenSvc   )
 *  @code 
 *  // name of Random Numbers Service 
 *  MaterialBudget.RndmService = "RndmGenSvc" ;
 *  @endcode 
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
  // x- and y- size of shooting area (parameters of 2d-histograms)
  double              m_xMax          ;
  double              m_yMax          ;
  double              m_xMin          ;
  double              m_yMin          ;
  int                 m_nbx           ;
  int                 m_nby           ;
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
