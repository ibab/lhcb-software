/*
 * ASDCollector.cpp
 *
 *  Created on: Oct 6, 2014
 *      Author: beat
 */

#include "ASDCollector.h"
#include <iostream>
#include <sys/stat.h> /* for stat() */
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
using namespace std;

ASDCollector::ASDCollector(std::string &FilePatt, std::string &dir)
{
  m_time = 0;
  m_dir = dir;
  m_filePatt = FilePatt;
}
void ASDCollector::setTime(time_t t)
{
  m_time = t;
}
void ASDCollector::setTime()
{
  m_time = time(0);
}
int ASDCollector::getfiles(string dir, vector<string> &fileList)
{
  DIR *dp;
  struct dirent *dirp;
  fileList.clear();
  if ((dp = opendir(dir.c_str())) == NULL)
  {
    return 1;
  }

  while ((dirp = readdir(dp)) != NULL)
  {
    if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0)
    {
      //dirlist.push_back(string(dirp->d_name));

      string Tmp = dir.c_str() + string("/") + string(dirp->d_name);
      if (isDir(Tmp))
      {
        continue;
      }
      else
      {
        //              cout << "Files :"<<dirp->d_name << endl;
        fileList.push_back(string(Tmp));
      }

    }
  }
  closedir(dp);
  return (int)fileList.size();
}

int ASDCollector::isDir(string path)
{
  struct stat stat_buf;
  stat(path.c_str(), &stat_buf);
  int is_dir = S_ISDIR(stat_buf.st_mode);
//    cout <<"isDir :Path "<<path.c_str()<<endl;
  return (is_dir ? 1 : 0);
}

void ASDCollector::collectASDs( Al::Equations &eqs)
{
  std::vector<string> fileList;
  getfiles(m_dir, fileList);
  struct stat Statres;
  printf("------------> Collecting ASDs from %ld Files\n",fileList.size());
  for (size_t i=0;i<fileList.size();i++)
    {
      stat(fileList[i].c_str(),&Statres);
      if (Statres.st_mtime >m_time)
	{
	  Al::Equations tmp(0);
	  tmp.readFromFile(fileList[i].c_str());
	  if(eqs.nElem()==0)
	    eqs = tmp;
	  else
	    eqs.add(tmp);
	  unlink(fileList[i].c_str()); // delete file     
	}
    }
  return;
}


 
