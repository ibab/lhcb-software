// $Id: GaussTrajectory.h,v 1.5 2004-02-22 16:51:54 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2003/10/09 09:12:58  witoldp
// extension of GaussTrackAction
//
// Revision 1.3  2003/03/11 09:45:18  ibelyaev
//  update for Win2K
//
// Revision 1.2  2002/12/07 21:19:13  ibelyaev
//  few optimization updates
//
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
  GaussTrajectory (                         ) ;
  GaussTrajectory ( const G4Track*          ) ;
  GaussTrajectory ( const GiGaTrajectory&   ) ;
  GaussTrajectory ( const GaussTrajectory&  ) ;
  virtual ~GaussTrajectory();
  ///  
  void* operator new    ( size_t ) ;
  void  operator delete ( void*  ) ;
  ///
  int operator == (const GaussTrajectory& right) const
  { return ( &right == this );} 
  ///
  virtual void AppendStep( const G4Step*  );
  virtual GiGaTrajectory* clone() const ;
  ///
private:
  ///
};
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
inline                  GaussTrajectory* 
gaussTrajectory (       G4VTrajectory* g4 )
{
  GiGaUtil::FastCast<G4VTrajectory,GaussTrajectory> cast ;
  return cast( g4 );
}


// ============================================================================
/** @fn gaussTrajectory
 *  Fast cast of G4VTrajectory interface to concrete Gauss implementation
 *  @param  g4   pointer to G4VTrajectory interface 
 *  @return cast (dynamic or static) to GaussTrajectory
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline            const GaussTrajectory* 
gaussTrajectory ( const G4VTrajectory* g4 )
{
  GiGaUtil::FastCast<const G4VTrajectory,const GaussTrajectory> cast ;
  return cast( g4 );
}


// ============================================================================
/** @fn gaussTrajectory
 *  Fast cast of GiGaTrajectory interface to concrete Gauss implementation
 *  @param  g4   pointer to GiGaTrajectory interface 
 *  @return cast (dynamic or static) to GaussTrajectory
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline                  GaussTrajectory* 
gaussTrajectory (       GiGaTrajectory* g4 )
{
  GiGaUtil::FastCast<GiGaTrajectory,GaussTrajectory> cast ;
  return cast( g4 );
}


// ============================================================================
/** @fn gaussTrajectory
 *  Fast cast of GiGaTrajectory interface to concrete Gauss implementation
 *  @param  g4   pointer to GiGaTrajectory interface 
 *  @return cast (dynamic or static) to GaussTrajectory
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline            const GaussTrajectory* 
gaussTrajectory ( const GiGaTrajectory* g4 )
{
  GiGaUtil::FastCast<const GiGaTrajectory,const GaussTrajectory> cast ;
  return cast( g4 );
}


// ============================================================================
// The END 
// ============================================================================
#endif  ///< GaussTools_GaussTrajectory_H








