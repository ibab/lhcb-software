// $Id: GiGaVisManager.h,v 1.1 2002-05-01 18:23:38 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2001/08/12 15:42:45  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.4  2001/07/23 13:11:44  ibelyaev
// the package restructurisation(II)
// 
// ============================================================================
#ifndef    GIGA_GIGAVISMANAGER_H 
#define    GIGA_GIGAVISMANAGER_H 1 
// ============================================================================

/** @class GiGaVisManager GiGaVisManager.h
 *  GiGa Visualization manager 
 *  @author Vanya Belyaev 
 *  @date xx/xxx/xxxxx
 */

#ifndef G4VIS_NONE
///
#include "G4VisManager.hh"
///
class GiGaVisManager: public G4VisManager 
{
  ///
public:
  ///
  GiGaVisManager ();
  ///
private:
  ///
  void RegisterGraphicsSystems ();
};

#endif  ///< G4VIS_NONE

// ============================================================================
#endif  ///< GIGA_GIGAVISMANAGER_H 
// ============================================================================
