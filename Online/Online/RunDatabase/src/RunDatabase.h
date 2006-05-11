#ifndef RUNDATABASE_RUNDATABASE_H
#define RUNDATABASE_RUNDATABASE_H

#include "PyRPC.h"

class RunDatabase  {
public:
  struct Parameter  {
    int ID;
    std::string Name, Value, Type;
    Parameter() : ID(0) {}
    Parameter(const Parameter& p);
    Parameter& operator=(const Parameter& p);
  };
  template<class T> class Result  {
  protected:
    int m_status;
  public:
    T   data;
    Result() : m_status(0)         {                        }
    Result(int sc) : m_status(sc)  {                        }
    bool ok()     const            {  return m_status == 1; }
    int  status() const            {  return m_status;      }
    int read(const std::string& s);
  };
  template<> class Result<void>  {
  protected:
    int m_status;
  public:
    Result() : m_status(0)         {                        }
    Result(int sc) : m_status(sc)  {                        }
    bool ok()     const            {  return m_status == 1; }
    int  status() const            {  return m_status;      }
    int  read(const std::string& ) {  return 1;             }
  };
  typedef PyRPC::Args  Options;
  typedef PyRPC::Dict  Dictionary;
  typedef Result<void> Status;
  typedef Result<Parameter> Param;
  typedef Result<std::vector<Parameter> > Params;

  Status existsRun(int run_no);
  Status createRun(int run_no, int fill_no, int part_id, time_t start_tim, float start_lum, float energy);
  Status modifyRun(int run_no, const Options& args = Options());
  Param  runParam(int run_no, const std::string& name);
  Params runParams(int run_no, const Options& args = Options());
  Status modifyRunParam(int run_no, const std::string& par_nam, int par_val, const std::string& par_typ="PARAM");
  Status modifyRunParam(int run_no, const std::string& par_nam, float par_val, const std::string& par_typ="PARAM");
  Status modifyRunParam(int run_no, const std::string& par_nam, const std::string& par_val, const std::string& par_typ="PARAM");
  Status addRunParam   (int run_no, const std::string& par_nam, int par_val, const std::string& par_typ="PARAM");
  Status addRunParam   (int run_no, const std::string& par_nam, float par_val, const std::string& par_typ="PARAM");
  Status addRunParam   (int run_no, const std::string& par_nam, const std::string& par_val, const std::string& par_typ="PARAM");

  Status addFile(int run_no, const std::string& fname, const std::string& s_name, const Options& opts=Options());
  Status addFileParam(int run_no, const Dictionary& file, const std::string& par_nam, int par_val, const std::string& par_typ);
  Status addFileParam(int run_no, const Dictionary& file, const std::string& par_nam, float par_val, const std::string& par_typ);
  Status addFileParam(int run_no, const Dictionary& file, const std::string& par_nam, const std::string& par_val, const std::string& par_typ);
};
#endif // RUNDATABASE_RUNDATABASE_H
