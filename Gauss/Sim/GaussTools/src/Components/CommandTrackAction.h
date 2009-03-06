// ============================================================================
#ifndef       GIGA_CommandTrackAction_H
#define       GIGA_CommandTrackAction_H 1 
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

/** @class CommandTrackAction CommandTrackAction.h 
 *
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  
 *  @date    23/01/2001
 */


class CommandTrackAction: virtual public GiGaTrackActionBase
{
  /// friend factory for instantiation 
  //  friend class GiGaFactory<CommandTrackAction>;

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
  CommandTrackAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~CommandTrackAction();

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
  virtual void PostUserTrackingAction ( const G4Track* ) ;
  

  ///
private:

  CommandTrackAction() ; ///< no default constructor
  CommandTrackAction( const CommandTrackAction& ) ; ///< no copy 
  CommandTrackAction& operator=( const CommandTrackAction& ) ; 

  void printInfo(const G4Track*, bool tkInfo = true); ///< Print info about track

private:
  
  bool m_test;
  std::vector<int> m_trackIDs;
  bool m_printIDs;
  bool m_trackInfos;
  std::vector<std::string> m_precommand;
  std::vector<std::string> m_postcommand;
};


#endif  //    GIGA_CommandTrackAction_H
