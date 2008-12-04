// $Id: FileIdInfo.cpp,v 1.1 2008-12-04 13:31:21 frankb Exp $
// Include files 
//-----------------------------------------------------------------------------
// Implementation file for class : FileIdInfo
//
// 2008-11-20 : Markus Frank
//-----------------------------------------------------------------------------
#include "GaudiOnline/FileIdInfo.h"
using namespace LHCb;

void FileIdInfo::reset() {
  ip0 = ip1 = l1 = l1 = l2 = 0;
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
}

void FileIdInfo::setguid(const std::string& data) {
  l2 = data.length()+1;
  ::strncpy(pdata+l0+l2,data.c_str(),l2);  
}
