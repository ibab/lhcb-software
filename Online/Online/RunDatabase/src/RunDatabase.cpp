#include "RunDatabase.h"
#include "PyRPC.h"
#include <ctime>

typedef LHCb::RunDatabase   RDB;
typedef const std::string&  CSTR;
typedef PyRPC::Param        _P;
typedef PyRPC::Args         _OPT;
typedef PyRPC::Tuple        _TUP;
typedef PyRPC::MethodCall   _CALL;

namespace {
  template<class T> T _exec(LHCb::DimRpcCommandClient& cl, const _CALL& call) {
    std::string cmd = call.str();
    std::string result = cl.executeCmd(cmd);
    std::cout << ">>> " << cmd << std::endl;
    std::cout << "<<< " << result << std::endl;
    T t;
    if ( !result.empty() )  {
      t.read(result);
      if ( !t.ok() )  {
        std::cout << "Error:" << cmd << std::endl << "-->" << t.error() << std::endl;
      }
      return t;
    }
    std::cout << "Error:" << cmd << std::endl;
    t.setError("(0, 'Got empty resultset. Is the server down ?\nCommand:"+cmd+"')");
    return t;
  }
}

// Initializing constructor
RDB::RunDatabase(CSTR addr) : m_rpcClient(0), m_address(addr)  {
  m_rpcClient = new DimRpcCommandClient(m_address);
}
// Default destructor
RDB::~RunDatabase()   {
  delete m_rpcClient;
}
//_________________________________________________________________________________________________
StatusCode RDB::initialize()  {
  return StatusCode::SUCCESS;
}
//_________________________________________________________________________________________________
StatusCode RDB::finalize()  {
  return StatusCode::SUCCESS;
}
//_________________________________________________________________________________________________
LHCb::DimRpcCommandClient& RDB::client() const  {
  if ( m_rpcClient ) return *m_rpcClient;
  throw std::runtime_error("Invalid RPC Client to access run database!");
}
//_________________________________________________________________________________________________
RDB::Status RDB::existsRun(int run_no)     {
  return _exec<Status>(client(),_CALL("existsRun",_OPT(_P("RunNumber",run_no))));
}
//_________________________________________________________________________________________________
RDB::Param RDB::runParam(int run_no, CSTR nam)  {
  return _exec<Param>(client(),_CALL("runParam",_OPT(_P("RunNumber",run_no),_P("Name",nam))));
}
//_________________________________________________________________________________________________
RDB::Params RDB::runParams(int run_no, const _OPT& args)   {
  return _exec<Params>(client(),_CALL("runParams",_OPT(_P("RunNumber",run_no),args)));
}
//_________________________________________________________________________________________________
RDB::Params RDB::runParams(int run_no)   {
  return _exec<Params>(client(),_CALL("runParams",_OPT(_P("RunNumber",run_no))));
}
//_________________________________________________________________________________________________
RDB::Run RDB::run (int run_no)  {
  return _exec<Run>(client(),_CALL("runs",_OPT(_P("RunNumber",run_no))));
}
//_________________________________________________________________________________________________
RDB::Runs   RDB::runs(int run_no_low, int run_no_high)  {
  _CALL call("runs",_OPT(_P("RunNumber",_TUP(">=",run_no_low," AND RunNumber <=",run_no_high))));
  return _exec<Runs>(client(),call);
}
//_________________________________________________________________________________________________
RDB::Runs RDB::runsForFill(int fill_no)  {
  return _exec<Runs>(client(), _CALL("runs",_OPT(_P("FillNumber",fill_no))));
}
//_________________________________________________________________________________________________
RDB::Runs RDB::runsForUTC(time_t from, time_t to)  {
  return _exec<Runs>(client(),_CALL("runs",_OPT(_P("StartDate",_TUP(">=",from," AND StartDate <=",to)))));
}
//_________________________________________________________________________________________________
RDB::Runs RDB::runsForUTC(tm& from, tm& to)  {
  time_t tto = mktime(&to);
  time_t tfr = mktime(&from);
  _CALL call("runs",_OPT(_P("StartDate",_TUP(">=",tfr," AND StartDate <=",tto))));
  return _exec<Runs>(client(),call);
}
//_________________________________________________________________________________________________
RDB::Files RDB::files(int run_no)  {
  return files(_OPT(_P("RunNumber",run_no)));
}
//_________________________________________________________________________________________________
RDB::Files RDB::files(const _OPT& args)  {
  return _exec<Files>(client(),_CALL("files",args));
}
//_________________________________________________________________________________________________
RDB::Params RDB::fileParams    (int file_id)  {
  return fileParams(_OPT(_P("FileID",file_id)));
}
//_________________________________________________________________________________________________
RDB::Params RDB::fileParams    (CSTR file_name)  {
  return fileParams(_OPT(_P("FileName",file_name)));
}
//_________________________________________________________________________________________________
RDB::Params RDB::fileParams    (const Options& opts)  {
  return _exec<Params>(client(),_CALL("fileParams",opts));
}
//_________________________________________________________________________________________________
RDB::Result<int> 
RDB::createRun(int fill_no, int part_id, CSTR activity, time_t start_tim, CSTR pgm, CSTR vsn)
{
  _OPT opts(_P("FillNumber",fill_no),
            _P("Partition",part_id),
            _P("Activity",activity),
            _P("StartDate",start_tim),
            _P("ProgramName",pgm),
            _P("ProgramVersion",vsn));
  _CALL call("createRun",opts);
  return _exec<Result<int> >(client(),call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::finalizeRun(int run_no, time_t end_tim, float integ_lumi)   {
  _OPT opts(_P("RunNumber",run_no),
            _P("EndDate",end_tim),
            _P("IntegratedLumi",integ_lumi));
  return _exec<Status>(client(),_CALL("finalizeRun",opts));
}
//_________________________________________________________________________________________________
RDB::Status RDB::modifyRun(int run_no, const _OPT& args)    {
  return _exec<Status>(client(),_CALL("modifyRun",_OPT(_P("RunNumber",run_no), args)));
}
//_________________________________________________________________________________________________
RDB::Status RDB::modifyRunParam(int run_no, CSTR par_nam, int par_val, CSTR par_typ)  {
  _CALL call("modifyRunParam",_OPT(_P("RunNumber",run_no),_P("Name",par_nam),_P("Val",par_val),_P("Typ",par_typ)));
  return _exec<Status>(client(),call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::modifyRunParam(int run_no, CSTR par_nam, float par_val, CSTR par_typ)  {
  _CALL call("modifyRunParam",_OPT(_P("RunNumber",run_no),_P("Name",par_nam),_P("Val",par_val),_P("Typ",par_typ)));
  return _exec<Status>(client(),call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::modifyRunParam(int run_no, CSTR par_nam, CSTR par_val, CSTR par_typ)  {
  _CALL call("modifyRunParam",_OPT(_P("RunNumber",run_no),_P("Name",par_nam),_P("Val",par_val),_P("Typ",par_typ)));
  return _exec<Status>(client(),call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addRunParam(int run_no, CSTR par_nam, int par_val, CSTR par_typ)  {
  _CALL call("addRunParam",_OPT(_P("RunNumber",run_no),_P("Name",par_nam),_P("Val",par_val),_P("Typ",par_typ)));
  return _exec<Status>(client(),call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addRunParam(int run_no, CSTR par_nam, float par_val, CSTR par_typ)  {
  _CALL call("addRunParam",_OPT(_P("RunNumber",run_no),_P("Name",par_nam),_P("Val",par_val),_P("Typ",par_typ)));
  return _exec<Status>(client(),call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addRunParam(int run_no, CSTR par_nam, CSTR par_val, CSTR par_typ)  {
  _CALL call("addRunParam",_OPT(_P("RunNumber",run_no),_P("Name",par_nam),_P("Val",par_val),_P("Typ",par_typ)));
  return _exec<Status>(client(),call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFile(int run_no, CSTR fname, CSTR s_name, const _OPT& opts)  {
  _CALL call("addFile",_OPT(run_no, fname, s_name, opts));
  return _exec<Status>(client(),call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, const _OPT& file, CSTR par_nam, int par_val, CSTR par_typ)
{
  _CALL call("addFileParam",_OPT(_P("RunNumber",run_no),_P("Name",par_nam),_P("Val",par_val),_P("Typ",par_typ), file));
  return _exec<Status>(client(),call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, const _OPT& file, CSTR par_nam, float par_val, CSTR par_typ)
{
  _CALL call("addFileParam",_OPT(_P("RunNumber",run_no),_P("Name",par_nam),_P("Val",par_val),_P("Typ",par_typ), file));
  return _exec<Status>(client(),call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, const _OPT& file, CSTR par_nam, CSTR par_val, CSTR par_typ)
{
  _CALL call("addFileParam",_OPT(_P("RunNumber",run_no),_P("Name",par_nam),_P("Val",par_val),_P("Typ",par_typ), file));
  return _exec<Status>(client(),call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, CSTR fname, CSTR par_nam, int par_val, CSTR par_typ)  {
  return addFileParam(run_no,_OPT("FileName",fname),par_nam,par_val,par_typ);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, CSTR fname, CSTR par_nam, float par_val, CSTR par_typ)  {
  return addFileParam(run_no,_OPT("FileName",fname),par_nam,par_val,par_typ);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, CSTR fname, CSTR par_nam, CSTR par_val, CSTR par_typ)  {
  return addFileParam(run_no,_OPT("FileName",fname),par_nam,par_val,par_typ);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, int fid, CSTR par_nam, int par_val, CSTR par_typ)  {
  return addFileParam(run_no,_OPT("FileID",fid),par_nam,par_val,par_typ);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, int fid, CSTR par_nam, float par_val, CSTR par_typ)  {
  return addFileParam(run_no,_OPT("FileID",fid),par_nam,par_val,par_typ);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, int fid, CSTR par_nam, CSTR par_val, CSTR par_typ)  {
  return addFileParam(run_no,_OPT("FileID",fid),par_nam,par_val,par_typ);
}
