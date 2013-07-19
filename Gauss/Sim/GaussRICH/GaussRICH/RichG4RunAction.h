// $Id: RichG4RunAction.h,v 1.9 2007-01-12 15:32:04 ranjard Exp $
#ifndef RichG4RunAction_h 
#define RichG4RunAction_h 1

// Include files
// GiGa
#include "GiGa/GiGaRunActionBase.h"

// forward declarations
class RichG4HistoDefineSet1;
class RichG4HistoDefineSet2;
class RichG4HistoDefineSet3;
class RichG4HistoDefineSet4;
class RichG4HistoDefineSet5;

class RichG4HistoDefineTimer;

/** @class GiGaRunActionBase GiGaRunActionBase.h src/RichActions//GiGaRunActionBase.h
 *  
 *  @author Sajan Easo
 *  @author Gloria Corti (port to Gaudi v19)
 *  @date   2002-08-21, last modified 2007-01-11
 */

class RichG4RunAction: public virtual GiGaRunActionBase
{
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


  /** standard constructor
   *  @see GiGaPhysListBase
   *  @see GiGaBase
   *  @see AlgTool
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  RichG4RunAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;

  // destructor (virtual and protected)
  virtual ~RichG4RunAction( );

  RichG4HistoDefineSet1* aRichG4HistoDefineSet1()
  {return  m_aRichG4HistoSet1; }

  RichG4HistoDefineSet2* aRichG4HistoDefineSet2()
  {return  m_aRichG4HistoSet2; }

  RichG4HistoDefineSet3* aRichG4HistoDefineSet3()
  {return  m_aRichG4HistoSet3; }


  bool defineRichG4HistoSet1()
  {
    return  m_defineRichG4HistoSet1;

  }
  void setdefineRichG4HistoSet1(bool aboolValue)
  {
    m_defineRichG4HistoSet1= aboolValue;
  }
  bool defineRichG4HistoSet2()
  {
    return  m_defineRichG4HistoSet2;

  }
  void setdefineRichG4HistoSet2(bool aboolValue2)
  {
    m_defineRichG4HistoSet2= aboolValue2;
  }
  void setdefineRichG4HistoSet3(bool aboolValue3)
  {
    m_defineRichG4HistoSet3= aboolValue3;
  }
  void setdefineRichG4HistoSet4(bool aboolValue4)
  {
    m_defineRichG4HistoSet4= aboolValue4;
  }

  void setdefineRichG4HistoSet5(bool aboolValue5)
  {
    m_defineRichG4HistoSet5= aboolValue5;
  }

  bool FirstTimeOfBeginRichRun() {
    return m_FirstTimeOfBeginRichRun;}

  void setFirstTimeOfBeginRichRun( bool aboolValue) {
    m_FirstTimeOfBeginRichRun=aboolValue;}


private:

  ///no default constructor
  RichG4RunAction();
  /// no copy constructor
  RichG4RunAction( const RichG4RunAction& );
  /// no assignement
  RichG4RunAction& operator=( const RichG4RunAction& );

private:

  COMMANDS  m_beginCmds;
  COMMANDS  m_endCmds;
  bool m_defineRichG4HistoSet1;
  bool m_defineRichG4HistoSet2;
  bool m_defineRichG4HistoSet3;
  bool m_defineRichG4HistoSet4;
  bool m_defineRichG4HistoSet5;
  bool m_defineRichG4HistoTimer;


  RichG4HistoDefineSet1* m_aRichG4HistoSet1;
  RichG4HistoDefineSet2* m_aRichG4HistoSet2;
  RichG4HistoDefineSet3* m_aRichG4HistoSet3;
  RichG4HistoDefineSet4* m_aRichG4HistoSet4;
  RichG4HistoDefineSet5* m_aRichG4HistoSet5;
  RichG4HistoDefineTimer* m_aRichG4HistoTimer;

  bool  m_FirstTimeOfBeginRichRun;

};

#endif ///< GIGA_GIGARUNACTIONCOMMAND_H

