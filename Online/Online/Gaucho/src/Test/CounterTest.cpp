/*
 * HistTest.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: beat
 */

#include "Gaucho/CounterTask.h"
#include "Gaucho/MonCounter.h"
int main(int , char *argv[]) // Taskname, DNS Node
{
  CounterTask *h;
  std::string task;
  std::string dns;
  task = std::string(argv[1]);
  dns = std::string(argv[2]);
  std::vector<std::string> tasks;
  tasks.clear();
  CounterTask::TaskList(dns,tasks);
  for (unsigned int i=0;i<tasks.size();i++)
  {
    printf ("%s\n",tasks[i].c_str());
  }
  h = new CounterTask(task,dns);
  std::vector<std::string> hists;
  hists.clear();
  int status = h->Directory(hists);
  if (status == 1)
  {
    printf("Task does not exist...\n");
    return 1;
  }
  for (unsigned int i=0;i<hists.size();i++)
  {
    printf ("%s\n",hists[i].c_str());
  }
  printf("Number of Histograms for Task %s %d\n",task.c_str(),(int)hists.size());
  std::vector<std::string> hsts;
  printf("Trying to retrieve the following Counters\n ");
  for (unsigned int i=0;i<hists.size();i++)
  {
    hsts.push_back(hists[i]);
    printf("%s\n",hsts[i].c_str());
  }
  std::vector<CntrDescr*> objs;
  h->Counters(hsts,objs);
  printf ("Retrieved %d Counters\n", (int)objs.size());
  for (unsigned int i=0;i<objs.size();i++)
  {
    printf ("Counter %s at address %llx data \n",hsts[i].c_str(), (long long unsigned int)objs[i]);
    switch (objs[i]->type)
    {
      case   C_INT:
      {
        printf ("%d\n",(int)objs[i]->data);
        break;
      }
      case C_LONGLONG:
      {
        printf ("%lld\n",(long long int)objs[i]->data);
        break;

      }
      case C_FLOAT:
      {
        printf ("%f\n",(float)objs[i]->data);
        break;
      }
      case C_DOUBLE:
      {
        printf ("%f\n",(float)objs[i]->data);
        break;
      }
      case C_RATE:
      {
        printf ("%f Hz\n",(double)objs[i]->data);
        break;
      }
    }
  }
  return 0;
}
