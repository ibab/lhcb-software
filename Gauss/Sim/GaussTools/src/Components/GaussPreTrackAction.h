// ============================================================================
#ifndef       GIGA_GaussPreTrackAction_H
#define       GIGA_GaussPreTrackAction_H 1 
// ============================================================================
// STL 
#include <string>
#include <vector>
// GiGa
#include "GiGa/GiGaTrackActionBase.h"

// forward declarations 
//template <class TYPE> class GiGaFactory;
class G4Track;
class G4ParticleDefinition;

/** @class GaussPreTrackAction GaussPreTrackAction.h 
 *
 * Gauss tracking action deals with storing tracks
 * and handles track information objects
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  
 *  @date    23/01/2001
 */


class GaussPreTrackAction: virtual public GiGaTrackActionBase
{
  /// friend factory for instantiation 
  //  friend class GiGaFactory<GaussPreTrackAction>;
  
  ///
  //protected:
public:
  
  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GaussPreTrackAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussPreTrackAction();

public: 
  
  /** initialize the track action  
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ; 
  
  /** finalize the action object 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code
   */
  virtual StatusCode finalize   () ;
  
  /** perform action 
   *  @see G4UserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PreUserTrackingAction  ( const G4Track* ) ;

  /** perform action 
   *  @see G4UserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PostUserTrackingAction ( const G4Track* ){};

private:

  GaussPreTrackAction() ; ///< no default constructor
  GaussPreTrackAction( const GaussPreTrackAction& ) ; ///< no copy 
  GaussPreTrackAction& operator=( const GaussPreTrackAction& ) ;
  
};

// ============================================================================
#endif  //    GIGA_GaussPreTrackAction_H
// ============================================================================
















