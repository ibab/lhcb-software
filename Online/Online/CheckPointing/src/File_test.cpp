#include "Checkpoining/FileMap.h"
#include "Checkpoining.h"

using namespace Checkpoining;

#include <unistd.h>
#include <fcntl.h>

namespace {
  struct Test : public FileMap {
    int fd1, fd2;
    Test() {
      mtcp_set_debug_level(2);
      fd1=::open("/home/frankm/.bashrc",O_RDONLY);
      fd2=::open("/home/frankm/.bashrc",O_RDONLY|O_APPEND);
    }
    ~Test() {
      ::close(fd1);
      ::close(fd2);
    }
    long memory() {
      MemCountFileHandler h;
      scan(h);
      mtcp_output(MTCP_INFO,"The current file scan requires: %ld bytes.\n",h.count());
      return h.count();
    }
  };
  struct FileReadCheckHandler: public FileReadHandler {
    FileReadCheckHandler(void* p) : FileReadHandler(p, false) {}
    virtual int handle(int) const {
      FileDesc dsc, *pdsc = (FileDesc*)(m_ptr+sizeof(Marker));
      m_ptr += dsc.read(m_ptr, false);
      dsc.print();
      pdsc->print();
      ++m_now;
      return m_now>= m_count ? 0 : 1;
    }
  };
}

int test_FileMap_scan() {
  return Test().scan(FilePrintHandler());
}

int test_FileMap_memory() {
  return Test().memory();
}
