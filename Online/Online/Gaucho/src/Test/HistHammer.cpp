/*
 * HistTest.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: beat
 */

#include "Gaucho/HistTask.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"

int main(int , char *argv[]) // Taskname, DNS Node
{
  TH1D::SetDefaultSumw2();
  TH2D::SetDefaultSumw2();
  TProfile::SetDefaultSumw2();
  HistTask *h;
  std::string task;
  std::string dns;
  task = std::string(argv[1]);
  dns = std::string(argv[2]);
  std::vector<std::string> tasks;
  tasks.clear();
  HistTask::TaskList(dns,tasks);
  for (unsigned int i=0;i<tasks.size();i++)
  {
    printf ("%s\n",tasks[i].c_str());
  }
  h = new HistTask(task,dns);
  std::vector<std::string> hists;
  while (1)
  {
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
  }
  return 0;
}
