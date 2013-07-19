// $Id: Rich1G4TrackActionUpstrPhoton.h,v 1.4 2007-01-12 15:32:03 ranjard Exp $
#ifndef       Rich1G4TrackActionUpstrPhoton_h
#define       Rich1G4TrackActionUpstrPhoton_h 1

// STL
#include <string>
#include <vector>

// GiGa
#include "GiGa/GiGaTrackActionBase.h"

// forward declarations
class G4Track;
class G4particleDefinition;

class Rich1G4TrackActionUpstrPhoton: virtual public GiGaTrackActionBase
{

public:
  /// useful typedefs
  typedef  std::vector<std::string>                  TypeNames;
  typedef  std::vector<const G4ParticleDefinition*>  PartDefs;
  ///

  Rich1G4TrackActionUpstrPhoton ( const std::string& type   ,
                                  const std::string& name   ,
                                  const IInterface*  parent ) ;

  /// destructor (virtual and protected)
  virtual ~Rich1G4TrackActionUpstrPhoton();


  /** perform action
   *  @see G4UserTrackingAction
   *  @param pointer to new track opbject
   */
  virtual void PreUserTrackingAction  ( const G4Track* ) ;

  /** perform action
   *  @see G4UserTrackingAction
   *  @param pointer to new track opbject
   */
  virtual void PostUserTrackingAction ( const G4Track* ) ;


private:

  Rich1G4TrackActionUpstrPhoton() ; ///< no default constructor
  Rich1G4TrackActionUpstrPhoton ( const Rich1G4TrackActionUpstrPhoton& ) ; ///< no copy
  Rich1G4TrackActionUpstrPhoton& operator=( const Rich1G4TrackActionUpstrPhoton& ) ;  ///< no copy


};


#endif
// ============================================================================
















