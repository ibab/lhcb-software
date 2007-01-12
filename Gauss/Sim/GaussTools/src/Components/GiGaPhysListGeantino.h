// $Id: GiGaPhysListGeantino.h,v 1.2 2007-01-12 15:36:52 ranjard Exp $ 
#ifndef    GIGA_GiGaPhysListGEANTINO_H
#define    GIGA_GiGaPhysListGEANTINO_H 1 

// include files 
// GiGa 
#include "GiGa/GiGaPhysListBase.h"
// forward declarations 
//template <class TYPE> class GiGaFactory;

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
  /// friend factory for instantiation
  //  friend class GiGaFactory<GiGaPhysListGeantino>;

public:
  // protected:

  
  /** standard constructor 
   *  @see GiGaPhysListBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaPhysListGeantino
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  // constructor (virtual and protected)
  virtual ~GiGaPhysListGeantino();

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
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GiGaPhysListGEANTINO_H
// ============================================================================
