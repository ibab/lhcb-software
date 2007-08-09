// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/Kernel.h,v 1.3 2007-08-09 20:03:58 frankm Exp $
//  ====================================================================
//  Kernel.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_KERNEL_H
#define ONLINE_PVSS_KERNEL_H

#include <iostream>
#define _INC_IOSTREAM
using std::ostream;
using std::istream;
using std::ios;
using std::streambuf;
using std::streampos;
using std::iostream;
using std::streamoff;

namespace PVSS  {

  enum PVSS_Status_Code { ERROR, SUCCESS };
  struct PVSSStatus {
    int m_code;
    PVSSStatus() : m_code(ERROR) {}
    PVSSStatus(const PVSSStatus& c) : m_code(c.m_code) {}
    PVSSStatus(const PVSS_Status_Code& c) : m_code(c)  {}
    bool isSuccess()  {   return m_code != ERROR;       }
    PVSSStatus& operator=(int value) {
      m_code = value;
      return *this;
    }
  };
  template<typename T> struct CfgManip 
  { public: bool operator()(T*& p)  {  if ( p ) { delete p; p = 0;} return true; }  };

  template<typename T> struct DeletePtr  {
    static bool del_item(T& p) { if ( p ) { delete p; p = 0;} return true; }
    static bool second(T& p){ return DeletePtr<typename T::second_type>::del_item(p.second); }
    static bool first(T& p) { return DeletePtr<typename T::first_type>::del_item(p.first);   }
  };
  template<typename T> struct DevManip {};
}

#endif // ONLINE_PVSS_KERNEL_H
