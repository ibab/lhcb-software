// $Id: TrCutsRunAction.h,v 1.8 2009-04-05 17:50:24 gcorti Exp $
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
  
  double m_ecut;
  double m_phcut;
  double m_pKpicut ;
  double m_ocut;
  double m_ncut;
  double m_nucut;
  double m_mcut;
  bool m_killloops;
  int m_maxsteps;
  double m_minstep;
  double m_minx, m_miny, m_minz, m_maxx, m_maxy, m_maxz;
  bool m_killPingPong;
  int m_nMaxForPingPong;
  int m_nMaxOfPingPong;
  double m_stepLenghtPingPong;
  std::vector<int> m_killAtOrigin;


};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif ///< GIGA_TRCUTSRUNACTION_H
// ============================================================================
