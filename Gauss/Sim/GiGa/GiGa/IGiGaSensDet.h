// $Id: IGiGaSensDet.h,v 1.6 2004-02-20 18:58:17 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/05/07 12:21:31  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
// 
// ============================================================================
#ifndef    GIGA_IGiGaSensDet_H
#define    GIGA_IGiGaSensDet_H 1 
// GiGa
#include "GiGa/IGiGaInterface.h"
// base class from Geant4 
#include "G4VSensitiveDetector.hh" 

/** @class IGiGaSensDet IGiGaSensDet.h GiGa/IGiGaSensDet.h
 *
 *  Definition of pseudo-abstract pseudo-interface for GiGa Sensitive Detector 
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    23/01/2001
 */

class IGiGaSensDet: 
  public virtual G4VSensitiveDetector,
  public virtual IGiGaInterface       
{
public:
  
  /** Retrieve the unique interface ID (static)
   *  @see IInterface
   */
  static const InterfaceID& interfaceID () ;
  
protected:
  
  ///  virtual destructor   
  virtual ~IGiGaSensDet() ;
  
protected:
  
  // default constructor
  IGiGaSensDet() ;
  
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif  // GIGA_IGiGaSensDet_H
// ============================================================================













