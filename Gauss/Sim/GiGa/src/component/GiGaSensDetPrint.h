// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/27 17:56:07  ibelyaev
/// add new component GiGaSensDetPrint
/// 
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
  friend class GiGaSensDetFactory<GiGaSensDetPrint>;
  ///
protected:
  
  /** standard consrtructor 
   *  @param Name name of sensitive detector 
   *  @param Loc  pointer to service Locator 
   */ 
  GiGaSensDetPrint( const std::string&  Name , 
                    ISvcLocator*        Loc  );
  /// destructor 
  virtual ~GiGaSensDetPrint();

public: 

  /** initialize the sensitive detector 
   *  @return status code 
   */
  virtual StatusCode initialize () ; 
  
  /** finalize the sensitive detector 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** process the hit
   *  @param step     pointer to current Geant4 step 
   *  @param history  pointert to touchable history 
   */
  virtual bool ProcessHits( G4Step* step                , 
                            G4TouchableHistory* history ) ; 
  ///
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
#endif  ///< GIGA_GiGaSensDetPRINT_H
// ============================================================================
