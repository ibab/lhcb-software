#ifndef RUNDATABASE_RUNDATABASE_H
#define RUNDATABASE_RUNDATABASE_H

#include "RunDatabase/IRunDatabaseWriter.h"
#include "DimRpcCommands.h"

namespace LHCb {

  class RunDatabase : virtual public IRunDatabaseWriter, virtual public IRunDatabaseReader {
    DimRpcCommandClient& client() const;

  public:
    DimRpcCommandClient* m_rpcClient;
    std::string          m_address;
    /// Initializing constructor
    RunDatabase(const std::string& dim_address);
    /// Default destructor
    virtual ~RunDatabase();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    /**@IRunDatabaseReader interface implementation         */
    virtual Status existsRun     (int run_no) throw();
    virtual Run    run           (int run_no) throw();
    virtual Runs   runs          (int run_no_low, int run_no_high) throw();
    virtual Runs   runsForFill   (int fill_no) throw();
    virtual Runs   runsForUTC    (time_t from, time_t to) throw();
    virtual Runs   runsForUTC    (tm& from, tm& to) throw();
    virtual Param  runParam      (int run_no, CSTR name) throw();
    virtual Params runParams     (int run_no) throw();
    virtual Params runParams     (int run_no, const Options& args) throw();
    virtual Files  files         (int run_no) throw();
    virtual Files  files         (const Options& opts) throw();
    virtual Params fileParams    (int file_id) throw();
    virtual Params fileParams    (const std::string& file_name) throw();
    virtual Params fileParams    (const Options& opts) throw();

    /**@IRunDatabaseWriter interface implementation         */
    virtual Result<int> createRun(int fill_no, int part_id, CSTR activity, time_t start_tim, CSTR pgm, CSTR vsn) throw();
    virtual Status finalizeRun   (int run_no, time_t end_tim, float integ_lumi) throw();
    virtual Status modifyRun     (int run_no, const Options& args) throw();
    virtual Status modifyRunParam(int run_no, CSTR par_nam, int   par_val, CSTR par_typ) throw();
    virtual Status modifyRunParam(int run_no, CSTR par_nam, float par_val, CSTR par_typ) throw();
    virtual Status modifyRunParam(int run_no, CSTR par_nam, CSTR  par_val, CSTR par_typ) throw();
    virtual Status addRunParam   (int run_no, CSTR par_nam, int   par_val, CSTR par_typ) throw();
    virtual Status addRunParam   (int run_no, CSTR par_nam, float par_val, CSTR par_typ) throw();
    virtual Status addRunParam   (int run_no, CSTR par_nam, CSTR  par_val, CSTR par_typ) throw();
    virtual Status addFile       (int run_no, CSTR fname, CSTR s_name, const Options& opts) throw();
    virtual Status addFileParam  (int run_no, int fid, CSTR par_nam, int par_val, CSTR par_typ) throw();
    virtual Status addFileParam  (int run_no, int fid, CSTR par_nam, float par_val, CSTR par_typ) throw();
    virtual Status addFileParam  (int run_no, int fid, CSTR par_nam, CSTR par_val, CSTR par_typ) throw();
    virtual Status addFileParam  (int run_no, CSTR fname, CSTR par_nam, int par_val, CSTR par_typ) throw();
    virtual Status addFileParam  (int run_no, CSTR fname, CSTR par_nam, float par_val, CSTR par_typ) throw();
    virtual Status addFileParam  (int run_no, CSTR fname, CSTR par_nam, CSTR par_val, CSTR par_typ) throw();
    virtual Status addFileParam  (int run_no, const Options& file, CSTR par_nam, int par_val, CSTR par_typ) throw();
    virtual Status addFileParam  (int run_no, const Options& file, CSTR par_nam, float par_val, CSTR par_typ) throw();
    virtual Status addFileParam  (int run_no, const Options& file, CSTR par_nam, CSTR par_val, CSTR par_typ) throw();
  };
}
#endif // RUNDATABASE_RUNDATABASE_H
