// $Id: GiGaPhysListEm.h,v 1.1 2002-09-26 18:10:59 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2002/05/07 12:21:35  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef    GIGA_GiGaPhysListEM_H
#define    GIGA_GiGaPhysListEM_H 1 
// ============================================================================
// include files 
// GiGa 
#include "GiGa/GiGaPhysListBase.h"
// forward declarations 
template <class TYPE> class GiGaFactory;

/** @class  GiGaPhysListEm        GiGaPhysListEm.h  
 *
 *  "Real" implementation of "Physics List"         
 *   it correspond("cut&paste") to novice example N02.
 *   
 *  In this example, all the so called 'electromagnetic processes' are
 *  introduced for gamma, charged leptons, and charged hadrons (see the
 *  method PhysicsList::ConstructEM()). 
 *  
 *   @author Vanya Belyaev
 */


class GiGaPhysListEm : public GiGaPhysListBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaPhysListEm>;

protected:
  
  /** standard constructor 
   *  @see GiGaPhysListBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaPhysListEm
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaPhysListEm();

public:
  
  /// from G4VUserPhysicsList 
  void ConstructParticle ();
  void ConstructProcess  ();
  void SetCuts           ();
  ///
private:
  
  /// these methods Construct particles 
  void ConstructBosons   ();
  void ConstructLeptons  ();
  void ConstructMesons   ();
  void ConstructBaryons  ();
  ///
  /// these methods Construct physics processes and register them
  void ConstructGeneral  ();
  void ConstructEM       ();
  ///
private:
  ///
  GiGaPhysListEm();
  GiGaPhysListEm           ( const GiGaPhysListEm& );
  GiGaPhysListEm& operator=( const GiGaPhysListEm& );
  ///
};
// ============================================================================


// ============================================================================
#endif   ///< GIGA_GiGaPhysListEM_H
// ============================================================================











