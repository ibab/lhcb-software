// $Id: IGiGaMagField.h,v 1.10 2004-02-20 18:58:17 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2003/04/06 18:49:46  ibelyaev
//  see $GIGAROOT/doc/release.notes
//
// Revision 1.8  2002/05/07 12:21:30  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ===========================================================================
#ifndef     GIGA_IGIGAMagField_H
#define     GIGA_IGIGAMagField_H 1 
// ===========================================================================
// GiGa 
#include "GiGa/IGiGaFieldMgr.h" 
// from G4 
#include "G4MagneticField.hh"

/** @interface IGiGaMagField IGiGaMagField.h "GiGa/IGiGaMagField.h"
 *
 *  definition of (pseudo) abstract (pseudo) interface 
 *  for Geant 4 Magnetic field class
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

class IGiGaMagField: 
  virtual public G4MagneticField , 
  virtual public IGiGaFieldMgr 
{
public:
  
  /// Retrieve the unique interface ID
  static const InterfaceID&  interfaceID() ;
  
protected:
  
  /// destructor 
  virtual ~IGiGaMagField() ;
};
// ===========================================================================

// ===========================================================================
// The END 
// ===========================================================================
#endif   ///< GIGA_IGIGAMagField_H
// ===========================================================================
