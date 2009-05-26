// $Id: TrCutsRunAction.h,v 1.9 2009-05-26 17:14:34 gcorti Exp $
#ifndef GAUSSTOOLS_COMPONENTS_TRCUTSRUNACTION_H 
#define GAUSSTOOLS_COMPONENTS_TRCUTSRUNACTION_H 1
// Include files
// from GiGa
#include "GiGa/GiGaRunActionBase.h"


/** @class TrCutsRunAction TrCutsRunAction.h
 *  
 *  Class to pass Tracking Cuts to Geant4 as processes. 
 *  Modified to  
 *  
 *
 *  @author Witek POKORSKI
 *  @date   2003-04-10
 *  @author Gloria CORTI
 *  @date   2009-03-31
 */

class TrCutsRunAction: public virtual GiGaRunActionBase
{
  
public:
  
  /** performe the action at the begin of each run 
   *  @param run pointer to Geant4 run object 
   */
  virtual void BeginOfRunAction ( const G4Run* run );
  
  /** performe the action at the end  of each event 
   *  @param run pointer to Geant4 run object 
   */
  virtual void EndOfRunAction   ( const G4Run* run );
  
  //protected:
  
  /** standard constructor 
   *  @see GiGaPhysListBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  TrCutsRunAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  // destructor (virtual and protected)
  virtual ~TrCutsRunAction( );
  
private:
  
  ///no default constructor
  TrCutsRunAction();
  /// no copy constructor 
  TrCutsRunAction( const TrCutsRunAction& );  
  /// no assignement 
  TrCutsRunAction& operator=( const TrCutsRunAction& );

private:
  
  double m_ecut;         ///< kinetic energy cut for e+/-
  double m_phcut;        ///< kinetic energy cut for gamma
  double m_pKpicut ;     ///< kinetic energy cut for proton, K+/- and pi+/-
  double m_ocut;         ///< kinetic energy cut for other particles
  double m_ncut;         ///< kinetic energy cut for neutrons
  double m_nucut;        ///< kinetic energy cut for neutrinos (when tracked)
  double m_mcut;         ///< kinetic energy cut for muons
  bool m_killloops;      ///< kill looping e- and gamma
  int m_maxsteps;        ///< maximum number of step looping
  double m_minstep;      ///< minimum lenght for looping cut
  double m_minx, m_miny, m_minz, m_maxx, m_maxy, m_maxz; ///< world cut limits
  bool m_killPingPong;   ///< kill particles ping ponging
  int m_nMaxForPingPong; ///< maximum number of steps to check pingpong
  int m_nMaxOfPingPong;  ///< maximum number of ping pong
  double m_stepLenghtPingPong; ///< step lenght to consider in pingpong
  std::vector<int> m_killAtOrigin; ///< particle to kill at origin (def=neutrino)
  bool m_killZeroSteps;   ///< kill particle with 'zero' step 
  int m_nStepsForZero;    ///< number of steps after which to check for 'zero' steps
  double m_zeroMaxLenght; ///< max lenght to consider a step of 'zero' lenght
  int m_maxNZeroSteps;    ///< number of steps of zero length to trigger cut
  std::string m_world;    ///< Name of world volume

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif ///< GIGA_TRCUTSRUNACTION_H
// ============================================================================
