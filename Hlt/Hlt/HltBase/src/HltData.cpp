
#include "HltBase/HltData.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltData
//
// 2007-11-25 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

Hlt::Data::~Data() {
  for (std::vector<Hlt::Selection*>::iterator it = m_selections.begin();
       it != m_selections.end(); ++it)
    delete *it;
} 

