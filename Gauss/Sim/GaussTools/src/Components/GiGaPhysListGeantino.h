// $Id: GiGaPhysListGeantino.h,v 1.1 2002-09-26 18:11:00 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2002/05/07 12:21:35  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef    GIGA_GiGaPhysListGEANTINO_H
#define    GIGA_GiGaPhysListGEANTINO_H 1 
// ============================================================================
// include files 
// GiGa 
#include "GiGa/GiGaPhysListBase.h"
// forward declarations 
template <class TYPE> class GiGaFactory;

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
  friend class GiGaFactory<GiGaPhysListGeantino>;

 protected:

  
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
