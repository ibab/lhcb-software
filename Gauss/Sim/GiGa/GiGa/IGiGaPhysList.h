// $Id: IGiGaPhysList.h,v 1.9 2004-02-20 18:58:17 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2003/04/06 18:49:46  ibelyaev
//  see $GIGAROOT/doc/release.notes
//
// ============================================================================
#ifndef        GIGA_IGiGaPhysList_H
#define        GIGA_IGiGaPhysList_H 1 
// ============================================================================
// GiGa 
#include "GiGa/IGiGaPhysicsList.h"
// Geant4 
#include "G4VUserPhysicsList.hh"

/** @class IGiGaPhysList IGiGaPhysList.h GiGa/IGiGaPhysList.h
 *
 *  definition of (pseudo)abstract (pseudo)interface 
 *    to Geant 4 Physics List class
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

class IGiGaPhysList: 
  virtual public   IGiGaPhysicsList ,
  virtual public G4VUserPhysicsList 
{
public:
  
  /** uniqie interface identification
   *  @see IInterface 
   *  @see InterfaceID 
   *  @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() ;
  
protected:
  
  // virtual destructor 
  virtual ~IGiGaPhysList() ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif   ///<   GIGA_IGiGaPhysList_H
// ============================================================================
