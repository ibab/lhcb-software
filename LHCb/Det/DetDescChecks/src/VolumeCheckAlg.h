// $Id: VolumeCheckAlg.h,v 1.1.1.1 2002-05-26 12:47:06 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef DETDESCCHECKS_VOLUMECHECKALG_H 
#define DETDESCCHECKS_VOLUMECHECKALG_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/Algorithm.h"
class IHistogram2D ;  // AIDA 
class ILVolume     ;  // DETDESC 
class IRndmGenSvc  ;  // GaudiKernel 

/** @class VolumeCheckAlg VolumeCheckAlg.h
 *  
 *  Simple algorithm which performs an extensive and detailed 
 *  check of logical volume 
 * 
 *  The algorithm performs an extensive printout for given  
 *  logical volume, its solid( if any) and material (if any).
 *
 *  The algorithm produces 8 2-dimentional plots with 
 *  material profile for the given logical volume. 
 *  The actual profiles could be obtained by the proper 
 *  normalization of distributions, e.g. using PAW: 
 *  @code 
 *  PAW > hi/li
 *
 *  ===> Directory : 
 *          1 (2)    3D-Material Budget for /dd/Geometry/Pipe/BeamPipe  
 *          2 (2)    3D-Material Budget (N) for /dd/Geometry/Pipe/BeamPipe  
 *          3 (2)    XY-Material Budget for /dd/Geometry/Pipe/BeamPipe  
 *          4 (2)    XY-Material Budget (N) for /dd/Geometry/Pipe/BeamPipe  
 *          5 (2)    YZ-Material Budget for /dd/Geometry/Pipe/BeamPipe  
 *          6 (2)    YZ-Material Budget (N) for /dd/Geometry/Pipe/BeamPipe  
 *          7 (2)    ZX-Material Budget for /dd/Geometry/Pipe/BeamPipe  
 *          8 (2)    ZX-Material Budget (N) for /dd/Geometry/Pipe/BeamPipe  
 *  PAW > hi/oper/div 1 2 10 
 *  PAW > hi/oper/div 3 4 30
 *  PAW > hi/oper/div 5 6 50
 *  PAW > hi/oper/div 7 8 70
 *  PAW > hi/plot 10 lego1.sph
 *  PAW > hi/plot 30 lego1    
 *  PAW > hi/plot 50 lego1
 *  PAW > hi/plot 70 lego1
 *  @endcode 
 *
 *  The first pair of the plots (1&2) represents the material budget
 *  of the volume itself in spherical coordinates. Thus the histogram 10 
 *  form example above shold have the same "topological" characteristics 
 *  as the whole logical volume. This distribution must be viewed using 
 *  SPHERICAL coordinates  (option @p SPH for @p PAW). 
 *  
 *  Other pairs of distributions represent the xy (3&4), yz(5&6) and 
 *  zx(7&8) projections of the volume.
 *
 *  The upper and lower limits of the histograms are evaluated automatically 
 *  for logical volumes of the type LVolume. For logical volumes of the type 
 *  LAssembly ("Logical Assembly") the parameters shoudl be given by algorithm 
 *  properties  @p MinX, @p MaxX , @p MinY, @p MaxY , @p MinZ and @p MaxZ .
 *  
 *  The full list of algorithm properties and their default values:
 *  <ul>                   
 *  <li> @p Volume    The name (full path)  of logical volume to be tested 
 *                                                           (@b mandatory )   
 *  <li> @p Shoots3D  Number of random shoots per event for 3D-profile 
 *                                                 (default value @p 10000 ) 
 *  <li> @p ShootsXY  Number of random shoots per event for XY-profile 
 *                                                 (default value @p 10000 ) 
 *  <li> @p ShootsYZ  Number of random shoots per event for YX-profile 
 *                                                 (default value @p 10000 ) 
 *  <li> @p ShootsZX  Number of random shoots per event for ZX-profile 
 *                                                 (default value @p 10000 ) 
 *  <li> @p Null      Position of the "null vertex" for 3D-profile
 *                                (default value <tt>{ 0. , 0. , 0. }</tt>)
 *  <li> @p MinX      Minimal value of @p X for 2D-projection profiles. 
 *                    It is used only for logical assemblies
 *                                             (default value is @p -10000 )
 *  <li> @p MaxX      Maximal value of @p X for 2D-projection profiles. 
 *                    It is used only for logical assemblies
 *                                             (default value is @p  10000 )
 *  <li> @p MinY      Minimal value of @p Y for 2D-projection profiles. 
 *                    It is used only for logical assemblies
 *                                             (default value is @p -10000 )
 *  <li> @p MaxY      Maximal value of @p Y for 2D-projection profiles. 
 *                    It is used only for logical assemblies
 *                                             (default value is @p  10000 )
 *  <li> @p MinZ      Minimal value of @p Z for 2D-projection profiles. 
 *                    It is used only for logical assemblies
 *                                             (default value is @p -10000 )
 *  <li> @p MaxZ      Maximal value of @p Z for 2D-projection profiles. 
 *                    It is used only for logical assemblies
 *                                             (default value is @p  10000 ) 
 *  <li> @p RandomNumberService The name of the Gaudi Random Number Service 
 *                                       (default value is @p "RndmGenSvc" )
 *  </ul> 
 *
 *  @see ILVolume 
 *  @see  LVolume 
 *  @see  LAssembly 
 *  @see  Material 
 *  @see ISolid 
 *  @see  SolidBox
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-05-25
 */

