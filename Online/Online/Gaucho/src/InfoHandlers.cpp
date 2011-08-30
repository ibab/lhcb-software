/*
 * InfoHandlers.cpp
 *
 *  Created on: Mar 1, 2011
 *      Author: beat
 */
#include "Gaucho/InfoHandlers.h"
#include "Gaucho/AdderSys.h"
#include "Gaucho/MonAdder.h"
#include "Gaucho/Utilities.h"

#include <memory>

using namespace std;

void HAdderServInfoHandler::infoHandler(void)
{
  bool Newe = true;
  char *input = itsService->getString();
  string taskname = itsService->getName();

  if (strcmp(input, "DEAD") == 0)
  {
    printf("Service Info Handler from %s DEAD\n",taskname.c_str());
    Adderlist_t::iterator adder;
    Adderlist_t& l = AdderSys::Instance().gg_AdderList;
    taskname = taskname.substr(0, taskname.find("/SERVICE_LIST"));
    for (adder = l.begin(); adder != l.end(); adder++)
    {
      (*adder)->TaskDied(taskname);
    }
    return;
  }
//  if (RTL::nodeNameShort().length()<=6)
  {
    printf("New Message from Service List Service %s data:\n%s\n==================\n",itsService->getName(),input);
  }
  if ((input[0] == '+') || (input[0] == '-') || (input[0] == '!'))
  {
    Newe = (input[0] == '+');
    ++input;
  }

  Adderlist_t& l = AdderSys::Instance().gg_AdderList;
  auto_ptr<dyn_string> service_list(Strsplit(input,"\n"));
  taskname = taskname.substr(0, taskname.find("/SERVICE_LIST"));
  for (unsigned int j = 0; j < service_list->size(); j++)
  {
    string service_line(service_list->at(j));
    auto_ptr<dyn_string> service(Strsplit((char*)service_line.c_str(),"|"));
    if (Newe)
    {
      for (Adderlist_t::iterator i = l.begin(); i != l.end(); i++)
      {
        (*i)->NewService(itsService, taskname, service->at(0));
      }
    }
    else
    {
      for (Adderlist_t::iterator i = l.begin(); i != l.end(); i++)
      {
        (*i)->RemovedService(itsService, taskname, service->at(0));
      }
    }
  }
}

void HAdderTaskInfoHandler::infoHandler(void)
{
  bool Newe = true;
  const char* input = itsService->getString();

  if (strcmp(input, "DEAD") == 0)
  {
    printf("Task Info Handler from DNS DEAD\n");
    return;
  }
  if ((input[0] == '+') || (input[0] == '-') || (input[0] == '!'))
  {
    Newe = (input[0] == '+');
    ++input;
  }

  Adderlist_t& l = AdderSys::Instance().gg_AdderList;
  auto_ptr<dyn_string> server_list(Strsplit(input,"|"));
  for (dyn_string::iterator i = server_list->begin(); i != server_list->end(); i++)
  {
    bool fnd = false;
    Adderlist_t::iterator adder;
    string tskname, tgen, servc, tname, server = *i;
    server = server.substr(0,server.find("@"));

    for (adder = l.begin(); adder != l.end(); adder++)
    {
//      printf ("Checking Server Name %s...",server.c_str());
      (*adder)->TaskName(server, tskname, tgen);
      if (tskname != "")
      {
//        printf("... Found\n");
        tname = tskname;
        fnd = true;
      }
      else
      {
//        printf("...NOT Found...\n");
      }
    }
    if (fnd)
    {
      if (Newe)
      {
        TaskIter i = m_TaskMap.find(tname);
        if (i == m_TaskMap.end())
        {
          servc = tname + string("/SERVICE_LIST");
          HAdderServInfoHandler *shand = new HAdderServInfoHandler();
          DimInfo *slist = new DimInfo(servc.c_str(), (char *) "DEAD",(DimInfoHandler*)shand);
          m_TaskMap.insert(TaskPair(tname, slist));
        }
      }
      else
      {
        TaskIter i = m_TaskMap.find(tname);
        if (i != m_TaskMap.end())
        {
          delete i->second->itsHandler;
          delete i->second;
          m_TaskMap.erase(i);
        }
        for (adder = l.begin(); adder != l.end(); adder++)
        {
          (*adder)->TaskDied(tname);
        }
      }
    }
  }
}

