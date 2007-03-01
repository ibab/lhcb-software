#ifndef PVSSMANAGER_SYNCWAITFORANSWER_H
#define PVSSMANAGER_SYNCWAITFORANSWER_H

// PVSS include files
#include "HotLinkWaitForAnswer.hxx"
#include "PVSS/DevAnswer.h"

/*
 * PVSS namespace
 */
namespace PVSS {

  // Framework forward declarations  
  class DevAnswer;

  /** @class SyncWaitForAnswer SyncWaitForAnswer.h PVSSManager/SyncWaitForAnswer.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *
    */
  class SyncWaitForAnswer : public HotLinkWaitForAnswer    {
  protected:
    /// Device answer callback
    DevAnswer*   m_answer;
  public:
    /// Initializing constructor
    SyncWaitForAnswer(DevAnswer* a) : m_answer(a)  {      }
    /// Default destructor
    virtual ~SyncWaitForAnswer()     {                    }
    /// Access Answer sync
    DevAnswer* waitContext() const   {  return m_answer;  }
    /// Add error to answer structure
    void addError(const char* err)  const;
    /// Start working on request
    void setAnswerState(DevAnswer::State s)   const;
    /// Collect possible errors
    bool collectErrors(DpMsgAnswer &answer) const;

    virtual void call(DpMsgAnswer &answer)  {}
    // Answer on conenct
    void hotLinkCallBack(DpMsgAnswer &answer);
    void hotLinkCallBack(DpHLGroup &group)  {}
  };
}
#endif  /* PVSSMANAGER_SYNCWAITFORANSWER_H  */
