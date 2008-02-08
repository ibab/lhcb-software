// $Id: ROMonOstream.h,v 1.2 2008-02-08 17:28:51 frankm Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMonOstream.h,v 1.2 2008-02-08 17:28:51 frankm Exp $
#ifndef ROMON_ROMONOSTREAM_H
#define ROMON_ROMONOSTREAM_H 1

#include <iomanip>
#include <iostream>

// Framework include files
#include "ROMon/ROMon.h"

template<typename T> std::ostream& operator<<(std::ostream& os, const ROMon::FixItems<T>& items) {
  os << "Start address:" << (void*)&items
     << " Num Elements:" << std::setw(6)  << std::right << items.size()
     << " - "            << std::setw(6)  << std::left << (items.end()-items.begin())
     << " Length:"       << std::setw(4)  << std::right << items.length()
     << " DataLen:"      << std::setw(4)  << std::right << items.data_length()
     << " Item size: "   << std::setw(4)  << std::right << sizeof(T)
     << " Begin: "       << (void*)items.begin()
     << " End: "         << (void*)items.end();
  if ( items.begin() != items.end() ) os << std::endl;
  for (const T* p=items.begin(); p!=items.end(); ++p) os << *p;
  return os;
}

template<typename T> std::ostream& operator<<(std::ostream& os, const ROMon::VarItems<T>& items) {
  os << "Start address:" << (void*)&items
     << " Num Elements:" << std::setw(6)  << std::right << items.size()
     << " - "            << std::setw(6)  << std::left << (items.end()-items.begin())
     << " Length:"       << std::setw(4)  << std::right << items.length()
     << " DataLen:"      << std::setw(4)  << std::right << items.data_length()
     << " Item size: "   << std::setw(4)  << std::right << sizeof(T)
     << " Begin: "       << (void*)items.begin()
     << " End: "         << (void*)items.end();
  if ( items.begin() != items.end() ) os << std::endl;
  for (const T* p=items.begin(); p!=items.end(); p=items.next(p)) os << *p;
  return os;
}

std::ostream& operator<<(std::ostream& os, const CONTROL& c);
std::ostream& operator<<(std::ostream& os, const ROMon::MBMClient& c);
std::ostream& operator<<(std::ostream& os, const ROMon::MBMBuffer& b);
std::ostream& operator<<(std::ostream& os, const ROMon::FSMTask& t);
std::ostream& operator<<(std::ostream& os, const ROMon::Node& n);
std::ostream& operator<<(std::ostream& os, const ROMon::Nodeset& n);

#endif /* ROMON_ROMONOSTREAM_H */

