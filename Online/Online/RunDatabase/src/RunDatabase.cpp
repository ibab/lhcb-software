#include "RunDatabase.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "RunDatabase/IRunDatabaseWriter.h"
#include "PyRPC.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,RunDatabase)

typedef LHCb::RunDatabase   RDB;
typedef const std::string&  CSTR;
typedef PyRPC::Param        _P;
typedef PyRPC::Args         _OPT;
typedef PyRPC::Tuple        _TUP;
typedef PyRPC::MethodCall   _CALL;

namespace {
  template<class T> T _exec(RDB* db, const _CALL& call) {
    std::string cmd = call.str();
    std::string result = db->client().executeCmd(cmd);
    // std::cout << ">>> " << cmd << std::endl << "<<< " << result << std::endl;
    T t;
    if ( !result.empty() )  {
      t.read(result);
      if ( !t.ok() )  {
        MsgStream log(db->serviceLocator() ? db->msgSvc() : 0, db->name());
        log << MSG::ERROR << "Error:" << cmd << std::endl << "-->" << t.error() << endmsg;
      }
      return t;
    }
    MsgStream log(db->serviceLocator() ? db->msgSvc() : 0, db->name());
    log << MSG::ERROR << "Error:" << cmd << endmsg;
    t.setError("(0, 'Got empty resultset. Is the server down ?\nCommand:"+cmd+"')");
    return t;
  }
}

// Initializing constructor
RDB::RunDatabase(CSTR addr) : Service("RunDatabase",0), m_rpcClient(0), m_address(addr)  {
  m_rpcClient = new DimRpcCommandClient(m_address);
}

// Initializing Gaudi service constructor
RDB::RunDatabase(const std::string& nam, ISvcLocator* svc) 
: Service(nam,svc), m_rpcClient(0), m_address("RunDbSrv/RunDb")
{
  declareProperty("DimAddress",  m_address);
}

