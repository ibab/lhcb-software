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
#include "AlignKernel/AlEquations.h"
#include <string.h>
#include <string>
#include <vector>
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
	for (size_t i=0;i<fileList.size();i++)
	{
		stat(fileList[i].c_str(),&Statres);
		if (Statres.st_mtime >m_time)
		{
			Al::Equations tmp(0);
			tmp.readFromFile(fileList[i].c_str());
			eqs.add(tmp);
		}
	}
	return;
}


