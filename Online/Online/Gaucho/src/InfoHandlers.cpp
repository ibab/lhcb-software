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
void HAdderServInfoHandler::infoHandler(void)
{
  char *input = (char*)itsService->getData();
  if (strcmp(input, "DEAD") == 0)
  {
    return;
  }
//  printf("New Message from Service List Service %s data:\n%s\n",itsService->getName(),input);
  bool Newe;
  Newe = false;
  char *sinput = input;
  if ((input[0] == '+') || (input[0] == '-') || (input[0] == '!'))
  {
    Newe = (input[0] == '+');
    sinput = &input[1];
  }
  else
  {
    Newe = true;
  }
  dyn_string *service_list = Strsplit(sinput, (char*) "\n");
  std::string taskname = std::string(itsService->getName());
  taskname = taskname.substr(0, taskname.find("/SERVICE_LIST"));
//  printf("infoHANDLER: TaskName %s input: %s\n",taskname.c_str(),input);
  for (unsigned int j = 0; j < service_list->size(); j++)
  {
    std::string service_line(service_list->at(j));
    dyn_string *service = Strsplit((char*) service_line.c_str(), (char*) "|");
    //    printf("comparing %s to pattern %s\n",service->at(0).c_str(),this->m_servicePattern.c_str());
    if (Newe)
    {
      Adderlist_t::iterator i;
      for (i = AdderSys::Instance().gg_AdderList.begin(); i
          != AdderSys::Instance().gg_AdderList.end(); i++)
      {
        MonAdder *p;
        p = *i;
        p->NewService(itsService, taskname, service->at(0));
      }
    }
    else
    {
      Adderlist_t::iterator i;
//      printf("Removed Service %s\n",service->at(0).c_str());
      for (i = AdderSys::Instance().gg_AdderList.begin(); i
          != AdderSys::Instance().gg_AdderList.end(); i++)
      {
        MonAdder *p;
        p = *i;
        p->RemovedService(itsService, taskname, service->at(0));
      }
    }
  }
}
void HAdderTaskInfoHandler::infoHandler(void)
{
  char *input;
  int siz;
  input = (char*) this->itsService->getData();
  siz = this->itsService->getSize();
  //  printf("Service Info Handler %s. Input :\n%s\n",this->itsService->getName(),input);
  if (strcmp(input, "DEAD") == 0)
  {
    //    printf("DNS Down!!!!\n");
    return;
  }
  //  printf("New Message from DNS %s\n",input);
  bool Newe;
  Newe = true;
  char *sinput = input;
  if ((input[0] == '+') || (input[0] == '-') || (input[0] == '!'))
  {
    Newe = (input[0] == '+');
    sinput = &input[1];
  }
//  printf("HAdderTaskInfoHandler input %s\n", input);
  dyn_string *server_list = Strsplit(sinput, (char*) "|");
  dyn_string::iterator i;
  for (i = server_list->begin(); i != server_list->end(); i++)
  {
    std::string server = *i;
    server = server.substr(0,server.find("@"));
    std::string tskname, tgen;
    std::string servc;
    std::string tname;
    bool fnd = false;
    Adderlist_t::iterator adder;
    for (adder = AdderSys::Instance().gg_AdderList.begin(); adder
        != AdderSys::Instance().gg_AdderList.end(); adder++)
    {
      (*adder)->TaskName(server, tskname, tgen);
      if (tskname != "")
      {
        tname = tskname;
        fnd = true;
      }
    }
    if (fnd)
    {
      if (Newe)
      {
        TaskIter i;
        i = m_TaskMap.find(tname);
        if (i == m_TaskMap.end())
        {
          servc = tname + std::string("/SERVICE_LIST");
          HAdderServInfoHandler *shand = new HAdderServInfoHandler();
          DimInfo *slist = new DimInfo(servc.c_str(), (char *) "DEAD",
              (DimInfoHandler*)shand);
          //            TaskSvcDescr *ts = new TaskSvcDescr(tskname,slist);
          m_TaskMap.insert(TaskPair(tname, slist));
//          printf("Listening to Task %s\n",server.c_str());
        }
        else
        {
          //          printf("Duplicate Task %s. DNS restart? \n",tskname.c_str());
        }
      }
      else
      {
        TaskIter i;
        i = m_TaskMap.find(tname);
        if (i != m_TaskMap.end())
        {
          std::string service;
          INServIter k, l;
          delete i->second->itsHandler;
          delete i->second;
          m_TaskMap.erase(i);
        }
        else
        {
          //        printf("Task %s not found in map\n",tskname.c_str());
        }
        Adderlist_t::iterator adder;
        for (adder = AdderSys::Instance().gg_AdderList.begin(); adder
            != AdderSys::Instance().gg_AdderList.end(); adder++)
        {
          (*adder)->TaskDied(tname);
        }
      }
    }
  }
}

