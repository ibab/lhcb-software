/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2001/07/23 13:12:27  ibelyaev
/// the package restructurisation(II)
/// 
/// ===========================================================================
#ifndef    GIGA_GiGaPhysListFULL_H
#define    GIGA_GiGaPhysListFULL_H 1 
/// ===========================================================================

#include "GiGa/GiGaPhysListBase.h"

template <class PL> 
class GiGaPhysListFactory;

/** @class  GiGaPhysListFull        GiGaPhysListFull.h  GiGaPhysListFull.h
 *   
 *  "Real" implementation of "Physics List"   
 *  it correspond("cut&paste") to novice example N04:
 *   ExN04PhysicsList. It defines almost all of leptons and hadrons which
 *  Geant4 has dedicated classes for. Also almost all physics processes 
 *  Geant4 has are defined.
 *  
 *  @author Vanya Belyaev
 */

class GiGaPhysListFull : public GiGaPhysListBase
{
  ///
  friend class GiGaPhysListFactory<GiGaPhysListFull>;
  ///
protected:
  ///
  GiGaPhysListFull( const std::string& nick , ISvcLocator* loc );
  virtual ~GiGaPhysListFull();
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
  virtual void ConstructGeneral ();
  virtual void ConstructEM      ();
  virtual void ConstructHad     ();
  // these methods Construct all particles in each category
  virtual void ConstructAllBosons      ();
  virtual void ConstructAllLeptons     ();
  virtual void ConstructAllMesons      ();
  virtual void ConstructAllBaryons     ();
  virtual void ConstructAllIons        ();
  virtual void ConstructAllShortLiveds ();
  ///
private:
  ///
  GiGaPhysListFull();
  GiGaPhysListFull           ( const GiGaPhysListFull& );
  GiGaPhysListFull& operator=( const GiGaPhysListFull& );
  ///
};


/// ===========================================================================
#endif  ///< GIGA_GiGaPhysListFULL_H
/// ===========================================================================










