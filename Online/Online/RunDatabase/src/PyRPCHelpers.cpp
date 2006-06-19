#include "RunDatabase/IRunDatabaseReader.h"
#include "PyRPC.h"

typedef LHCb::IRunDatabaseReader RDB;

// Specializations of read methods for generic template 
// PyRpc::ObjectReader
namespace PyRPC  {

  Reader<RDB::Param::DATA>::Result Reader<RDB::Param::DATA>::next()  {
    Result r(new Result::element_type);
    const char* tok = m_rdr.next();
    for(size_t cnt=0; tok && cnt<4; ++cnt, tok=m_rdr.next()) {
      switch(cnt) {
        case 0:    r->ID = atol(tok);      break;
        case 1:    r->Name = tok;          break;
        case 2:    r->Value = tok;         break;
        case 3:    r->Type = tok;          
          return r;
      }
    }
    return Result(0);
  }

  Reader<RDB::Run::DATA>::Result Reader<RDB::Run::DATA>::next()  {
    Result r(new Result::element_type);
    const char* tok = m_rdr.next();
    for(size_t cnt=0; tok && cnt<9; ++cnt, tok=m_rdr.next()) {
      switch(cnt) {
        // Select reads in this order:
        // RunNumber,FillNumber,Partition,Activity,
        // StartDate,EndDate,ProgramName,ProgramVersion,IntegratedLumi
        case 0:    r->RunNumber       = atol(tok);   break;
        case 1:    r->FillNumber      = atol(tok);   break;
        case 2:    r->Partition       = tok;         break;
        case 3:    r->Activity        = tok;         break;
        case 4:    r->StartTime       = atol(tok);   break;
        case 5:    r->EndTime         = atol(tok);   break;
        case 6:    r->ProgramName     = tok;         break;
        case 7:    r->ProgramVersion  = tok;         break;
        case 8:    r->IntegLumi       = atof(tok);
          return r;
      }
    }
    return Result(0);
  }

  Reader<RDB::File::DATA>::Result Reader<RDB::File::DATA>::next()  {
    Result r(new Result::element_type);
    const char* tok = m_rdr.next();
    for(size_t cnt=0; tok && cnt<12; ++cnt, tok=m_rdr.next()) {
      switch(cnt) {
        // Select reads in this order:
        // FileID, RunNumber, FileName, FileStatus, StartDate, EndDate, Stream,
        // MD5Sum, LogicalName, LogicalStatus, EventStat, FileSize
        case 0:    r->ID              = atol(tok);   break;
        case 1:    r->RunNumber       = atol(tok);   break;
        case 2:    r->FileName        = tok;         break;
        case 3:    r->FileStatus      = tok;         break;
        case 4:    r->StartTime       = atol(tok);   break;
        case 5:    r->EndTime         = atol(tok);   break;
        case 6:    r->Stream          = tok;         break;
        case 7:    r->MD5Sum          = tok;         break;
        case 8:    r->LogicalName     = tok;         break;
        case 9:    r->LogicalStatus   = tok;         break;
        case 10:   r->EventStat       = atol(tok);   break;
        case 11:   r->FileSize        = atol(tok);
          return r;
      }
    }
    return Result(0);
  }
}

