// $Id: ICaloDigits4Track.h,v 1.1 2009-10-25 14:40:11 ibelyaev Exp $
// ============================================================================
#ifndef CALOINTERFACES_ICALODIGITS4TRACK_H 
#define CALOINTERFACES_ICALODIGITS4TRACK_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/CaloCellID.h"
// ============================================================================
// DigiEvent 
// ============================================================================
#include "Event/CaloDigit.h"
// ============================================================================
// LHCbMath 
// ============================================================================
#include "LHCbMath/Line.h"
// ============================================================================
// forward decalrations
// ============================================================================
namespace LHCb
{
  class Track     ;
  class CaloDigit ;
}
// ============================================================================
/** @class ICaloDigits4Track CaloInterfaces/ICaloDigits4Track.h
 *  
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-10-25
 */
class GAUDI_API ICaloDigits4Track : public virtual IAlgTool
{
public: 
  // ==========================================================================
  /// the actual type of line in 3D  
  typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector>       Line      ;
  // ==========================================================================
public:
  // ==========================================================================
  /** collect the cellID-s along the line 
   *  @param line   (INPUT)  the line  
   *  @param cells  (OUTPUT) the container of cells 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const Line&               line   , 
    LHCb::CaloCellID::Vector& cells  ) const = 0 ;
  // ==========================================================================
  /** collect the cellID-s along the line 
   *  @param line   (INPUT)  the line  
   *  @param cells  (OUTPUT) the container of cells 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const Line&               line   , 
    LHCb::CaloCellID::Set&    cells  ) const = 0 ;
  // ==========================================================================  
public:
  // ==========================================================================
  /** collect the cellID-s along the path of the tracks 
   *  @param track  (INPUT)  the track 
   *  @param cells  (OUTPUT) the container of cells 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const LHCb::Track*        track  , 
    LHCb::CaloCellID::Vector& cells  ) const = 0 ;
  // ==========================================================================
  /** collect the cellID-s along the path of the tracks 
   *  @param track  (INPUT)  the track 
   *  @param cells  (OUTPUT) the container of cells 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const LHCb::Track*        track  , 
    LHCb::CaloCellID::Set&    cells  ) const = 0 ;
  // ==========================================================================  
public:
  // ==========================================================================  
  /** collect the fired digits along the line 
   *  @param line   (INPUT)  the line  
   *  @param digits (OUTPUT) the container of digits 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const Line&               line   , 
    LHCb::CaloDigit::Vector&  digits ) const = 0 ;
  // ==========================================================================
  /** collect the fired digits along the path of the tracks 
   *  @param line   (INPUT)  the line  
   *  @param digits (OUTPUT) the container of digits 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const Line&               line   , 
    LHCb::CaloDigit::Set&     digits ) const = 0 ;
  // ==========================================================================  
public: 
  // ==========================================================================  
  /** collect the fired digits along the path of the tracks 
   *  @param track  (INPUT)  the track 
   *  @param digits (OUTPUT) the container of digits 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const LHCb::Track*                   track  , 
    LHCb::CaloDigit::Vector& digits ) const = 0 ;
  // ==========================================================================
  /** collect the fired digits along the path of the tracks 
   *  @param track  (INPUT)  the track 
   *  @param digits (OUTPUT) the container of digits 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const LHCb::Track*        track  , 
    LHCb::CaloDigit::Set&     digits ) const  = 0 ;
  // ==========================================================================  
public: 
  // =========================================================================
  /** static interface identification
   *  @see IInterface
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID () ;
  // ==========================================================================  
protected:
  // ==========================================================================
  /// virtual & protected desctructor 
  virtual ~ICaloDigits4Track() ;             // virtual & protected desctructor 
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // CALOINTERFACES_ICALODIGITS4TRACK_H
// ============================================================================
