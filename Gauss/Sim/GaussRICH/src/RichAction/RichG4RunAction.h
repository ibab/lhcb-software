#ifndef RichG4RunAction_h 
#define RichG4RunAction_h 1
// ============================================================================
// GiGa
#include "GiGa/GiGaRunActionBase.h"

// forward declarations
template <class TYPE> class GiGaFactory;
class RichG4HistoDefineSet1;
class RichG4HistoDefineTimer;

// SE 21-8-2002

class RichG4RunAction: public virtual GiGaRunActionBase
{
  /// friend factory for instantiation
  friend class GiGaFactory<RichG4RunAction>;
  
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
  RichG4RunAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  // destructor (virtual and protected)
  virtual ~RichG4RunAction( );

  RichG4HistoDefineSet1* aRichG4HistoDefineSet1() 
   {return  m_aRichG4HistoSet1; }

  bool defineRichG4HistoSet1() 
  {
    return  m_defineRichG4HistoSet1;
    
  }

  void setdefineRichG4HistoSet1(bool aboolValue) 
  {
    m_defineRichG4HistoSet1= aboolValue;
  }

  
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
  bool m_defineRichG4HistoTimer;
  
  
  RichG4HistoDefineSet1* m_aRichG4HistoSet1;
  RichG4HistoDefineTimer* m_aRichG4HistoTimer;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif ///< GIGA_GIGARUNACTIONCOMMAND_H
// ============================================================================
