/*
 * NodeThread.h
 *
 *  Created on: Feb 11, 2016
 *      Author: beat
 */

#ifndef SOURCE_DIRECTORY__ONLINE_IPMI_SRC_NEWLIB_NODETHREAD_H_
#define SOURCE_DIRECTORY__ONLINE_IPMI_SRC_NEWLIB_NODETHREAD_H_

#include <string>
#include <mutex>
using namespace std;
class NodeConfiguration
{
  public:
    NodeConfiguration(string &N, string &U, string &P, string *sp,
        int statepoll=1, int setuppoll=10)
    {
      NodeName = N;
      UserName = U;
      Password = P;
      ServicePrefix = sp;
      this->statePollDelay = statepoll;
      this->setupRetryDelay = setuppoll;
    }
    NodeConfiguration()
    {
      NodeName = "";
      UserName = "";
      Password = "";
      ServicePrefix = 0;
      this->statePollDelay = 1;
      this->setupRetryDelay =10;
    }
    string NodeName;
    string UserName;
    string Password;
    string *ServicePrefix;
    int statePollDelay;
    int setupRetryDelay;
};
void NodeThread(NodeConfiguration *config, string *setup_fail,
    mutex *setup_fail_mtx);
#endif /* SOURCE_DIRECTORY__ONLINE_IPMI_SRC_NEWLIB_NODETHREAD_H_ */
