// $Id: EventServerRunable.h,v 1.7 2008-10-14 08:37:21 frankb Exp $
//====================================================================
//  MooreTestSvc
//--------------------------------------------------------------------
//
//  Description: Runable to run Moore standalone on a single node.
//  Author     : M.Frank
//====================================================================
// $Header: $
#ifndef MOORESTANDALONE_INTERNALS_H
#define MOORESTANDALONE_INTERNALS_H

#include <map>
#include <ctime>
#include <string>
/*f
 *   LHCb namespace declaration
 */
namespace LHCb {

  namespace MooreTest {
    struct StatEntry   {
      timespec time;
      unsigned long evt_seen;
      unsigned long evt_prod;
    };
    struct UserStats   {
      StatEntry  start;
      StatEntry  last;
      StatEntry  now;
      UserStats& operator+=(const UserStats& c);
    };
    typedef std::map<std::string,UserStats> Statistics;
    enum __global__cmds__
    {
      CMD_CLOSE = 99900,
      CMD_MOORE_EXIT,
      CMD_ADD_MOORE_CLIENT = 200000,
      CMD_REMOVE_MOORE_CLIENT = 200001,
      CMD_UPDATE_MOORE_INFO   = 200002
    };
  }
}
#endif // MOORESTANDALONE_INTERNALS_H
