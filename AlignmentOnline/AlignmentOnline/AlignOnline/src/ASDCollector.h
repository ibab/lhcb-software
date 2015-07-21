/*
 * ASDCollector.h
 *
 *  Created on: Oct 6, 2014
 *      Author: beat
 */

#ifndef ASDCOLLECTOR_H_
#define ASDCOLLECTOR_H_
#include "AlignKernel/AlEquations.h"
#include <string>
#include <vector>
#include "time.h"

class ASDCollector
{
  public:
    std::string m_dir;
    std::string m_filePatt;
    time_t m_time;
    ASDCollector() {}
    ASDCollector(std::string &FilePatt, std::string &dir);
    void setTime(time_t t);
    void setTime();
    void collectASDs(Al::Equations &eqs);
    int getfiles(std::string dir, std::vector<std::string> &fileList);
    int isDir(std::string path);
};

#endif /* ASDCOLLECTOR_H_ */
