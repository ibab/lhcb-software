#ifndef    GIGA_GiGaPhysListGEANTINO_H
#define    GIGA_GiGaPhysListGEANTINO_H 1 


#include "GiGa/GiGaPhysList.h" 


template <class PL> class GiGaPhysListFactory;

/** @class  GiGaPhysListGeantino        GiGaPhysListGeantino.h  GiGaPhysListGeantino.h
    
    "Real" implementation of "Physics List"                                            
    it correspond to novice example N01.
    Only geantino is defined  
    
    PL defines only a geantino and a transportation
    process. Though cutoff will not be applied onto geantino, a dummy
    value must be set.


    @author Vanya Belyaev
*/


class GiGaPhysListGeantino : public GiGaPhysList
{
  ///
  friend class GiGaPhysListFactory<GiGaPhysListGeantino>;
  ///
 protected:
  ///
  GiGaPhysListGeantino( const std::string& nick , ISvcLocator* loc );
  virtual ~GiGaPhysListGeantino();
  ///
  
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


#endif     GIGA_GiGaPhysListGEANTINO_H
