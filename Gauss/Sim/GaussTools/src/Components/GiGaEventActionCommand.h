// $Id: GiGaEventActionCommand.h,v 1.1 2002-12-12 15:19:30 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/26 18:10:51  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.3  2002/05/07 12:21:34  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef GIGA_GIGAEVENTACTIONCOMMAND_H 
#define GIGA_GIGAEVENTACTIONCOMMAND_H 1
// ============================================================================
/// GiGa
#include "GiGa/GiGaEventActionBase.h"
/// forward declarations
template <class TYPE> class GiGaFactory;

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
  friend class GiGaFactory<GiGaEventActionCommand>;
  
public:
  
  /// useful typedef 
  typedef  std::vector<std::string> COMMANDS;
  
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
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaEventActionCommand
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent );
  
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
