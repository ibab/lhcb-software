#include "RunDatabase.h"
typedef RunDatabase::Status  Status;
typedef RunDatabase::Param   Param;
typedef RunDatabase::Params  Params;

namespace {
  class ParamReader  {
    PyRPC::ResultReader    m_rdr;
    RunDatabase::Parameter m_param;
  public:
    ParamReader(const std::string& s) : m_rdr(s)  {  }
    int status()  const  {  return m_rdr.status();      }
    void reset(const std::string& s)  {
      m_rdr.reset(s);
    }
    RunDatabase::Parameter* next()   {
      const char* tok = m_rdr.next();
      for(size_t cnt=0; tok && cnt<4; ++cnt, tok=m_rdr.next()) {
        switch(++cnt) {
          case 1:    m_param.ID = atol(tok);      break;
          case 2:    m_param.Name = tok;          break;
          case 3:    m_param.Value = tok;         break;
          case 4:    m_param.Type = tok;          return &m_param;
        }
      }
      return 0;
    }
  };
}

template<> int Params::read(const std::string& s)  {
  ParamReader rdr(s);
  data.clear();
  for(RunDatabase::Parameter* p=rdr.next(); p; p=rdr.next())
    data.push_back(*p);
  return rdr.status();
}

template<> int Param::read(const std::string& s)  {
  ParamReader rdr(s);
  for(RunDatabase::Parameter* p=rdr.next(); p; )  {
    data = *p;
    return rdr.status();
  }
  data.Value = data.Name = data.Type = "";
  data.ID = 0;
  return rdr.status();
}

RunDatabase::Parameter::Parameter(const Parameter& p) 
: ID(p.ID), Name(p.Name), Value(p.Value), Type(p.Type) 
{
}

RunDatabase::Parameter& RunDatabase::Parameter::operator=(const Parameter& p) {
  ID = p.ID;
  Name = p.Name;
  Value = p.Value;
  Type = p.Type;
  return *this;
}

using PyRPC::MethodCall;
template<class T> T _exec(const MethodCall& call)  {
  std::cout << call << std::endl;
  return T();
}

Status RunDatabase::existsRun(int run_no)  {
  MethodCall call("existsRun",Options(PyRPC::Param("RunNumber",run_no)));
  return _exec<Status>(call);
}

Status RunDatabase::createRun(int run_no, int fill_no, int part_id, time_t start_tim, float start_lum, float energy)  {
  MethodCall call("createRun",Options(PyRPC::Param("RunNumber",run_no),
                                      PyRPC::Param("FillNumber",fill_no),
                                      PyRPC::Param("PartitionID",part_id),
                                      PyRPC::Param("StartDate",PyRPC::Time(start_tim)),
                                      PyRPC::Param("StartLuminosity",start_lum),
                                      PyRPC::Param("BeamEnergy",energy)
    ));
  return _exec<Status>(call);
}

Status RunDatabase::modifyRun(int run_no, const Options& args)   {
  MethodCall call("modifyRun",Options(run_no, args));
  return _exec<Status>(call);
}

Param  RunDatabase::runParam(int run_no, const std::string& nam)    {
  MethodCall call("runParam",Options(run_no, nam.c_str()));
  return _exec<Param>(call);
}

Params RunDatabase::runParams(int run_no, const Options& args)  {
  MethodCall call("runParam",Options(run_no, args));
  return _exec<Params>(call);
}

Status RunDatabase::modifyRunParam(int run_no, const std::string& par_nam, int par_val, const std::string& par_typ)
{
  MethodCall call("modifyRunParam",Options(run_no, par_nam.c_str(), par_val, par_typ.c_str()));
  return _exec<Status>(call);
}

Status RunDatabase::modifyRunParam(int run_no, const std::string& par_nam, float par_val, const std::string& par_typ)
{
  MethodCall call("modifyRunParam",Options(run_no, par_nam.c_str(), par_val, par_typ.c_str()));
  return _exec<Status>(call);
}

Status RunDatabase::modifyRunParam(int run_no, const std::string& par_nam, const std::string& par_val, const std::string& par_typ)
{
  MethodCall call("modifyRunParam",Options(run_no, par_nam.c_str(), par_val.c_str(), par_typ.c_str()));
  return _exec<Status>(call);
}

Status RunDatabase::addRunParam(int run_no, const std::string& par_nam, int par_val, const std::string& par_typ)
{
  MethodCall call("addRunParam",Options(run_no, par_nam.c_str(), par_val, par_typ.c_str()));
  return _exec<Status>(call);
}

Status RunDatabase::addRunParam(int run_no, const std::string& par_nam, float par_val, const std::string& par_typ)
{
  MethodCall call("addRunParam",Options(run_no, par_nam.c_str(), par_val, par_typ.c_str()));
  return _exec<Status>(call);
}

Status RunDatabase::addRunParam(int run_no, const std::string& par_nam, const std::string& par_val, const std::string& par_typ)
{
  MethodCall call("addRunParam",Options(run_no, par_nam.c_str(), par_val.c_str(), par_typ.c_str()));
  return _exec<Status>(call);
}

Status RunDatabase::addFile(int run_no, const std::string& fname, const std::string& s_name, const Options& opts)
{
  MethodCall call("addFile",Options(run_no, fname.c_str(), s_name.c_str(), opts));
  return _exec<Status>(call);
}

Status RunDatabase::addFileParam(int run_no, const Dictionary& file, const std::string& par_nam, int par_val, const std::string& par_typ)
{
  MethodCall call("addFileParam",Options(run_no, par_nam.c_str(), par_val, par_typ.c_str()));
  return _exec<Status>(call);
}

Status RunDatabase::addFileParam(int run_no, const Dictionary& file, const std::string& par_nam, float par_val, const std::string& par_typ)
{
  MethodCall call("addFileParam",Options(run_no, par_nam.c_str(), par_val, par_typ.c_str()));
  return _exec<Status>(call);
}

Status RunDatabase::addFileParam(int run_no, const Dictionary& file, const std::string& par_nam, const std::string& par_val, const std::string& par_typ)
{
  MethodCall call("addFileParam",Options(run_no, par_nam.c_str(), par_val.c_str(), par_typ.c_str()));
  return _exec<Status>(call);
}

void test_rundatabase()  {
RunDatabase::Parameter * p;
  ParamReader rdr("(1, [(260, 'Status', 'Opened', 'PARAM')])");
  for(p=rdr.next(); p; p=rdr.next())
    std::cout << p->ID << " " << p->Name << " " << p->Value << " " << p->Type << std::endl;
  rdr.reset("(1, [(260, 'Status', 'Opened with blanks !', 'PARAM')(260, 'Other', 'Value', 'ENV')])");
  for(p=rdr.next(); p; p=rdr.next())
    std::cout << p->ID << " " << p->Name << " " << p->Value << " " << p->Type << std::endl;
  rdr.reset("(1, (12, 'OtherParam', '7654321032', 'SOME_TYPE'))");
  for(p=rdr.next(); p; p=rdr.next())
    std::cout << p->ID << " " << p->Name << " " << p->Value << " " << p->Type << std::endl;
}