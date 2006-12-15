// $Id: MaterialBudgetAlg.h,v 1.10 2006-12-15 16:49:13 cattanem Exp $
// ============================================================================
#ifndef DETDESCCHECKS_MaterialBudgetALG_H 
#define DETDESCCHECKS_MaterialBudgetALG_H 1
// Include files
// from STL
#include <string>
// Math Definitions
#include "GaudiKernel/Point3DTypes.h"
// GaudiAlg
#include "GaudiAlg/GaudiHistoAlg.h"

/** @class MaterialBudgetAlg MaterialBudgetAlg.h
 *  
 *  Simple algorithm, which perform the evaluation of the 
 *  material budget using Transport Service ("LHCb's GEANE") 
 *
 *  The algorithm produces 2 2-dimentional plots with the 
 *  evaluationfo the material budget ( in units of radiation length)
 *  between origin vertex and x-y point at the reference plane. 
 *  The plot need to be normalized properly. Normalization is given 
 *  by histogram number 2. The proper normalization is achieved e.g in PAW
 *
 *  @code 
 *
 *  PAW> hi/oper/div 1 2 10
 *  PAW> hi/plot     10     lego1 
 *
 *  @endcode 
 *
 *  The full list of algorithm properties and their default values:
 *  <ul>                   
 *  <li> @p ShootingPoint  Position of the "origin vertex" 
 *                             (default value <tt>{ 0. , 0. , 0. }</tt>)
 *  <li> @ Shots           Number of random shots per event 
 *                                           (default value is @p  1000 ) 
 *  <li> @ zPlane          @p Z -position of the reference plane
 *                                           (default value is @p 12000 )
 *  <li> @ xMax            Maximal value of @p X at reference plane 
 *                                           (default value is @p  4000 ) 
 *  <li> @ yMax            Maximal value of @p Y at reference plane 
 *                                           (default value is @p  3000 ) 
 *  <li> @ xMin            Minimal value of @p X at reference plane 
 *                                           (default value is @p     0 ) 
 *  <li> @ yMin            Minimal value of @p Y at reference plane 
 *                                           (default value is @p     0 ) 
 *  <li> @ nBx             Number of bins in @p X -direction 
 *                                           (default value is @p    50 ) 
 *  <li> @ nBy             Number of bins in @p Y -direction 
 *                                           (default value is @p    50 )
 *  <li> @ TransportSvc    The name of Gaudi Transport Service 
 *                                  (default value is @p "TransportSvc" )
 *  <li> @ RndmService     The name of Gaudi Random Numbers Service 
 *                                    (default value is @p "RndmGenSvc" )
 *  </ul>
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   23/04/2002
 *
 *  added by W. Pokorski:
 *  - Grid      flag to switch between random shooting (0) and grid (1)
 *  - xbinref   x-size of the reference bin (to be scaled as m_z/m_zref)
 *  - ybinref   y-size of the reference bin (to be scaled as m_z/m_zref)
 *  - zref      reference z position (at which xbinref and ybinref  
 *                         are given) 
 */


class MaterialBudgetAlg : public GaudiHistoAlg 
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
  
  /// make  random shoots 
  StatusCode makeRandomShots () ;
  /// make  grid shots 
  StatusCode makeGridShots   () ;
  /// make  psrap shots 
  StatusCode makePsrapShots  () ;
  
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
  // point of shooting 
  std::vector<double> m_vrtx          ;
  Gaudi::XYZPoint     m_vertex        ;
  // number of shots per event
  int                 m_shots        ;
  // z-position of reference plane 
  double              m_z             ;
  // x- and y- size of shooting area (parameters of 2d-histograms)
  double              m_xMax          ;
  double              m_yMax          ;
  double              m_xMin          ;
  double              m_yMin          ;
  double              m_etaMax        ;
  double              m_phiMax        ;
  double              m_etaMin        ;
  double              m_phiMin        ;
  int                 m_nbx           ;
  int                 m_nby           ;
  // parameters for "grid shooting" (if grid=1) (added by W. Pokorski)
  // variables ending with "ref" correspond to some reference grid.
  // the actuall grid will be scaled according to m_z/m_zref
  int                 m_grid          ;
  // parameter to switch between cartesian (0) and pseudorapidity (1)
  // flat scans.
  int                 m_psrap         ;
  double              m_xbinref       ;
  double              m_ybinref       ;
  double              m_zref          ;
  
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif // DETDESC_MaterialBudgetALG_H
// ============================================================================
