#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "ROMon/RunDB.h"
#include "ROMon/ROMonGblBuffer.h"

using namespace ROMon;

std::ostream& operator<<(std::ostream& os, const File& f) {
  os << "   File:"  << f.id
     << " Name:"      << f.location << "/" << f.name << std::endl
     << " Created: "  << std::setw(8)  << f.created
     << " Events:  "  << std::setw(8)  << f.events
     << " Bytes:   "  << std::setw(12) << f.bytes
     << " Adler32: "  << std::setw(8)  << f.adler32 << std::endl
     << " MD5:";
  for(size_t i=0; i<16; ++i) os << f.md5[i];
  os << " State:" << int(f.state);
  os << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const Run& r) {
  os << "   Run:"  << r.number
     << " PartID:" << std::setw(8) << r.partitionID
     << " [" << r.partition << "] :" << r.runType
     << " From:   " << r.startTime << " -> " << r.endTime << std::endl
     << " Lumi:   " << std::setw(8)  << r.startLumi << " " << r.endLumi
     << " Fill:   " << std::setw(8)  << r.fill 
     << " State:  " << std::setw(12) << int(r.state) << std::endl
     << "Files:   " << std::endl
     << r.files << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const RunDB& db) {
  os << " RunDB:"  << db.readTime << " " << db.totalSize
     << " Runs: "  << std::endl
     << db.runs
     << std::endl;
  return os;
}

File::File() {
  reset();
}

File* File::reset() {
  ::memset(this,0,sizeof(File));
  return this;
}

Run::Run() {
  reset();
}

Run* Run::reset() {
  ::memset(this,0,sizeof(Run));
  return this;
}

RunDB* RunDB::createBuffer() {
  RunDB* rdb = (RunDB*)malloc(1024*1024);
  rdb->reset();
  return rdb;
}

RunDB* RunDB::reset() {
  ::memset(this,0,sizeof(RunDB));  
  type = TYPE;
  return this;
}

void RunDB::fixup() {
  type = TYPE;
  totalSize = length();
}

void RunDB::dump() {
  std::cout << *this;
}


RunDBCollector::RunDBCollector() {
  m_gbl = new ROMonGblBuffer();
  m_gbl->setSectionName("RunDBdisplay");
  m_gbl->setMapOnly(false);
  m_gbl->setSectionSize(1024);
  m_gbl->initialize();
}

RunDBCollector::~RunDBCollector() {
  if ( m_gbl ) delete m_gbl;
  m_gbl = 0;
}

int RunDBCollector::beginUpdate() {
  return 0;
}

int RunDBCollector::endUpdate() {
  return 0;
}

RunDB* RunDBCollector::data() const {
  return (RunDB*)(m_gbl ? m_gbl->data() : 0);
}

