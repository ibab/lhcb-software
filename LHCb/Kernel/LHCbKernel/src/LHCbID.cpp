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
  if (isVL())   { s << vlID();   }
  if (isST())   { s << stID();   }
  if (isOT())   { s << otID();   }
  if (isRich()) { s << richID(); }
  if (isCalo()) { s << caloID(); }
  if (isMuon()) { s << muonID(); }
  if (isFT())   { s << ftID();   }
  return s << " }";
}
