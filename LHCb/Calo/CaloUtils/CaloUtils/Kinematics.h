// $Id: $
// ============================================================================
#ifndef CALOUTILS_KINEMATICS_H 
#define CALOUTILS_KINEMATICS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloPosition.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LorentzVectorWithError.h"
// ============================================================================
/** @file 
 *  Helper file to collect simple functions to 
 *  (re)calculate various kinmatical properties for Calorieet robjects 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikkef.nl
 *  @date 2010-08--29
 *
 *  Last modification $Date$
 *                 by $AUthor$
 */
namespace Calo 
{
  // ==========================================================================
  namespace Kinematics 
  {
    // ========================================================================
    /** evaluate 4-momentum of photon hypothesis 
     *  for the given calo-position object
     *  @param calo    (INPUT) calo-position object 
     *  @param origin  (INPUT) the assumed origin point 
     *  @return the estimate for the photon 4-momentum 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08--29
     */
    GAUDI_API Gaudi::LorentzVector momentum 
    ( const LHCb::CaloPosition& calo   , 
      const Gaudi::XYZPoint&    origin ) ;
    // ========================================================================
    /** evaluate 4-momentum of photon pythothesis 
     *  for the given calo-position object
     *  @param calo    (INPUT)  calo-position object 
     *  @param origin  (INPUT)  the assumed origin point 
     *  @param output  (OUTPUT) the estimate for the photon 4-momentum 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08--29
     */
    GAUDI_API void momentum 
    ( const LHCb::CaloPosition& calo   , 
      const Gaudi::XYZPoint&    origin , 
      Gaudi::LorentzVector&     output ) ;
    // ========================================================================
    /** evaluate 4-momentum of photon hypothesis 
     *  for the given calo-position object
     *  @param calo    (INPUT)  calo-position object 
     *  @param origin  (INPUT)  the assumed origin point 
     *  @param output  (OUTPUT) the estimate for the photon 4-momentum 
     *  @param matrix  (OUTPUT) the estimate for the photon covariance matrix 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08--29
     */
    GAUDI_API void momentum 
    ( const LHCb::CaloPosition& calo   , 
      const Gaudi::XYZPoint&    origin , 
      Gaudi::LorentzVector&     output , 
      Gaudi::SymMatrix4x4&      matrix ) ;
    // ========================================================================
    /** evaluate 4-momentum of photon hypothesis 
     *  for the given calo-position object
     *  @param calo    (INPUT)  calo-position object 
     *  @param origin  (INPUT)  the assumed origin point 
     *  @param output  (OUTPUT) the estimate for the photon 4-momentum 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08--29
     */
    GAUDI_API void momentum 
    ( const LHCb::CaloPosition&            calo   , 
      const Gaudi::XYZPoint&               origin , 
      Gaudi::Math::LorentzVectorWithError& output ) ;
    // ========================================================================
    /** add photon 4-momentum for the given calo-position object
     *  @param calo    (INPUT)  calo-position object 
     *  @param origin  (INPUT)  the assumed origin point 
     *  @param output  (UPDATE) the estimate for 4-momentum
     *  @param matrix  (UPDATE) the estimate for the covariance matrix 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08--29
     */
    GAUDI_API void add2momentum 
    ( const LHCb::CaloPosition& calo   , 
      const Gaudi::XYZPoint&    origin , 
      Gaudi::LorentzVector&     output , 
      Gaudi::SymMatrix4x4&      matrix ) ;
    // ========================================================================
    /** add photon 4-momentum for the given calo-position object
     *  for the given calo-position object
     *  @param calo    (INPUT)  calo-position object 
     *  @param origin  (INPUT)  the assumed origin point 
     *  @param output  (OUTPUT) the estimate for the 4-momentum 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08--29
     */
    GAUDI_API void add2momentum 
    ( const LHCb::CaloPosition&            calo   , 
      const Gaudi::XYZPoint&               origin , 
      Gaudi::Math::LorentzVectorWithError& output ) ;
    // ========================================================================
    /** get the jacobian: d(px,py,pz,E)/d(xc,yc,E)
     *  @param calo     (INPUT)  calo-position object 
     *  @param origin   (INPUT)  the assumed origin point 
     *  @param F11      (OUTPUT) the jacobian d(px,py,pz,E)/d(xc,yc,E)
     *  @return 4-momentum estimate 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08--29
     */        
    GAUDI_API Gaudi::LorentzVector jacobian1 
    ( const LHCb::CaloPosition& calo     , 
      const Gaudi::XYZPoint&    origin   , 
      Gaudi::Matrix4x3&         F11      ) ;
    // ========================================================================
    /** get the "jacobian": d(px,py,pz,E)/d(xv,yv,zv)
     *  @param calo     (INPUT)  calo-position object 
     *  @param origin   (INPUT)  the assumed origin point 
     *  @param F12      (OUTPUT) the jacobian d(px,py,pz,E)/d(xv,yv,zv)
     *  @return 4-momentum estimate 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08--29
     */        
    GAUDI_API Gaudi::LorentzVector jacobian2 
    ( const LHCb::CaloPosition& calo     , 
      const Gaudi::XYZPoint&    origin   , 
      Gaudi::Matrix4x3&         F12      ) ;
    // ========================================================================
    /** get the jacobians: d(px,py,pz,E)/d(xc,yc,E) and d(px,py,pz,E)/d(xv,yv,zv)
     *  @param calo     (INPUT)  calo-position object 
     *  @param origin   (INPUT)  the assumed origin point 
     *  @param F11      (OUTPUT) the jacobian d(px,py,pz,E)/d(xc,yc,E)
     *  @param F12      (OUTPUT) the jacobian d(px,py,pz,E)/d(xv,yv,zv)
     *  @return 4-momentum estimate 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08--29
     */        
    GAUDI_API Gaudi::LorentzVector jacobians
    ( const LHCb::CaloPosition& calo     , 
      const Gaudi::XYZPoint&    origin   , 
      Gaudi::Matrix4x3&         F11      ,
      Gaudi::Matrix4x3&         F12      ) ;
    // ========================================================================
  } //                                        end of namespace Calo::Kinematics 
  // ==========================================================================
} //                                                      end of namespace Calo 
// ============================================================================
// The END 
// ============================================================================
#endif // CALOUTILS_KINEMATICS_H
// ============================================================================