class VolumeCheckAlg : 
  public Algorithm 
{
  
  /// friend factory for instantiation 
  friend class AlgFactory<VolumeCheckAlg>;
  
public:
  
  /** standard algorithm initialization
   *  @see IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize () ;

  /** standard execution of algorithm 
   *  @see IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute    () ;
  
  /** standard algorithm finalization
   *  @see IAlgorithm
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:

  /** Standard constructor
   *  @param name name of the algorithm
   *  @param svcloc pointer to Service Locator 
   */
  VolumeCheckAlg( const std::string& name   , 
                  ISvcLocator*       svcloc ) ;
  
  /// destructor (protected and virtual)
  virtual ~VolumeCheckAlg(); 

private:

  /// default constructor   is private 
  VolumeCheckAlg();
  /// copy    constructor   is private 
  VolumeCheckAlg
  ( const VolumeCheckAlg& );
  /// assignement operator  is private 
  VolumeCheckAlg& operator=
  ( const VolumeCheckAlg& );
  
private:
  
  // volume name 
  std::string         m_volumeName     ;
  // volume itself 
  const ILVolume*     m_volume         ;
  
  // volume limits (for assemblies)
  double              m_minx           ;
  double              m_maxx           ;
  double              m_miny           ;
  double              m_maxy           ;
  double              m_minz           ;
  double              m_maxz           ;
  
  // number of shoots  
  int                 m_shootsSphere   ; 
  int                 m_shootsXY       ;
  int                 m_shootsYZ       ;
  int                 m_shootsZX       ;

  // random numbers 
  std::string         m_rndmSvcName    ;  
  IRndmGenSvc*        m_rndmSvc        ;
  
  // point of shooting for sphere 
  std::vector<double> m_vrtx           ;
  HepPoint3D          m_vertex         ;
  
  // budget histogram for sphere shooting and its normailzation
  IHistogram2D*       m_sphere         ;
  IHistogram2D*       m_sphereNorm     ;

  // budget histogram for xy-shooting  and its normailzation
  IHistogram2D*       m_planeXY        ;
  IHistogram2D*       m_planeXYNorm    ;

  // budget histogram for yz-shooting  and its normailzation
  IHistogram2D*       m_planeYZ        ;
  IHistogram2D*       m_planeYZNorm    ;
  
  // budget histogram for zx-shooting  and its normailzation
  IHistogram2D*       m_planeZX        ;
  IHistogram2D*       m_planeZXNorm    ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // DETDESCCHECKS_VOLUMECHECKALG_H
// ============================================================================
