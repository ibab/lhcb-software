// $Id: EcalSensDet.h,v 1.11 2008-05-30 13:51:07 gcorti Exp $
#ifndef       GAUSSCALO_EcalSensDet_H
#define       GAUSSCALO_EcalSensDet_H 1

// Ntuple Svc
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

// GiGa
#include "GiGa/GiGaSensDetBase.h"
#include "GiGa/GiGaHashMap.h"

// local
#include "CaloSim.h"
#include "CaloHit.h"
#include "EHCalSensDet.h"
/// forward declarations
//template <class SD> class GiGaSensDetFactory;

class DeCalorimeter ;

/** @class EcalSensDet EcalSensDet.h
 *
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Patrick Robbe robbe@lal.in2p3.fr
 *
 *  @date    23/01/2001
 */

class EcalSensDet: public EHCalSensDet
{
  /// friend factory
  //  friend class GiGaFactory<EcalSensDet>;

protected:

  /** fill the hit with the concrete information about the energy and the time.
   *  The function is to be called from ProcessHits method.
   *
   *  @param hit        hit  to be filled with the correct information,
   *                    the cellID information is accessible from the hit
   *  @param prePoint   PreStepPoint  for the given step
   *  @param globalTime the global time of prestep point
   *  @param deposit    raw energy deposition for the step
   *  @param track      the actual G4Track pbject (decoded from G4Step)
   *  @param pdef       the actual particle type  (decoded from G4Step/G4Track)
   *  @param material   the actual material       (decoded from G4Step)
   *  @param step       the step itself, the most important
   *                    information from the step
   *                    is already decoded into prePoint,globalTime,track,
   *                    particle definition,material
   *                    etc for efficiency reasons.
   *
   */
  virtual StatusCode    fillHitInfo
  ( CaloSubHit*                 hit         ,
    const HepGeom::Point3D<double>& prePoint    ,
    const double                globalTime  ,
    const double                deposit     ,
    const G4Track*              track       ,
    const G4ParticleDefinition* pdef        ,
    const G4MaterialCutsCouple* material    ,
    const G4Step*               step        ) const ;

public:

  /** standard constructor
   *  @see CaloSensDet
   *  @see GiGaSensDetBase
   *  @see GiGaBase
   *  @see AlgTool
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  EcalSensDet
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;

  /// destructor (virtual and protected)
  virtual ~EcalSensDet() {};

private:

  // no default constructor
  EcalSensDet() ;
  // no copy constructor
  EcalSensDet           ( const EcalSensDet& );
  // no assignement
  EcalSensDet& operator=( const EcalSensDet& ) ;

protected:

  /** Correction due to the local non uniformity due to the light
   *  collection efficiency in cell cell
   */
  inline double localNonUniformity
  ( const HepGeom::Point3D<double>& prePoint    ,
    const LHCb::CaloCellID& cell    ) const ;

private :
  // Amplitudes of the local non uniformity correction
  // depending on the region of the Ecal
  double                                      m_a_local_inner_ecal  ;
  double                                      m_a_local_middle_ecal ;
  double                                      m_a_local_outer_ecal  ;
  // Amplitudes of the global non uniformity correction
  // depending on the region of the Ecal
  double                                      m_a_global_inner_ecal  ;
  double                                      m_a_global_middle_ecal ;
  double                                      m_a_global_outer_ecal  ;
  // Correction for light reflection at the edges
  double                                      m_a_reflection_height ;
  double                                      m_a_reflection_width  ;
};
// ============================================================================




// ============================================================================
// Local Non Uniformity
// ============================================================================
/** Correction due to the local non uniformity due to the light
 *  collection efficiency in cell cell
 */

inline double EcalSensDet::localNonUniformity
( const HepGeom::Point3D<double>& prePoint ,
  const LHCb::CaloCellID& cell ) const
{

  // Only for ECal for the moment
  double correction = 1. ;

  // Find the position of the step
  double x        = prePoint.x() ;
  double y        = prePoint.y() ;
  // Center of the cell
  double x0       = calo()->cellX( cell ) ;
  double y0       = calo()->cellY( cell ) ;

  // Distance between fibers
  // and correction amplitude
  double d        = 10.1 * mm ;
  double A_local  = m_a_local_inner_ecal ; // in inner Ecal
  double A_global = m_a_global_inner_ecal ;

  // Cell size
  double cellSize = calo()->cellSize( cell ) ;

  // Assign amplitude of non uniformity as a function of the
  // Ecal region

  if ( cell.area() == 0 ) { // outer Ecal
    A_local  = m_a_local_outer_ecal ;
    A_global = m_a_global_outer_ecal ;
    d        = 15.25 * mm ;
  }
  else if ( cell.area() == 1 ) { // middle Ecal
    A_local  = m_a_local_middle_ecal ;
    A_global = m_a_global_middle_ecal ;
  }

  // Local uniformity is product of x and y sine-like functions
  // The Amplitude of the sin-like function is a function of x and
  // y
  if ( A_local > LHCb::Math::lowTolerance )
    correction += A_local / 2. * ( 1. - cos( 2.*pi * (x-x0)/d ) ) *
      ( 1. - cos( 2.*pi * (y-y0)/d ) ) ;

  double rX(0.) , rY(0.) , hCell(0.) ;

  // Global non uniformity
  if ( A_global > LHCb::Math::lowTolerance ) {
    rX = x - x0 ;
    rY = y - y0 ;
    hCell = cellSize / 2. ;
    correction +=
      A_global * ( hCell - rX ) * ( rX + hCell ) / ( hCell * hCell )
      * ( hCell - rY ) * ( rY + hCell ) / ( hCell * hCell ) ;
  }

  // Light Reflexion on the edges
  if ( m_a_reflection_height > LHCb::Math::lowTolerance ) {
    rX = rX / m_a_reflection_width ;
    rY = rY / m_a_reflection_width ;
    hCell = hCell / m_a_reflection_width ;
    correction += m_a_reflection_height *
      ( exp( - fabs ( rX + hCell ) ) + exp( - fabs ( rX - hCell ) )
        + exp( - fabs ( rY + hCell ) ) + exp( - fabs ( rY - hCell ) ) ) ;
  }

  return correction ;
}

// ============================================================================
// The END
// ============================================================================
#endif  ///< GAUSSCALO_EcalSensDet_H
// ============================================================================
