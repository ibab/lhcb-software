// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/26 16:09:22  ibelyaev
/// GiGaRunActions budiness
///
///
// ============================================================================
#ifndef GIGA_GIGARUNACTIONCOMMAND_H 
#define GIGA_GIGARUNACTIONCOMMAND_H 1
// ============================================================================
/// GiGa
#include "GiGa/GiGaRunActionBase.h"
/// forward declarations
template <class RA>
class GiGaRunActionFactory;

/** @class GiGaRunActionCommand GiGaRunActionCommand.h
 *  
 *  A concrete Run Action. 
 *  It executes some Geant4 commands at begin and at the 
 *  end of each run
 *
 *  @author Vanya Belyaev
 *  @date   25/07/2001
 */

class GiGaRunActionCommand: public GiGaRunActionBase
{
  /// friend factory for instantiation
  friend class GiGaRunActionFactory<GiGaRunActionCommand>;
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
  
  /** performe the action at the begin of each run 
   *  @param run pointer to Geant4 run object 
   */
  virtual void BeginOfRunAction ( const G4Run* run );
  
  /** performe the action at the end  of each event 
   *  @param run pointer to Geant4 run object 
   */
  virtual void EndOfRunAction   ( const G4Run* run );
  
protected:
  
  /** standard constructor
   *  @param Name  name of this concrete event action instance 
   *  @param Loc   poinetr to service locator 
   */
  GiGaRunActionCommand( const std::string& Name ,
                          ISvcLocator*       Loc  );
  /// destructor
  virtual ~GiGaRunActionCommand( );
  
private:
  
  ///no default constructor
  GiGaRunActionCommand();
  /// no copy constructor 
  GiGaRunActionCommand( const GiGaRunActionCommand& );  
  /// no assignement 
  GiGaRunActionCommand& operator=( const GiGaRunActionCommand& );

private:
  
  COMMANDS  m_beginCmds;
  COMMANDS  m_endCmds;

};

// ============================================================================
#endif ///< GIGA_GIGARUNACTIONCOMMAND_H
// ============================================================================
