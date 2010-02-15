#ifndef MONSYS_H
#define MONSYS_H
#include "RTL/rtl.h"
#include <vector>
#include <string>
#include "CCPCSubSys.h"
class MonSys
{
protected:
  std::string m_name;
  lib_rtl_lock_t m_lockid;
  std::vector<CCPCSubSys*> Subsyslist;
public:
  MonSys();
  ~MonSys();
  void addSubSys(CCPCSubSys *);
  void remSubSys(CCPCSubSys *);
  void Lock();
  void unLock();
  static MonSys& m_instance();
  MonSys *setup(char *n);
  void start();
};
#endif