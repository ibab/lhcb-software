// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/27 17:03:20  ibelyaev
/// improved printout
///
/// Revision 1.4  2001/07/23 13:12:28  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef    GIGA_GiGaPhysListGEANTINO_H
#define    GIGA_GiGaPhysListGEANTINO_H 1 
// ============================================================================

#include "GiGa/GiGaPhysListBase.h"
template <class PL> 
class GiGaPhysListFactory;

/** @class  GiGaPhysListGeantino GiGaPhysListGeantino.h
 *    
 *   "Real" implementation of "Physics List"
 *  it correspond to novice example N01.
 *   Only geantino is defined  
 *   
 *  PL defines only a geantino and a transportation
 *   process. Though cutoff will not be applied onto geantino, a dummy
 *  value must be set.
 *
 *  @author Vanya Belyaev
 */


class GiGaPhysListGeantino : public GiGaPhysListBase
{
  ///
  friend class GiGaPhysListFactory<GiGaPhysListGeantino>;
  ///
 protected:
  ///
  GiGaPhysListGeantino( const std::string& , ISvcLocator* );
  virtual ~GiGaPhysListGeantino();
  ///
public: 

  /** initialization of the object
   *  @return status code 
   */
  virtual StatusCode initialize();
  
  /** finalization of the object
   *  @return status code 
   */
  virtual StatusCode finalize  ();

  /// from G4VUserPhysicsList 
  void ConstructParticle ();
  void ConstructProcess  ();
  void SetCuts           ();
  ///
 private:
  ///
  GiGaPhysListGeantino();
  GiGaPhysListGeantino           ( const GiGaPhysListGeantino& );
  GiGaPhysListGeantino& operator=( const GiGaPhysListGeantino& );
  ///
};


// ============================================================================
#endif   ///< GIGA_GiGaPhysListGEANTINO_H
// ============================================================================
