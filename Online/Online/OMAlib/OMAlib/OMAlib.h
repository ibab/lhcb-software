// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/OMAlib/OMAlib.h,v 1.6 2008-05-14 10:01:16 ggiacomo Exp $
#ifndef OMALIB_OMALIB_H
#define OMALIB_OMALIB_H 1
/** @class  OMAlib OMAlib.h OMAlib/OMAlib.h
 * 
 *  Online Monitoring Analysis library
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 3/10/2007
 */
#include <map>
#include "OnlineHistDB/OnlineHistDBEnv.h"
#include "OMAlib/OMAalg.h"
class OnlineHistDB;

namespace OMAconstants {
  static const int AlgListID = 4;
  static const std::string version = "v1r1";
  static const double epsilon = 1.e-10;
}


class OMAlib
{
 public:
  /// constructor to be used if already connected to HistDB (use NULL for not using HistDB)
  OMAlib (OnlineHistDB* HistDB);
  /// constructor opening a new DB session
  OMAlib (std::string DBpasswd, 
	  std::string DBuser = OnlineHistDBEnv_constants::ACCOUNT, 
	  std::string DB = OnlineHistDBEnv_constants::DB);
  /// constructor with a new default (read-only) DB session
  OMAlib();  
  virtual ~OMAlib ();
  /// retrieve algorithm from the known algorithm list
  inline OMAalg* getAlg(std::string Name){    
    return m_algorithms[Name];}
  inline OnlineHistDB* dbSession() {
    return m_histDB;}
  /// set where algorithms should send messages 
  inline void setMessageMode(OMAMsgInterface::OMAMsgMode mode) {
    std::map<std::string, OMAalg*>::iterator ia;
    for (ia=m_algorithms.begin(); ia != m_algorithms.end(); ia++) {
      ia->second->setMessageMode(mode);
    }
  }

 private:
  void doAlgList();
  void syncList();
  bool m_localDBsession;
  OnlineHistDB* m_histDB;
  std::map<std::string, OMAalg*> m_algorithms;
  int m_debug;
};


#endif // OMALIB_OMALIB_H
