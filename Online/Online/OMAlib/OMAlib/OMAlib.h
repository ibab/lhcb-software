// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/OMAlib/OMAlib.h,v 1.1.1.1 2007-10-16 13:03:03 ggiacomo Exp $
#ifndef OMALIB_H
#define OMALIB_H 1
/** @class  OMAlib OMAlib.h OMAlib/OMAlib.h
 * 
 *  header for the Online Monitoring Analysis library
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 3/10/2007
 */
#include <map>
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAalg.h"
#include "OMAlib/OMAAlgorithms.h"
using namespace std;

namespace OMAconstants {
  static const int AlgListID = 2;
}

class OMAlib
{
 public:
  OMAlib (OnlineHistDB* HistDB);
  OMAlib (std::string DBpasswd, 
	  std::string DBuser = OnlineHistDBEnv_constants::ACCOUNT, 
	  std::string DB = OnlineHistDBEnv_constants::DB);
  virtual ~OMAlib ();
  inline OMAalg* getAlg(std::string Name)
  {return m_algorithms[Name];}
  

 private:
  void doAlgList();
  void syncList();
  bool m_localDBsession;
  OnlineHistDB* m_histDB;
  std::map<std::string, OMAalg*> m_algorithms;
  int m_debug;
};


#endif // OMALIB_H
