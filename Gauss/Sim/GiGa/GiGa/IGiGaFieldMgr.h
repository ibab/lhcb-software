// $Id: IGiGaFieldMgr.h,v 1.2 2004-02-20 18:58:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/04/06 18:49:46  ibelyaev
//  see $GIGAROOT/doc/release.notes
// 
// ============================================================================
#ifndef GIGA_IGIGAFIELDMGR_H 
#define GIGA_IGIGAFIELDMGR_H 1
// ============================================================================
// Include files
#include "GiGa/IGiGaInterface.h"
// Geant4
class G4MagneticField        ;
class G4FieldManager         ;
class G4MagIntegratorStepper ;
// ============================================================================

/** @class IGiGaFieldMgr IGiGaFieldMgr.h GiGa/IGiGaFieldMgr.h
 *
 *  An abstract interface to deal with G4 Field manager, magnetic 
 *  field and stepper 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
class IGiGaFieldMgr : virtual public IGiGaInterface 
{
public:
  
  /** uniqie interface identification
   *  @see IInterface 
   *  @see InterfaceID 
   *  @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() ;
  
  /** accessor to magnetic field 
   *  @see G4MagneticField 
   *  @return pointer to the magnetic field object
   */
  virtual G4MagneticField*        field    () const = 0 ;
  
  /** accessor to field  manager 
   *  @see G4FieldManager
   *  @return pointer to the field manager  
   */
  virtual G4FieldManager*         fieldMgr () const = 0 ;
  
  /** accessor to the stepper 
   *  @see G4MagIntegratorStepper
   *  @return pointer to the stepper  
   */
  virtual G4MagIntegratorStepper* stepper  () const = 0 ;

  /** flag for indication global/local characted of 
   *  the tool field manager  
   */
  virtual bool                    global   () const = 0 ;

protected:

  // destructor 
  virtual ~IGiGaFieldMgr( ) ;

};

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_IGIGAFIELDMGR_H
// ============================================================================