// Default destructor
RDB::~RunDatabase()   {
  if ( m_rpcClient ) delete m_rpcClient;
  m_rpcClient = 0;
}
//_________________________________________________________________________________________________
StatusCode RDB::initialize()  {
  StatusCode sc = Service::initialize();
  if ( !sc.isSuccess() )  {
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << "Failed to initialize base class service." << endmsg;
    return sc;
  }
  m_rpcClient = new DimRpcCommandClient(m_address);
  return StatusCode::SUCCESS;
}
//_________________________________________________________________________________________________
StatusCode RDB::finalize()  {
  if ( m_rpcClient ) delete m_rpcClient;
  m_rpcClient = 0;
  return Service::finalize();
}
/// Query interfaces of Interface
StatusCode RDB::queryInterface(const InterfaceID& riid, void** ppvUnknown)  {
  if ( IID_IRunDatabaseReader.versionMatch(riid) )
    *ppvUnknown = (IRunDatabaseReader*)this;
  else if ( IID_IRunDatabaseWriter.versionMatch(riid) )
    *ppvUnknown = (IRunDatabaseWriter*)this;
  else
    return Service::queryInterface(riid, ppvUnknown);
  addRef();
  return StatusCode::SUCCESS;
}
//_________________________________________________________________________________________________
LHCb::DimRpcCommandClient& RDB::client() const  {
  if ( m_rpcClient ) return *m_rpcClient;
  throw std::runtime_error("Invalid RPC Client to access run database!");
}
//_________________________________________________________________________________________________
RDB::Status RDB::existsRun(int run_no) throw()    {
  return _exec<Status>(this,_CALL("existsRun",_OPT(_P("RunNumber",run_no))));
}
//_________________________________________________________________________________________________
RDB::Param RDB::runParam(int run_no, CSTR name) throw() {
  return _exec<Param>(this,_CALL("runParam",_OPT(_P("RunNumber",run_no),_P("Name",name))));
}
//_________________________________________________________________________________________________
RDB::Params RDB::runParams(int run_no, const _OPT& args) throw()  {
  return _exec<Params>(this,_CALL("runParams",_OPT(_P("RunNumber",run_no),args)));
}
//_________________________________________________________________________________________________
RDB::Params RDB::runParams(int run_no) throw()  {
  return _exec<Params>(this,_CALL("runParams",_OPT(_P("RunNumber",run_no))));
}
//_________________________________________________________________________________________________
RDB::Run RDB::run (int run_no) throw() {
  return _exec<Run>(this,_CALL("runs",_OPT(_P("RunNumber",run_no))));
}
//_________________________________________________________________________________________________
RDB::Runs   RDB::runs(int run_low, int run_high) throw() {
  _CALL call("runs",_OPT(_P("RunNumber",_TUP(">=",run_low," AND RunNumber <=",run_high))));
  return _exec<Runs>(this,call);
}
//_________________________________________________________________________________________________
RDB::Runs RDB::runsForFill(int fill_no) throw() {
  return _exec<Runs>(this, _CALL("runs",_OPT(_P("FillNumber",fill_no))));
}
//_________________________________________________________________________________________________
RDB::Runs RDB::runsForUTC(time_t from, time_t to)  throw()  {
  return _exec<Runs>(this,_CALL("runs",_OPT(_P("StartDate",_TUP(">=",from," AND StartDate <=",to)))));
}
//_________________________________________________________________________________________________
RDB::Runs RDB::runsForUTC(tm& from, tm& to) throw() {
  _CALL call("runs",_OPT(_P("StartDate",_TUP(">=",mktime(&from)," AND StartDate <=",mktime(&to)))));
  return _exec<Runs>(this,call);
}
//_________________________________________________________________________________________________
RDB::Files RDB::files(int run_no) throw()  {
  return files(_OPT(_P("RunNumber",run_no)));
}
//_________________________________________________________________________________________________
RDB::Files RDB::files(const _OPT& args)  throw()  {
  return _exec<Files>(this,_CALL("files",args));
}
//_________________________________________________________________________________________________
RDB::Params RDB::fileParams    (int file_id)  throw()  {
  return fileParams(_OPT(_P("FileID",file_id)));
}
//_________________________________________________________________________________________________
RDB::Params RDB::fileParams    (CSTR file_name)  throw()  {
  return fileParams(_OPT(_P("FileName",file_name)));
}
//_________________________________________________________________________________________________
RDB::Params RDB::fileParams    (const Options& opts)  throw()  {
  return _exec<Params>(this,_CALL("fileParams",opts));
}
//_________________________________________________________________________________________________
RDB::Result<int> 
RDB::createRun(int fill_no, int part_id, CSTR activity, time_t start_tim, CSTR pgm, CSTR vsn) throw()
{
  _OPT opts(_P("FillNumber",    fill_no),
            _P("Partition",     part_id),
            _P("Activity",      activity),
            _P("StartDate",     start_tim),
            _P("ProgramName",   pgm),
            _P("ProgramVersion",vsn));
  _CALL call("createRun",opts);
  return _exec<Result<int> >(this,call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::finalizeRun(int run_no, time_t end_tim, float integ_lumi)   throw()  {
  _OPT opts(_P("RunNumber",run_no),
            _P("EndDate",end_tim),
            _P("IntegratedLumi",integ_lumi));
  return _exec<Status>(this,_CALL("finalizeRun",opts));
}
//_________________________________________________________________________________________________
RDB::Status RDB::modifyRun(int run_no, const _OPT& args)   throw()   {
  return _exec<Status>(this,_CALL("modifyRun",_OPT(_P("RunNumber",run_no), args)));
}
//_________________________________________________________________________________________________
RDB::Status RDB::modifyRunParam(int run_no, CSTR name, int value, CSTR type)  throw()  {
  _CALL call("modifyRunParam",_OPT(_P("RunNumber",run_no),_P("Name",name),_P("Val",value),_P("Typ",type)));
  return _exec<Status>(this,call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::modifyRunParam(int run_no, CSTR name, float value, CSTR type)  throw()  {
  _CALL call("modifyRunParam",_OPT(_P("RunNumber",run_no),_P("Name",name),_P("Val",value),_P("Typ",type)));
  return _exec<Status>(this,call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::modifyRunParam(int run_no, CSTR name, CSTR value, CSTR type)  throw()  {
  _CALL call("modifyRunParam",_OPT(_P("RunNumber",run_no),_P("Name",name),_P("Val",value),_P("Typ",type)));
  return _exec<Status>(this,call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addRunParam(int run_no, CSTR name, int value, CSTR type)  throw()  {
  _CALL call("addRunParam",_OPT(_P("RunNumber",run_no),_P("Name",name),_P("Val",value),_P("Typ",type)));
  return _exec<Status>(this,call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addRunParam(int run_no, CSTR name, float value, CSTR type)  throw()  {
  _CALL call("addRunParam",_OPT(_P("RunNumber",run_no),_P("Name",name),_P("Val",value),_P("Typ",type)));
  return _exec<Status>(this,call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addRunParam(int run_no, CSTR name, CSTR value, CSTR type)  throw()  {
  _CALL call("addRunParam",_OPT(_P("RunNumber",run_no),_P("Name",name),_P("Val",value),_P("Typ",type)));
  return _exec<Status>(this,call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFile(int run_no, CSTR fname, CSTR s_name)  throw()  {
  return _exec<Status>(this,_CALL("addFile",_OPT(_P("RunNumber",run_no),_P("FileName",fname),_P("Stream",s_name))));
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFile(int run_no, CSTR fname, CSTR s_name, const _OPT& opts)  throw()  {
  return _exec<Status>(this,_CALL("addFile",_OPT(_P("RunNumber",run_no),_P("FileName",fname),_P("Stream",s_name), opts)));
}
//_________________________________________________________________________________________________
RDB::Status RDB::modifyFile(int run_no, CSTR fname, const Options& args)  throw()  {
  return _exec<Status>(this,_CALL("modifyFile",_OPT(_P("RunNumber",run_no),_P("FileName",fname),args)));
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, const _OPT& file, CSTR name, int value, CSTR type) throw()  {
  _CALL call("addFileParam",_OPT(_P("RunNumber",run_no),_P("Name",name),_P("Val",value),_P("Typ",type), file));
  return _exec<Status>(this,call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, const _OPT& file, CSTR name, float value, CSTR type) throw()  {
  _CALL call("addFileParam",_OPT(_P("RunNumber",run_no),_P("Name",name),_P("Val",value),_P("Typ",type), file));
  return _exec<Status>(this,call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, const _OPT& file, CSTR name, CSTR value, CSTR type) throw()  {
  _CALL call("addFileParam",_OPT(_P("RunNumber",run_no),_P("Name",name),_P("Val",value),_P("Typ",type), file));
  return _exec<Status>(this,call);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, CSTR fname, CSTR name, int value, CSTR type)  throw()  {
  return addFileParam(run_no,_OPT("FileName",fname),name,value,type);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, CSTR fname, CSTR name, float value, CSTR type)  throw()  {
  return addFileParam(run_no,_OPT("FileName",fname),name,value,type);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, CSTR fname, CSTR name, CSTR value, CSTR type)  throw()  {
  return addFileParam(run_no,_OPT("FileName",fname),name,value,type);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, int fid, CSTR name, int value, CSTR type)  throw()  {
  return addFileParam(run_no,_OPT("FileID",fid),name,value,type);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, int fid, CSTR name, float value, CSTR type)  throw()  {
  return addFileParam(run_no,_OPT("FileID",fid),name,value,type);
}
//_________________________________________________________________________________________________
RDB::Status RDB::addFileParam(int run_no, int fid, CSTR name, CSTR value, CSTR type)  throw()  {
  return addFileParam(run_no,_OPT("FileID",fid),name,value,type);
}
