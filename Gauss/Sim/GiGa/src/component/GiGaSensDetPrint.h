// $Id: GiGaSensDetPrint.h,v 1.4 2002-05-07 12:21:36 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef       GIGA_GiGaSensDetPRINT_H
#define       GIGA_GiGaSensDetPRINT_H 1 
// ============================================================================
/// GiGa 
#include "GiGa/GiGaSensDetBase.h"
/// forward declarations 
template <class SD> 
class GiGaSensDetFactory;

/** @class GiGaSensDetPrint GiGaSensDetPrint.h GiGaSensDetPrint.h
 *
 *  Example of "primitive" implementation of sensitive detector
 *  just print some information inside G4 sensitive volume, no 
 *  real hit creation
 *
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */
class GiGaSensDetPrint: virtual public GiGaSensDetBase
{
  /// friend factory 
  friend class GiGaFactory<GiGaSensDetPrint>;

protected:
  
  /** standard constructor 
   *  @see GiGaSensDetBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaSensDetPrint
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaSensDetPrint();
  
public: 
  
  /** process the hit
   *  @param step     pointer to current Geant4 step 
   *  @param history  pointert to touchable history 
   */
  virtual bool ProcessHits
  ( G4Step* step                , 
    G4TouchableHistory* history ) ;
  
private:
  ///
  GiGaSensDetPrint(); ///< no default constructor
  GiGaSensDetPrint( const GiGaSensDetPrint& ); ///< no copy constructor 
  GiGaSensDetPrint& operator=( const GiGaSensDetPrint& ) ; ///< no = 
  ///
private:
  ///
};

// ============================================================================
// The END 
// ============================================================================
#endif  ///< GIGA_GiGaSensDetPRINT_H
// ============================================================================
