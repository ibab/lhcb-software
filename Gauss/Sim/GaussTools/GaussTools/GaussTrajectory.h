// $Id: GaussTrajectory.h,v 1.2 2002-12-07 21:19:13 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
/** @fn gaussTrajectory
 *  Fast cast of G4VTrajectory interface to concrete Gauss implementation
 *  @param  g4   pointer to G4VTrajectory interface 
 *  @return cast (dynamic or static) to GaussTrajectory
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline GaussTrajectory* gaussTrajectory( G4VTrajectory* g4 )
{
  GiGaUtil::FastCast<G4VTrajectory,GaussTrajectory> cast ;
  return cast( g4 );
};
// ============================================================================

// ============================================================================
/** @fn gaussTrajectory
 *  Fast cast of GiGaTrajectory interface to concrete Gauss implementation
 *  @param  g4   pointer to GiGaTrajectory interface 
 *  @return cast (dynamic or static) to GaussTrajectory
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline GaussTrajectory* gaussTrajectory( GiGaTrajectory* g4 )
{
  GiGaUtil::FastCast<GiGaTrajectory,GaussTrajectory> cast ;
  return cast( g4 );
};
// ============================================================================

// ============================================================================
#endif  ///< GaussTools_GaussTrajectory_H
// ============================================================================







