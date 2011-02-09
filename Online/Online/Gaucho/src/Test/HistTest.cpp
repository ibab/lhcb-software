/*
 * HistTest.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: beat
 */

#include "Gaucho/HistTask.h"

int main(int argc, char *argv[]) // Taskname, DNS Node
{
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
  return 0;
}
