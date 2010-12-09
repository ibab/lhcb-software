#ifndef FILEDESCRIPTORS_H
#define FILEDESCRIPTORS_H

#include <vector>
#include <dirent.h>
#include <sys/stat.h>

class ForkProcess {
 public:
  struct FileDescriptor {
    struct stat statbuf;
    int    fdnum;
    off_t  offset;
    char   linkbuf[PATH_MAX];
    void*  tmpbuf;
    bool   tmpdel;
  };
  typedef std::vector<FileDescriptor> FileDescriptors;

  FileDescriptors m_descriptors;

  ForkProcess() {}
  virtual ~ForkProcess() {}
  static int getFileDescriptors(int fd, FileDescriptors& descrs);
  static int setFileDescriptors(const FileDescriptors& descrs);
};

#endif // FILEDESCRIPTORS_H
