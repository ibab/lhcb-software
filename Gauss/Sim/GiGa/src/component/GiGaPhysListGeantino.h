/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef    GIGA_GiGaPhysListGEANTINO_H
#define    GIGA_GiGaPhysListGEANTINO_H 1 
/// ===========================================================================

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


/// ===========================================================================
#endif   ///< GIGA_GiGaPhysListGEANTINO_H
/// ===========================================================================
