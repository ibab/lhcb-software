// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/SyncWaitForAnswer.cpp,v 1.3 2007-10-01 14:46:55 frankm Exp $
//  ====================================================================
//  SyncWaitForAnswer.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: SyncWaitForAnswer.cpp,v 1.3 2007-10-01 14:46:55 frankm Exp $

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
    ::printf("PVSS> ----------Error:%s\n",txt);
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

