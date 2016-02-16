/*
 * NodeThread.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: beat
 */

#include <string>
#include <mutex>
//#include <vector>
#include "string.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "ctype.h"
#include "dim/dis.hxx"
#include "src/NewLib/IPMINode.h"
#include "src/NewLib/ipmi_intf.h"
#include "NodeThread.h"

using namespace std;

void NodeThread(NodeConfiguration *config,string *setup_fail, mutex *setup_fail_mtx )
{
  int status;
  string uname;
  string pword;
  uname = config->UserName;
  pword = config->Password;
  string nam = config->NodeName;
  string *ServicePrefix = config->ServicePrefix;
  IPMINode *nod;
  while (1)
  {
//    m.lock();
    nod = new IPMINode(nam, uname, pword);
    status = nod->setup((char*)ServicePrefix->c_str());
    if (status == 0)
    {
//      m.unlock();
//      printf("Setup of node %s successful\n", nam.c_str());
      setup_fail_mtx->lock();
      size_t fpos = setup_fail->find(nam);
      if (fpos != string::npos)
      {
        setup_fail->replace(fpos-1,(nam+" ").size(),"");
      }
      setup_fail_mtx->unlock();
      break;
    }
    setup_fail_mtx->lock();
    size_t fpos = setup_fail->find(nam);
    if (fpos == string::npos)
    {
      setup_fail->append(" "+nam);
    }
    setup_fail_mtx->unlock();

//    printf("node setup failed for %s\n", nam.c_str());
    int session_setup_code = nod->m_ipmiInterface->get_session_setup_code();
    delete nod;
    if (session_setup_code ==SessSup_PingFail)
    {
      sleep(10*config->setupRetryDelay);
    }
    else if (session_setup_code ==SessSup_BadUserName)
    {
      sleep(100*config->setupRetryDelay);
    }
    else
    {
      sleep(config->setupRetryDelay);
    }
  }
  sleep(1);
  while (1)
  {
    status = nod->getPowerState();
    if (status < 0)
    {
      printf(
          "Failed to get power status from node %s\nDeleting and restarting node...\n",
          nam.c_str());
      delete (nod);
      sleep(2);
      while (1)
      {
        nod = new IPMINode(nam, uname, pword);
        status = nod->setup((char*)ServicePrefix->c_str());
        if (status == 0)
        {
//          printf("Setup of node %s successful\n", nam.c_str());
          setup_fail_mtx->lock();
          size_t fpos = setup_fail->find(nam);
          if (fpos != string::npos)
          {
            setup_fail->replace(fpos-1,(nam+" ").size(),"");
          }
          setup_fail_mtx->unlock();
          break;
        }
//        printf("node setup failed for %s\n", nam.c_str());
        int session_setup_code = nod->m_ipmiInterface->get_session_setup_code();
        delete nod;
        setup_fail_mtx->lock();
        size_t fpos = setup_fail->find(nam);
        if (fpos == string::npos)
        {
          setup_fail->append(" "+nam);
        }
        setup_fail_mtx->unlock();
        if (session_setup_code ==SessSup_PingFail)
        {
          sleep(10*config->setupRetryDelay);
        }
        else if (session_setup_code ==SessSup_BadUserName)
        {
          sleep(100*config->setupRetryDelay);
        }
        else
        {
          sleep(config->setupRetryDelay);
        }
      }
      sleep(1);
    }
    if (nod->m_cmd->getNext() == 1)
    {
      char *cmd = nod->m_cmd->getString();
      printf("received command %s for node %s\n", cmd, nod->m_Name.c_str());
      if (strcmp(cmd, "on") == 0)
      {
        nod->turnOn();
      }
      else if (strcmp(cmd, "off") == 0)
      {
        nod->turnOff();
      }
      else if (strcmp(cmd, "cycle") == 0)
      {
        nod->cycle();
      }
      else if (strcmp(cmd, "reset") == 0)
      {
        nod->reset();
      }
      else
      {
        printf("unknown command %s for node %s\n", cmd, nod->m_Name.c_str());
      }
//      status = nod->getPowerState();
//      if (status <0)
//      {
//        delete (nod);
//        nod = new IPMINode(nam,uname,pword);
//        while (1)
//        {
//          status = nod->setup(ServerName);
//          if (status == 0)
//          {
//            break;
//          }
//          printf ("node setup failed for %s\n",nam.c_str());
//          sleep(10);
//        }
//      }
    }
    sleep(config->statePollDelay);
  }
}


