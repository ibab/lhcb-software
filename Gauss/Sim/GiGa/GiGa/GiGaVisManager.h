// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2001/07/23 13:11:44  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef    GIGA_GIGAVISMANAGER_H 
#define    GIGA_GIGAVISMANAGER_H 1 
// ============================================================================

/// 
#ifdef G4VIS_USE
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
  ///
};
///
#endif  ///< G4VIS_USE 

// ============================================================================
#endif  ///< GIGA_GIGAVISMANAGER_H 
// ============================================================================
