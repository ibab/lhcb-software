/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef    GIGA_GIGAVISMANAGER_H 
#define    GIGA_GIGAVISMANAGER_H 1 
/// ===========================================================================

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

/// ===========================================================================
#endif  ///< GIGA_GIGAVISMANAGER_H 
/// ===========================================================================
