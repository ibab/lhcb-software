// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/25 18:13:39  ibelyaev
/// add new component GiGaEventActionCommand
///
// ============================================================================
#ifndef GIGA_GIGAEVENTACTIONCOMMAND_H 
#define GIGA_GIGAEVENTACTIONCOMMAND_H 1
// ============================================================================
/// GiGa
#include "GiGa/GiGaEventActionBase.h"
/// forward declarations
template <class EA>
class GiGaEventActionFactory;

/** @class GiGaEventActionCommand GiGaEventActionCommand.h
 *  
 *  A concrete Event Action. 
 *  It executes some Geant4 commands at begin and at the 
 *  end of each event
 *
 *  @author Vanya Belyaev
 *  @date   25/07/2001
 */

class GiGaEventActionCommand: public GiGaEventActionBase
{
  /// friend factory for instantiation
  friend class GiGaEventActionFactory<GiGaEventActionCommand>;
  ////
public:

  /// useful typedef 
  typedef  std::vector<std::string> COMMANDS;
  
  /** initialization of event action object
   *  @return status code
   */
  virtual StatusCode initialize();
  
  /** finalization of event action object
   *  @return status code
   */
  virtual StatusCode finalize  ();
  
  /** performe the action at the begin of each event 
   *  @param event pointer to Geant4 event object 
   */
  virtual void BeginOfEventAction ( const G4Event* event );
  
  /** performe the action at the end  of each event 
   *  @param event pointer to Geant4 event object 
   */
  virtual void EndOfEventAction   ( const G4Event* event );
  
protected:
  
  /** standard constructor
   *  @param Name  name of this concrete event action instance 
   *  @param Loc   poinetr to service locator 
   */
  GiGaEventActionCommand( const std::string& Name ,
                          ISvcLocator*       Loc  );
  /// destructor
  virtual ~GiGaEventActionCommand( );
  
private:
  
  ///no default constructor
  GiGaEventActionCommand();
  /// no copy constructor 
  GiGaEventActionCommand( const GiGaEventActionCommand& );  
  /// no assignement 
  GiGaEventActionCommand& operator=( const GiGaEventActionCommand& );

private:
  
  COMMANDS  m_beginCmds;
  COMMANDS  m_endCmds;

};

// ============================================================================
#endif ///< GIGA_GIGAEVENTACTIONCOMMAND_H
// ============================================================================
