// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.11  2001/08/12 15:42:45  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.10  2001/07/23 13:11:44  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef    GAUSSTOOLS_GAUSSTRAJECTORY_H
#define    GAUSSTOOLS_GAUSSTRAJECTORY_H 1 
// ============================================================================
/// GiGa
#include "GiGa/GiGaTrajectory.h"
///

class GaussTrajectory: public GiGaTrajectory
{
  ///
public:
  ///
  inline GaussTrajectory (                        );
  inline GaussTrajectory ( const G4Track*         );
  inline GaussTrajectory ( const GaussTrajectory & );
  virtual inline ~GaussTrajectory();
  ///  
  inline void* operator new    ( size_t ) ;
  inline void  operator delete ( void*  ) ;
  ///
  inline int operator == (const GaussTrajectory& right) const
  {return ( &right == this );} 
  ///
  virtual        void AppendStep      ( const G4Step*  )       ;
  ///
private:
  ///
};
///
#include "GaussTools/GaussTrajectory.icpp"
///

// ============================================================================
#endif  ///< GaussTools_GaussTrajectory_H
// ============================================================================







