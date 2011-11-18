// $Id: FileIdInfo.cpp,v 1.3 2009-04-27 08:40:42 frankb Exp $
// Include files 
//-----------------------------------------------------------------------------
// Implementation file for class : FileIdInfo
//
// 2008-11-20 : Markus Frank
//-----------------------------------------------------------------------------
#include "GaudiOnline/FileIdInfo.h"
#include <cstring>

using namespace LHCb;

void FileIdInfo::reset() {
  ip0 = ip1 = l0 = l1 = l2 = 0;
  pdata[0] = 0;
}

void FileIdInfo::setipar(const unsigned long* data) {
  if ( data ) {
    ip0 = int(data[0]);
    ip1 = int(data[1]);
  }
}

void FileIdInfo::setpar(const std::string* data) {
  l0 = data[0].length()+1;
  l1 = data[1].length()+1;
  ::strncpy(pdata,data[0].c_str(),l0);
  ::strncpy(pdata+l0,data[1].c_str(),l1);
  pdata[l0-1] = pdata[l0+l1-1] = 0;
}

void FileIdInfo::setguid(const std::string& data) {
  l2 = data.length()+1;
  ::strncpy(pdata+l0+l1,data.c_str(),l2);
  pdata[l0+l1+l2-1] = 0;
}
