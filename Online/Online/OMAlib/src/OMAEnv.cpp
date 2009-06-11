// $Id: OMAEnv.cpp,v 1.1 2009-06-11 16:00:21 ggiacomo Exp $

#include "OMAlib/OMAEnv.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAFitFunction.h"
#include <stdlib.h>
#include <sstream>

//-----------------------------------------------------------------------------
// Implementation file for class : OMAEnv
//
// 2008-08-11 : Giacomo GRAZIANI
//-----------------------------------------------------------------------------


OMAEnv::OMAEnv(OnlineHistDB* HistDB,  
               std::string Name) :
  m_debug(2), m_anaTaskname(Name), m_histDB(HistDB)
{
  setDefRefRoot();
}

OMAEnv::~OMAEnv() {
  if (m_localDBsession)
    delete m_histDB;
}


// sets the default path of reference histogram files
void OMAEnv::setDefRefRoot() {
#ifdef _WIN32
  m_RefRoot = "H:";
#else
  m_RefRoot = "/hist/";
#endif
  char * envHistDir = getenv ("HISTDIR");
  if (envHistDir !=NULL) {
    m_RefRoot = envHistDir;
    m_RefRoot += "/";
  }
  if(m_histDB) {
    m_RefRoot += m_histDB->refRoot();
  }
  else {
    char * envRefRoot = getenv ("HISTREFPATH");
    if (envRefRoot !=NULL)
      m_RefRoot += envRefRoot;
    else
      m_RefRoot += OnlineHistDBEnv_constants::StdRefRoot;
  }
}



