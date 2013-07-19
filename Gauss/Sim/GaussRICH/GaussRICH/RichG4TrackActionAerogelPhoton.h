// $Id: RichG4TrackActionAerogelPhoton.h,v 1.3 2009-07-03 11:59:49 seaso Exp $
#ifndef RICHACTION_RICHG4TRACKACTIONAEROGELPHOTON_H 
#define RICHACTION_RICHG4TRACKACTIONAEROGELPHOTON_H 1

// Include files
#include <string>
#include <vector>
// GiGa
#include "GiGa/GiGaTrackActionBase.h"
/// Geant4
#include "globals.hh"

/** @class RichG4TrackActionAerogelPhoton RichG4TrackActionAerogelPhoton.h RichAction/RichG4TrackActionAerogelPhoton.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2006-11-01
 *
 */

// forward declarations
class G4Track;
class G4particleDefinition;

class RichG4TrackActionAerogelPhoton: virtual public GiGaTrackActionBase {

public: 

 /// useful typedefs
  typedef  std::vector<std::string>                  TypeNames;
  typedef  std::vector<const G4ParticleDefinition*>  PartDefs;
  ///

  virtual StatusCode initialize () ;

  /// Standard constructor
  RichG4TrackActionAerogelPhoton(  const std::string& type   ,
                                   const std::string& name   ,
                                   const IInterface*  parent ) ;
  virtual ~RichG4TrackActionAerogelPhoton( ); ///< Destructor

public:

  virtual void PreUserTrackingAction  ( const G4Track* ) ;

  //  virtual void PostUserTrackingAction ( const G4Track* ) ;

  bool ActivateAerogelFilter() 
  {  return m_ActivateAerogelFilter;}
  double EnergyCutoffInAerogel(){  return m_EnergyCutoffInAerogel;}
  double EnergyCutoffNoFilterInAerogel() 
  {  return  m_EnergyCutoffNoFilterInAerogel;}
  double EnergyCutoffWithFilterInAerogel() 
  {return  m_EnergyCutoffWithFilterInAerogel;}
  
  
private:
  // no default constructor.
  RichG4TrackActionAerogelPhoton ();
 // no copy
  RichG4TrackActionAerogelPhoton (const  RichG4TrackActionAerogelPhoton& );
  RichG4TrackActionAerogelPhoton& operator=(const  RichG4TrackActionAerogelPhoton& );

  bool m_ActivateAerogelFilter;
  double m_EnergyCutoffInAerogel;
  double m_EnergyCutoffNoFilterInAerogel;
  double m_EnergyCutoffWithFilterInAerogel;  


};
#endif // RICHACTION_RICHG4TRACKACTIONAEROGELPHOTON_H
