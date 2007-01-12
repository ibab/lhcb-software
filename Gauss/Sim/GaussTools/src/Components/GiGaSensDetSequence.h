// $Id: GiGaSensDetSequence.h,v 1.2 2007-01-12 15:36:56 ranjard Exp $
#ifndef GIGASENSDETSEQUENCE_H 
#define GIGASENSDETSEQUENCE_H 1

// Include files
// from GiGa
#include "GiGa/GiGaSensDetBase.h"

/** @class GiGaSensDetBase GiGaSensDetBase.h
 *  
 *
 *  @author Gloria CORTI
 *  @date   2006-07-12
 */
class GiGaSensDetSequence : virtual public GiGaSensDetBase {

  /// Friend factory for instantiation (constructor protected in base class)
  //  friend class GiGaFactory<GiGaSensDetSequence>;

public:
  //protected:

  /// Standard constructor
  GiGaSensDetSequence( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~GiGaSensDetSequence( ); ///< Destructor

public: 

  /** initialize method (Gaudi).
   *  Called after creating the tool
   *  @see    GiGaSensDetBase
   *  @see    GiGaBase
   *  @see    GaudiTool
   *  @return StatusCode 
   */
  virtual StatusCode initialize();
  
  /** finalize method (Gaudi).
   *  Called before deleting the tool
   *  @see    GiGaSensDetBase
   *  @see    GiGaBase
   *  @see    GaudiTool
   *  @return StatusCode 
   */
  virtual StatusCode finalize();

  /** Initialize method (Geant4).
   *  Called at the beginning of each G4event
   *  @see    G4VSensitiveDetector
   *  @param  HCE pointer to hits collections of current G4event
   */
  virtual void Initialize( G4HCofThisEvent* HCE );

  /** Process a hit (Geant4).
   *  The method is invoked by G4 for each step in the 
   *  sensitive detector. 
   *  This implementation calls the same methods of different Sensitive
   *  detectors to allow to associate more than one to a given volume.
   *  @see   G4VSensitiveDetector
   *  @param step     pointer to current Geant4 step 
   *  @param history  pointert to touchable history 
   */
  virtual bool ProcessHits( G4Step* step, G4TouchableHistory* history );

  /** EndOfEvent method (Geant4).
   *  Called at the end of each G4event
   *  @see    G4VSensitiveDetector
   *  @param  HCE pointer to hits collections of current G4event
   */
  virtual void EndOfEvent( G4HCofThisEvent* HCE );

private:

  /// Names of GiGaSensDets to be executed
  std::vector< std::string >   m_sensDetsNames;
  /// Pointers to GiGaSensDets to excute 
  std::vector< IGiGaSensDet* > m_sensDets;

};
#endif // GIGASENSDETSEQUENCE_H
