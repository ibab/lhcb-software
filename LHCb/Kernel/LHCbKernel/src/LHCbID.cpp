// $Id: LHCbID.cpp,v 1.1 2007-07-11 13:33:34 jonrob Exp $

// local
#include "Kernel/LHCbID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LHCbID
//
// 2007-07-11 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& LHCb::LHCbID::fillStream(std::ostream& s) const
{
  s << "{ LHCbID : ";
  if (isVelo()) { s << veloID(); }
  if (isST())   { s << stID();   }
  if (isOT())   { s << otID();   }
  if (isRich()) { s << richID(); }
  if (isCalo()) { s << caloID(); }
  if (isMuon()) { s << muonID(); }
  return s << " }";
}
