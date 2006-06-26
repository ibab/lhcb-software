#ifndef RUNDATABASE_IRUNDATABASEWRITER_H
#define RUNDATABASE_IRUNDATABASEWRITER_H

#include "RunDatabase/IRunDatabaseReader.h"

/*
 *  LHCb namespace
 */
namespace LHCb {

  // Declaration of the interface ID ( interface id, major version, minor version) 
  static const InterfaceID IID_IRunDatabaseWriter("IRunDatabaseWriter", 1 , 0); 

  class IRunDatabaseWriter : virtual public IRunDatabaseReader {
  protected:
    typedef const std::string&        CSTR;
  public:
    typedef IRunDatabaseTypes::Status Status;

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IRunDatabaseWriter; }

    /// Create a new run object in the database.
    /** >>> createRun(FillNumber=1000, Partition=103, Activity="Physics", StartDate=1150728764, ProgramName="Moore", ProgramVersion="v3r6")
      * <<< (1, 201)
      */
    virtual Result<int> createRun(int fill_no, int part_id, CSTR activity, time_t start_tim, CSTR pgm, CSTR vsn) throw() = 0;
    /// Modify existing run table items
    /** >>> finalizeRun(RunNumber=201, EndDate=1150728764, IntegratedLumi=100000)
      * <<< (1,)
      */
    virtual Status finalizeRun   (int run_no, time_t end_tim, float integ_lumi) throw() = 0;
    /// Modify existing run table items
    /** >>> modifyRun(RunNumber=201, EndDate=1150728764, IntegratedLumi=100000)
      * <<< (1,)
      */
    virtual Status modifyRun     (int run_no, const Options& args) throw() = 0;
    virtual Status modifyRunParam(int run_no, CSTR par_nam, int   par_val, CSTR par_typ="PARAM") throw() = 0;
    virtual Status modifyRunParam(int run_no, CSTR par_nam, float par_val, CSTR par_typ="PARAM") throw() = 0;
    virtual Status modifyRunParam(int run_no, CSTR par_nam, CSTR  par_val, CSTR par_typ="PARAM") throw() = 0;
    virtual Status addRunParam   (int run_no, CSTR par_nam, int   par_val, CSTR par_typ="PARAM") throw() = 0;
    virtual Status addRunParam   (int run_no, CSTR par_nam, float par_val, CSTR par_typ="PARAM") throw() = 0;
    virtual Status addRunParam   (int run_no, CSTR par_nam, CSTR  par_val, CSTR par_typ="PARAM") throw() = 0;

    virtual Status addFile       (int run_no, CSTR fname, CSTR s_name) throw() = 0;
    virtual Status addFile       (int run_no, CSTR fname, CSTR s_name, const Options& opts) throw() = 0;
    virtual Status modifyFile    (int run_no, CSTR fname, const Options& args) throw() = 0;
    virtual Status addFileParam  (int run_no, int fid, CSTR par_nam, int par_val, CSTR par_typ) throw() = 0;
    virtual Status addFileParam  (int run_no, int fid, CSTR par_nam, float par_val, CSTR par_typ) throw() = 0;
    virtual Status addFileParam  (int run_no, int fid, CSTR par_nam, CSTR par_val, CSTR par_typ) throw() = 0;
    virtual Status addFileParam  (int run_no, CSTR fname, CSTR par_nam, int par_val, CSTR par_typ) throw() = 0;
    virtual Status addFileParam  (int run_no, CSTR fname, CSTR par_nam, float par_val, CSTR par_typ) throw() = 0;
    virtual Status addFileParam  (int run_no, CSTR fname, CSTR par_nam, CSTR par_val, CSTR par_typ) throw() = 0;
    virtual Status addFileParam  (int run_no, const Options& file, CSTR par_nam, int par_val, CSTR par_typ) throw() = 0;
    virtual Status addFileParam  (int run_no, const Options& file, CSTR par_nam, float par_val, CSTR par_typ) throw() = 0;
    virtual Status addFileParam  (int run_no, const Options& file, CSTR par_nam, CSTR par_val, CSTR par_typ) throw() = 0;
  };
}
#endif // RUNDATABASE_IRUNDATABASEWRITER_H
