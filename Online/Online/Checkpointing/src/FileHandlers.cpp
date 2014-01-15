#include "Checkpointing/FileMap.h"
#include "Checkpointing.h"
using namespace Checkpointing;
#include <unistd.h>

#ifndef __STATIC__
DefineMarker(FILEMAP_BEGIN_MARKER,"FMAP");
DefineMarker(FILEMAP_END_MARKER,  "fmap");
#endif

/// Handle single file entry
WEAK(int) FilePrintHandler::handle(int fdnum)  const {
  FileDesc dsc;
  if ( 1 == dsc.setup(fdnum) ) {
    dsc.print(MTCP_INFO);
  }
  return 1;
}

/// Handle single file entry
WEAK(int) MemCountFileHandler::handle(int fdnum)  const {
  FileDesc dsc;
  if ( 1 == dsc.setup(fdnum) ) {
    m_count += dsc.length();
    if ( dsc.hasData ) m_count += dsc.dataLength();
    m_count += 2*sizeof(Marker)+sizeof(int);
    return m_count;
  }
  return -1;
}

/// Start the write cycle
WEAK(FileWriteHandler&) FileWriteHandler::start()  {
  m_count  = 0;
  m_bytes  += writeMarker(m_fd,FILEMAP_BEGIN_MARKER);
  m_offset = ::lseek(m_fd,0,SEEK_CUR);
  m_bytes  += writeInt(m_fd,0);
  return *this;
}

/// Stop and finish the write cycle
WEAK(int) FileWriteHandler::stop()  {
  off_t off = ::lseek(m_fd,0,SEEK_CUR);
  if ( off != (off_t)-1 )  {
    ::lseek(m_fd,m_offset,SEEK_SET);
    writeInt(m_fd,m_count);
    ::lseek(m_fd,off,SEEK_SET);
    m_bytes += writeMarker(m_fd,FILEMAP_END_MARKER);
    return m_bytes;
  }
  return 0;
}

/// Handle single file entry
WEAK(int) FileWriteHandler::handle(int fdnum) const {
  FileDesc dsc;
  if ( 1 == dsc.setup(fdnum) ) {
    long n = dsc.write(m_fd);
    if ( n > 0 ) {
      ++m_count;
      m_bytes += n;
      mtcp_output(MTCP_DEBUG,"FileMap: read %ld bytes current position:%p\n",n,m_offset+m_bytes);
      return 1;
    }
    mtcp_output(MTCP_ERROR,"FileMap: error saving file information for FD:%d -> %s\n",fdnum,dsc.name);
    return 0;
  }
  mtcp_output(MTCP_ERROR,"FileMap: accessing file information for FD:%d\n",fdnum);
  return 0;
}

/// Standard constructor
FileStreamOutHandler::FileStreamOutHandler(void* add) 
  : m_addr((char*)add), m_ptr((char*)add), m_count(0) {
}

/// Start the write cycle
WEAK(FileStreamOutHandler&) FileStreamOutHandler::start()  {
  m_count  = 0;
  m_ptr  += saveMarker(m_ptr,FILEMAP_BEGIN_MARKER);
  m_ptr  += saveInt(m_ptr,0);
  return *this;
}

/// Stop and finish the write cycle
WEAK(int) FileStreamOutHandler::stop()  {
  saveInt(m_addr + sizeof(Marker),m_count);
  m_ptr += saveMarker(m_ptr,FILEMAP_END_MARKER);
  return addr_diff(m_ptr,m_addr);
}

/// Handle single file entry
WEAK(int) FileStreamOutHandler::handle(int fdnum) const {
  FileDesc dsc;
  if ( 1 == dsc.setup(fdnum) ) {
    long n = dsc.streamOut(m_ptr);
    if ( n > 0 ) {
      ++m_count;
      m_ptr += n;
      mtcp_output(MTCP_DEBUG,"FileMap: read %ld bytes current position:%p\n",n,m_ptr);
      return 1;
    }
    mtcp_output(MTCP_ERROR,"FileMap: error saving file information for %s\n",dsc.name);
    return 0;
  }
  mtcp_output(MTCP_ERROR,"FileMap: accessing file information for FD:%d\n",fdnum);
  return 0;
}

/// Start the read cycle
WEAK(FileReadHandler&) FileReadHandler::start()  {
  m_ptr   = m_addr;
  m_ptr  += checkMarker(m_ptr,FILEMAP_BEGIN_MARKER);
  m_count = *(int*)m_ptr;
  m_ptr  += sizeof(Marker);
  return *this;
}

/// Stop and finish the read cycle
WEAK(int) FileReadHandler::stop()  {
  m_ptr += checkMarker(m_ptr,FILEMAP_END_MARKER);
  return addr_diff(m_ptr,m_addr)>0 ? 1 : 0;
}

/// Handle single file entry
WEAK(int) FileReadHandler::handle(int) const {
  FileDesc dsc;
  m_ptr += dsc.read(m_ptr, m_restore);
  ++m_now;
  return m_now>= m_count ? 0 : 1;
}

/// Handle single file entry
WEAK(int) FileMemPrintHandler::handle(int) const {
  FileDesc *pdsc = (FileDesc*)(m_ptr+=sizeof(Marker));
  m_ptr += pdsc->length() + sizeof(Marker) + (pdsc->hasData ? pdsc->dataLength() : 0);
  pdsc->print(MTCP_INFO);
  ++m_now;
  return m_now>= m_count ? 0 : 1;
}

/// Handle single file entry
WEAK(int) FileMapper::handle(int) const {
  FileDesc dsc;
  m_ptr += dsc.read(m_ptr, m_restore);
  if ( m_print ) dsc.print(MTCP_INFO);
  ++m_now;
  return m_now>= m_count ? 0 : 1;
}

