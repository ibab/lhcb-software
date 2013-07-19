// $Id: RichG4TrackAction.h,v 1.2 2007-01-12 15:32:05 ranjard Exp $
#ifndef       RichG4TrackAction_h
#define       RichG4TrackAction_h 1 

// Include files
#include <string>
#include <vector>

// GiGa
#include "GiGa/GiGaTrackActionBase.h"

// forward declarations 
class G4Track;
class G4particleDefinition;

/** @class GiGaTrackActionBase GiGaTrackActionBase.h src/RichActions/GiGaTrackActionBase.h
 *  
 *
 *  @author Sajan Easo
 *  @author Gloria Corti (port to Gaudi v19)
 *  @date   last modified 2007-01-11
 */
class RichG4TrackAction: virtual public GiGaTrackActionBase
{
public: 
  /// useful typedefs
  typedef  std::vector<std::string>                  TypeNames; 
  typedef  std::vector<const G4ParticleDefinition*>  PartDefs; 

  /// Standard constructor
  RichG4TrackAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~RichG4TrackAction();

  
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


#endif // RichG4TrackAction_h

















