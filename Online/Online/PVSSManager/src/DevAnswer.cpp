// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DevAnswer.cpp,v 1.3 2007-10-01 14:46:55 frankm Exp $
//  ====================================================================
//  CfgDetail.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DevAnswer.cpp,v 1.3 2007-10-01 14:46:55 frankm Exp $

#include "PVSS/DevAnswer.h"

using namespace PVSS;
#include <cstdlib>
#include <cstring>

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
    ::printf("PVSS> %s\n",m_errs[i]);
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
