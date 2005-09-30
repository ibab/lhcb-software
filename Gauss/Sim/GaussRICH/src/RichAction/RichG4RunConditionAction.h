#ifndef RichG4RunConditionAction_h 
#define RichG4RunConditionAction_h 1
// ============================================================================
// GiGa
#include "GiGa/GiGaRunActionBase.h"

#include "RichKernel/IRichConditionsOverall.h"
// forward declarations
template <class TYPE> class GiGaFactory;

// SE 29-8-2005

class RichG4RunConditionAction: public virtual GiGaRunActionBase
{
  /// friend factory for instantiation
  friend class GiGaFactory<RichG4RunConditionAction>;

public:

  /// useful typedef
  typedef  std::vector<std::string> COMMANDS;

  /** performe the action at the begin of each run
   *  @param run pointer to Geant4 run object
   */
  virtual StatusCode         initialize  ()        ;


protected:

  /** standard constructor
   *  @see GiGaPhysListBase
   *  @see GiGaBase
   *  @see AlgTool
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  RichG4RunConditionAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;

  // destructor (virtual and protected)
  virtual ~RichG4RunConditionAction( );


  bool getFirstTimeOfBeginRichCondRun() 
  {    return m_FirstTimeOfBeginRichCondRun;}
  
  void setFirstTimeOfBeginRichCondRun( bool aboolValue) {
    m_FirstTimeOfBeginRichCondRun=aboolValue;}
  bool getActivateRichSimCondition() 
  {    return m_ActivateRichSimCondition;  }
  void setActivateRichSimCondition(bool aAcVal)
  {m_ActivateRichSimCondition=aAcVal;}

  IRichConditionsOverall* getRichConditionsOverall() 
  {    return m_RichConditionsOverall;}
  
    
private:

  ///no default constructor
  RichG4RunConditionAction();
  /// no copy constructor
  RichG4RunConditionAction( const RichG4RunConditionAction& );
  /// no assignement
  RichG4RunConditionAction& operator=( const RichG4RunConditionAction& );

private:


  bool  m_FirstTimeOfBeginRichCondRun;
  bool m_ActivateRichSimCondition;
  IRichConditionsOverall* m_RichConditionsOverall;
  
    
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif ///< RICHG4RUNACTIONCONDITION_H
// ============================================================================
