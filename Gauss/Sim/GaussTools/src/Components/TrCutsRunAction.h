// $Id: TrCutsRunAction.h,v 1.7 2007-01-12 15:36:58 ranjard Exp $
#ifndef GIGA_TRCUTSRUNACTION_H 
#define GIGA_TRCUTSRUNACTION_H 1

// GiGa
#include "GiGa/GiGaRunActionBase.h"
// forward declarations
//template <class TYPE> class GiGaFactory;

/** @class TrCutsRunAction TrCutsRunAction.h
 *  
 *
 *  @author Witek Pokorski
 *  @date   10/04/2003
 */

class TrCutsRunAction: public virtual GiGaRunActionBase
{
  /// friend factory for instantiation
  //friend class GiGaFactory<TrCutsRunAction>;
  
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
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif ///< GIGA_TRCUTSRUNACTION_H
// ============================================================================
