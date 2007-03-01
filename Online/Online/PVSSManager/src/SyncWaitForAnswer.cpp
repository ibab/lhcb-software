// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/SyncWaitForAnswer.cpp,v 1.1 2007-03-01 10:40:00 frankb Exp $
//  ====================================================================
//  SyncWaitForAnswer.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $

#include "PVSSManager/SyncWaitForAnswer.h"
#include "PVSS/DevAnswer.h"

#include "Manager.hxx"
#include "AnswerGroup.hxx"

using namespace PVSS;


/// Start working on request
void SyncWaitForAnswer::setAnswerState(DevAnswer::State s) const {
  if ( m_answer ) m_answer->setState(s);
}

/// Add error to answer structure
void SyncWaitForAnswer::addError(const char* txt)  const  {
  if ( m_answer )
    m_answer->addError(txt);
  else 
    printf("----------Error:%s\n",txt);
}

/// Collect possible errors
bool SyncWaitForAnswer::collectErrors(DpMsgAnswer &answer)  const {
  bool err = false;
  for(AnswerGroup *g=answer.getFirstGroup(); g; g=answer.getNextGroup() )  {
    if ( g->getError() )  {
      err = true;
      addError(g->getError()->getErrorText());
    }
  }
  return err;
}

void SyncWaitForAnswer::hotLinkCallBack(DpMsgAnswer &answer)  {
  setAnswerState(DevAnswer::WORKING);
  call(answer);
  setAnswerState(collectErrors(answer) ? DevAnswer::ERROR : DevAnswer::FINISHED);
}

