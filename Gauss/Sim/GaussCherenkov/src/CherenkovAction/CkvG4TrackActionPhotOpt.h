// $Id: CkvG4TrackActionPhotOpt.h,v 1.5 2009-07-03 11:59:49 seaso Exp $
#ifndef RICHACTION_CKVG4TRACKACTIONPHOTOPT_HH 
#define RICHACTION_CKVG4TRACKACTIONPHOTOPT_HH 1

// Include files
// STL 
#include <string>
#include <vector>
// GiGa
#include "GiGa/GiGaTrackActionBase.h"

/// Geant4 
#include "globals.hh"

/** @class CkvG4TrackActionPhotOpt 
 * CkvG4TrackActionPhotOpt.hh 
 * RichAction/CkvG4TrackActionPhotOpt.hh
 *  
 *
 *  @author Sajan EASO
 *  @author Gloria Corti (adapt to Gaudi v19)
 *  @date   2003-04-29, last modified 01
 */

// forward declarations
class G4Track;
class G4particleDefinition;

class CkvG4TrackActionPhotOpt: virtual public GiGaTrackActionBase 
{
public:

  /// useful typedefs
  typedef  std::vector<std::string>                  TypeNames; 
  typedef  std::vector<const G4ParticleDefinition*>  PartDefs; 
  ///

  virtual StatusCode initialize () ;   ///< initialize

  /// Standard constructor
  CkvG4TrackActionPhotOpt ( const std::string& type   ,
                             const std::string& name   ,
                             const IInterface*  parent ) ;

   virtual ~CkvG4TrackActionPhotOpt( ); ///< Destructor


  virtual void PreUserTrackingAction  ( const G4Track* ) ;

  //  virtual void PostUserTrackingAction ( const G4Track* ) ;

private:

  // no default constructor.
  CkvG4TrackActionPhotOpt ();
  // no copy
  CkvG4TrackActionPhotOpt (const  CkvG4TrackActionPhotOpt& );
  CkvG4TrackActionPhotOpt& operator=(const  CkvG4TrackActionPhotOpt& );
  
  G4double  m_MaxPhdQuantumEffFromDB;
  G4double  m_MaxRich1Mirror1Reflect;
  G4double  m_MaxRich1Mirror2Reflect;
  G4double  m_MaxRich2Mirror1Reflect;
  G4double  m_MaxRich2Mirror2Reflect;
  G4double  m_ZDownstreamOfRich1;
  G4double  m_Rich1TotPhotonSuppressFactor;
  G4double  m_Rich2TotPhotonSuppressFactor;
  G4bool    m_RichPhdMaxQEOverRideDB;
  G4double  m_RichPhdMaxQEValueWhenOverRideDB;
  
};

#endif // RICHACTION_RICHG4TRACKACTIONPHOTOPT_HH
