// $Id: RunDB.h,v 1.1 2008-03-11 12:39:11 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Structure of basic items published by the readout
//               monitoring.
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/RunDB.h,v 1.1 2008-03-11 12:39:11 frankb Exp $
#ifndef ROMON_RUNDB_H
#define ROMON_RUNDB_H 1

// ++ include files
#include <ctime>
#include "ROMon/Collections.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {
  class File {
  public:
    int            id;
    char           name[64];
    char           location[128];
    time_t         created;      
    int            events;
    long long int  bytes;
    unsigned int   adler32;
    unsigned char  md5[16];
    unsigned char  state;
    unsigned char  pad[3];
    File();
    File*          reset();
    long   sizeOf() const {  return sizeof(File); }
  };

  class Run {
  public:
    typedef Online::FixItems<File> Files;
    int    number;
    int    partitionID;
    char   partition[32];
    char   runType[32];
    time_t startTime;
    time_t endTime;
    float  startLumi;
    float  endLumi;
    int    fill;
    int    state;
    Files  files;
    Run();
    Run*   reset();
    int    length() const {  return sizeof(Run)+files.data_length(); }
    long   sizeOf() const {  return sizeof(Run); }
  };
  class RunDB {
  public:
    typedef Online::VarItems<Run> Runs;
    enum { TYPE = 3 };
    /// First word: Data type descriptor (MUST always be 3)
    int  type;
    time_t  readTime;
    int     totalSize;
    Runs    runs;

    RunDB*  reset();
    void    fixup();
    int     length()  const {  return sizeof(RunDB)+runs.data_length(); }
    static RunDB* createBuffer();
    void    dump();
  };

  class ROMonGblBuffer;
  class RunDBCollector {
  private:
    ROMonGblBuffer* m_gbl;
  private:
    /// Private copy constructor
    RunDBCollector(const RunDBCollector&) {}
    /// Private assignment operator
    RunDBCollector& operator=(const RunDBCollector&) { return *this; }
  public:
    RunDBCollector();
    ~RunDBCollector();
    
    RunDB*  data()   const;
    int     beginUpdate();
    int     endUpdate();
  };

  std::ostream& operator<<(std::ostream& os, const File& f); 
  std::ostream& operator<<(std::ostream& os, const Run& f); 
  std::ostream& operator<<(std::ostream& os, const RunDB& f); 
}
#endif /* ROMON_RUNDB_H */

