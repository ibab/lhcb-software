// $Id: IGiGaUIsession.h,v 1.2 2004-02-20 18:58:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/12/04 21:12:49  ibelyaev
//  eliminate GiGa's dependency on Vis and UI code
//
// ============================================================================
#ifndef GIGA_IGIGAUISESSION_H 
#define GIGA_IGIGAUISESSION_H 1
// GiGa
#include "GiGa/IGiGaInterface.h"
// forward declaration class from Geant4
class G4UIsession ;

/** @class IGiGaUIsession IGiGaUIsession.h GiGa/IGiGaUIsession.h
 *  
 *  Definition of pseudo-abstract pseudo-interface for 
 *  GiGa Interactive User interface  
 * 
 *  Here the first time the new phylosophy is applied, 
 *  inpired by J.P.Wellish
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-04
 */
class IGiGaUIsession : public virtual IGiGaInterface 
{
  
public:
  
  /** Retrieve the unique interface ID (static)
   *  @see IInterface
   */
  static const InterfaceID& interfaceID () ;
  
  /** get the pointer to G4 user interface
   *  @return pointer to G4 user interface
   */
  virtual G4UIsession* session () const = 0 ;
  
protected:
  
  ///  virtual destructor (protected)  
  virtual ~IGiGaUIsession () ;

};

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_IGIGAUISESSION_H
// ============================================================================
