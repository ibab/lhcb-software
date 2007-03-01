// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DevAnswer.cpp,v 1.1 2007-03-01 10:40:00 frankb Exp $
//  ====================================================================
//  CfgDetail.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $

#include "PVSS/DevAnswer.h"

using namespace PVSS;

void DevAnswer::clear()  {
  if ( m_nerrs > 0 )  {
    for(size_t i=0; i<m_nerrs; ++i)  {
      ::free(m_errs[i]);
    }
    ::free(m_errs);
  }
}

/// Print errors
void DevAnswer::print() const   {
  for(size_t i=0; i<m_nerrs; ++i)  {
    printf("%s\n",m_errs[i]);
  }
}

/// Add new error
void DevAnswer::addError(const char* text)   {
  if ( m_nerrs+1 > m_mxerrs )  {
    m_errs = (char**)::realloc(m_errs,sizeof(char*)*(m_mxerrs+64));
    m_mxerrs += 64;
  }
  m_errs[m_nerrs] = (char*)::malloc(strlen(text)+1);
  ::strcpy(m_errs[m_nerrs],text);
  m_nerrs++;
}
