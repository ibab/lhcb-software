// ============================================================================
#ifndef       RichG4TrackAction_h
#define       RichG4TrackAction_h 1 
// ============================================================================
// STL 
#include <string>
#include <vector>
// GiGa
#include "GiGa/GiGaTrackActionBase.h"
// forward declarations 
template <class TYPE> class GiGaFactory;
class G4Track;
class G4particleDefinition;

class RichG4TrackAction: virtual public GiGaTrackActionBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<RichG4TrackAction>;

public: 
  /// useful typedefs
  typedef  std::vector<std::string>                  TypeNames; 
  typedef  std::vector<const G4ParticleDefinition*>  PartDefs; 
  ///
protected:

  RichG4TrackAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~RichG4TrackAction();

public: 
  
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

  RichG4TrackAction() ; ///< no default constructor
  RichG4TrackAction( const RichG4TrackAction& ) ; ///< no copy 
  RichG4TrackAction& operator=( const RichG4TrackAction& ) ; 


};


#endif 
// ============================================================================
















