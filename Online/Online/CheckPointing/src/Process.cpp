#include "CheckPointing/Static.h"
#include "CheckPointing/Process.h"
#include "CheckPointing/MemMaps.h"
#include "CheckPointing/FileMap.h"
#include "CheckPointing.h"

#include <cerrno>
#include <cstring>

using namespace CheckPointing;

static const int PROCESS_BEGIN_MARKER = *(Marker*)"PROC";
static const int PROCESS_END_MARKER   = *(Marker*)"proc";

/// Calculate the length of the data to be stored
long Process::length() const {
  FileMap f;
  MemMaps m;
  AreaBaseHandler b;
  MemCountFileHandler h;
  long s1 = m.scan(b), s2 = f.scan(h);

  if ( s1 > 0 && s2 > 0 ) {
    long len = chkpt_sys.length() + b.space() + h.count() + 2*sizeof(Marker) + sizeof(long);
    return len;
  }
  if ( s1<=0 ) mtcp_output(MTCP_ERROR,"Memory map scan failed:%s.\n",strerror(errno));
  if ( s2<=0 ) mtcp_output(MTCP_ERROR,"File entry scan failed:%s.\n",strerror(errno));
  return 0;
}

/// Write full process information to checkpoint file
int Process::write(void* addr)  const {
  long rc = 0;
  if ( addr != 0 ) {
    Pointer out = (Pointer)addr;
    if ( (rc=writeHeader(out)) ) {
      mtcp_output(MTCP_INFO,"Wrote %ld bytes of process header.\n",rc);
      if ( (rc=writeSysInfo(out+=rc)) ) {
	mtcp_output(MTCP_INFO,"Wrote %ld bytes of process sysinfo.\n",rc);
	if ( (rc=writeFiles(out+=rc)) ) {
	  mtcp_output(MTCP_INFO,"Wrote %ld bytes of process file info.\n",rc);
	  if ( (rc=writeMemory(out+=rc)) ) {
	    mtcp_output(MTCP_INFO,"Wrote %ld bytes of process memory.\n",rc);
	    if ( (rc=writeTrailer(out+=rc)) ) {
	      mtcp_output(MTCP_INFO,"Wrote %ld bytes of process trailer.\n",rc);
	      out+= rc;
	      return addr_diff(out,addr);
	    }
	  }
	}
      }
    }
  }
  return 0;
}

/// Read full process information from checkpoint file
int Process::read(const void* addr)  const {
  long rc = 0;
  if ( addr != 0 ) {
    const_Pointer out = (const_Pointer)addr;
    if ( (rc=readHeader(out)) ) {
      mtcp_output(MTCP_INFO,"Read %ld bytes of process header.\n",rc);
      if ( (rc=readSysInfo(out+=rc)) ) {
	SysInfo& sys = chkpt_sys;
	mtcp_output(MTCP_INFO,"Read %ld bytes of process sysinfo.\n",rc);
	if ( m_flag == PROCESS_RESTORE )   {
	  const void* a[2] = {this,out+rc};
	  sys.restore((void*)a);
	}
	if ( (rc=readFiles(out+=rc)) ) {
	  mtcp_output(MTCP_INFO,"Read %ld bytes of process file info.\n",rc);
	  if ( (rc=readMemory(out+=rc)) ) {
	    mtcp_output(MTCP_INFO,"Read %ld bytes of process memory.\n",rc);
	    if ( (rc=readTrailer(out+=rc)) ) {
	      mtcp_output(MTCP_INFO,"Read %ld bytes of process trailer.\n",rc);
	      out+= rc;
	      return addr_diff(out,addr);
	    }
	  }
	}
      }
    }
  }
  return 0;
}

/// Write header information to checkpoint file
int Process::writeHeader(void* addr)  const {
  Pointer ptr = Pointer(addr);
  if ( ptr ) {
    ptr += saveMarker(ptr,PROCESS_BEGIN_MARKER);
    ptr += saveLong(ptr,m_length);
  }
  return addr_diff(ptr,addr);
}

/// Read header information from checkpoint file
int Process::readHeader(const void* addr)  const {
  const_Pointer ptr = (const_Pointer)addr;
  if ( ptr ) {
    ptr += checkMarker(ptr,PROCESS_BEGIN_MARKER);
    ptr += getLong(ptr,&m_length);
  }
  return addr_diff(ptr,addr);
}

/// Write trailer information to checkpoint file
int Process::writeTrailer(void* addr)  const {
  size_t sz = saveMarker(addr,PROCESS_END_MARKER);
  return sz;
}

/// Read trailer information from checkpoint file
int Process::readTrailer(const void* addr)  const {
  size_t sz = checkMarker(addr,PROCESS_END_MARKER);
  return sz;
}

/// Write system information to checkpoint file
int Process::writeSysInfo(void* addr)  const {
  SysInfo& sys = chkpt_sys;
  sys.aquire();
  sys.print();
  long len = sys.write(addr);
  if ( len ) {
    mtcp_output(MTCP_INFO,"Wrote %ld bytes of System information.\n",len);
    return len;
  }
  mtcp_output(MTCP_ERROR,"Failed to write System information!\n");
  return 0;
}

/// Read system information from checkpoint file
int Process::readSysInfo(const void* addr)  const {
  SysInfo& sys = chkpt_sys;
  sys.aquire();
  sys.print();
  long len = sys.read(addr);
  if ( len ) {
    mtcp_output(MTCP_INFO,"Read %ld bytes of System information.\n",len);
    return len;
  }
  mtcp_output(MTCP_ERROR,"Failed to read System information!\n");
  return 0;
}

/// Write the file descriptor information to checkpoint file
int Process::writeFiles(void* addr)  const {
  FileMap f;
  FileWriteHandler wr(addr);
  long len = f.scan(wr.start());
  wr.stop();
  if ( len ) {
    mtcp_output(MTCP_INFO,"Wrote %ld bytes of information for %ld files.\n",wr.bytes(),len);
    return wr.bytes();
  }
  mtcp_output(MTCP_ERROR,"Failed to write file information!\n");
  return 0;
}

/// Read the file descriptor information to checkpoint file
int Process::readFiles(const void* addr)  const {
  FileMap f;
  FileMapper rd(addr,true,m_flag == PROCESS_RESTORE);
  long len = f.analyze(rd.start());
  rd.stop();
  if ( len ) {
    mtcp_output(MTCP_INFO,"Read %ld bytes of information for %ld files.\n",rd.bytes(),len);
    return rd.bytes();
  }
  mtcp_output(MTCP_ERROR,"Failed to read file information!\n");
  return 0;
}

/// Write the memory areas to checkpoint file
int Process::writeMemory(void* addr)  const {
  MemMaps m;
  long len = m.write(addr);
  if ( len ) {
    mtcp_output(MTCP_INFO,"Wrote %ld bytes of memory dump.\n",len);
    return len;
  }
  mtcp_output(MTCP_ERROR,"Failed to write memory dump!\n");
  return 0;
}

/// Read the memory areas to checkpoint file
int Process::readMemory(const void* addr)  const {
  MemMaps m;
  AreaMapper mapper;
  long len = m.read(addr,mapper);
  if ( len ) {
    mtcp_output(MTCP_INFO,"Wrote %ld bytes of memory dump.\n",len);
    return len;
  }
  mtcp_output(MTCP_ERROR,"Failed to read memory dump!\n");
  return 0;
}
