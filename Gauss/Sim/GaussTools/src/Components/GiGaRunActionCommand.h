// $Id: GiGaRunActionCommand.h,v 1.1 2002-12-12 15:19:32 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/26 18:10:52  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.3  2002/05/07 12:21:35  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef GIGA_GIGARUNACTIONCOMMAND_H 
#define GIGA_GIGARUNACTIONCOMMAND_H 1
// ============================================================================
// GiGa
#include "GiGa/GiGaRunActionBase.h"
// forward declarations
template <class TYPE> class GiGaFactory;

/** @class GiGaRunActionCommand GiGaRunActionCommand.h
 *  
 *  A concrete Run Action. 
 *  It executes some Geant4 commands at begin and at the 
 *  end of each run
 *
 *  @author Vanya Belyaev
 *  @date   25/07/2001
 */

class GiGaRunActionCommand: public virtual GiGaRunActionBase
{
  /// friend factory for instantiation
  friend class GiGaFactory<GiGaRunActionCommand>;
  
public:

  /// useful typedef 
  typedef  std::vector<std::string> COMMANDS;
  
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
   *  @see GiGaPhysListBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaRunActionCommand
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  // destructor (virtual and protected)
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

// ============================================================================
// The END 
// ============================================================================
#endif ///< GIGA_GIGARUNACTIONCOMMAND_H
// ============================================================================
