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
    printf ("Counter %s at address %llx data ",hsts[i].c_str(), (long long unsigned int)objs[i]);
    switch (objs[i]->type)
    {
      case   C_INT:
      {
        printf ("(INT) %d\n",objs[i]->i_data);
        break;
      }
      case C_LONGLONG:
      {
        printf ("(LONGLONG) %lld\n",objs[i]->l_data);
        break;

      }
      case C_FLOAT:
      {
        printf ("(FLOAT) %f\n",objs[i]->f_data);
        break;
      }
      case C_DOUBLE:
      {
        printf ("(DOUBLE) %f\n",objs[i]->d_data);
        break;
      }
      case C_RATEDOUBLE:
      {
        printf ("(RATEDOUBLE) %f Hz\n",objs[i]->d_data);
        break;
      }
      case C_RATEFLOAT:
      {
        printf ("(RATEFLOAT) %f Hz\n",objs[i]->f_data);
        break;
      }
      case C_INTSTAR:
      {
        int *p = (int*)objs[i]->ptr;
        int nel = objs[i]->nel;
        for (int j=0;j<nel;j++)
        {
          printf("(INT*) index %d value %d\n",j,p[j]);
        }
        break;
      }
      case C_LONGSTAR:
      {
        long *p = (long*)objs[i]->ptr;
        int nel = objs[i]->nel;
        for (int j=0;j<nel;j++)
        {
          printf("(LONG*) index %d value %li\n",j,p[j]);
        }
        break;
      }
      case C_FLOATSTAR:
      {
        float *p = (float*)objs[i]->ptr;
        int nel = objs[i]->nel;
        for (int j=0;j<nel;j++)
        {
          printf("(FLOAT*) index %d value %f\n",j,p[j]);
        }
        break;
      }
      case C_DOUBLESTAR:
      case C_RATEDOUBLESTAR:
      {
        double *pp = (double*)(objs[i]->ptr);
        int nel = objs[i]->nel;
        for (int j=0;j<nel;j++)
        {
          double p = pp[j];
          printf("(DOUBLE*) index %d value %f\n",j,p);
        }
        break;
      }
    }
  }
  return 0;
}
