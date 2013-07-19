// $Id: RichG4TrackActionPhotOpt.h,v 1.5 2009-07-03 11:59:49 seaso Exp $
#ifndef RICHACTION_RICHG4TRACKACTIONPHOTOPT_HH 
#define RICHACTION_RICHG4TRACKACTIONPHOTOPT_HH 1

// Include files
// STL 
#include <string>
#include <vector>
// GiGa
#include "GiGa/GiGaTrackActionBase.h"

/// Geant4 
#include "globals.hh"

/** @class RichG4TrackActionPhotOpt 
 * RichG4TrackActionPhotOpt.hh 
 * RichAction/RichG4TrackActionPhotOpt.hh
 *  
 *
 *  @author Sajan EASO
 *  @author Gloria Corti (adapt to Gaudi v19)
 *  @date   2003-04-29, last modified 01
 */

// forward declarations
class G4Track;
class G4particleDefinition;

class RichG4TrackActionPhotOpt: virtual public GiGaTrackActionBase 
{
public:

  /// useful typedefs
  typedef  std::vector<std::string>                  TypeNames; 
  typedef  std::vector<const G4ParticleDefinition*>  PartDefs; 
  ///

  virtual StatusCode initialize () ;   ///< initialize

  /// Standard constructor
  RichG4TrackActionPhotOpt ( const std::string& type   ,
                             const std::string& name   ,
                             const IInterface*  parent ) ;

   virtual ~RichG4TrackActionPhotOpt( ); ///< Destructor


  virtual void PreUserTrackingAction  ( const G4Track* ) ;

  //  virtual void PostUserTrackingAction ( const G4Track* ) ;

private:

  // no default constructor.
  RichG4TrackActionPhotOpt ();
  // no copy
  RichG4TrackActionPhotOpt (const  RichG4TrackActionPhotOpt& );
  RichG4TrackActionPhotOpt& operator=(const  RichG4TrackActionPhotOpt& );
  
  G4double  m_MaxHpdQuantumEffFromDB;
  G4double  m_MaxRich1Mirror1Reflect;
  G4double  m_MaxRich1Mirror2Reflect;
  G4double  m_MaxRich2Mirror1Reflect;
  G4double  m_MaxRich2Mirror2Reflect;
  G4double  m_ZDownstreamOfRich1;
  G4double  m_Rich1TotPhotonSuppressFactor;
  G4double  m_Rich2TotPhotonSuppressFactor;
  G4bool    m_RichHpdMaxQEOverRideDB;
  G4double  m_RichHpdMaxQEValueWhenOverRideDB;
  
};

#endif // RICHACTION_RICHG4TRACKACTIONPHOTOPT_HH
