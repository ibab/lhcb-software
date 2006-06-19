#ifndef RUNDATABASE_IRUNDATABASEREADER_H
#define RUNDATABASE_IRUNDATABASEREADER_H

#include "GaudiKernel/StatusCode.h"
#include <vector>
#include <string>

namespace PyRPC {   template <int i> struct Container;  }
/*
 *  LHCb namespace
 */
namespace LHCb {

  class IRunDatabaseTypes  {
  public:
    /**@class Handle  IRundatabaseReader.h RunDatabase/IRundatabaseReader.h
      *
      *  @author  M. Frank
      *  @version 1.0
      */
    template<typename T> class Handle    {
    public:
	    typedef T element_type;
	    explicit Handle(T *p = 0) : _Owns(p != 0), _Ptr(p) {}
	    Handle(const Handle<T>& h): _Owns(h._Owns), _Ptr(h.release()) {}
	    Handle<T>& operator=(const Handle<T>& _Y) {
        if (this != &_Y)  {
          if (_Ptr != _Y.get()) {
            if (_Owns)
					    delete _Ptr;
				    _Owns = _Y._Owns;
          }
			    else if (_Y._Owns)
				    _Owns = true;
			    _Ptr = _Y.release();
        }
		    return (*this);
      }
      ~Handle()               { if (_Owns) delete _Ptr;    }
      T& operator*() const		{ return (*get());           }
      T *operator->() const		{ return (get());            }
      T *get() const      		{ return (_Ptr);             }
      T *release() const      {
        ((Handle<T> *)this)->_Owns = false;
        return (_Ptr);
      }
    private:
	    bool _Owns;
	    T *_Ptr;
	  };


    /**@class ResultBase  IRundatabaseReader.h RunDatabase/IRundatabaseReader.h
      *
      *  @author  M. Frank
      *  @version 1.0
      */
    class ResultBase  {
    protected:
      int m_status;
      std::string m_error;
    public:
      ResultBase(int sc) : m_status(sc)  {}
      bool ok()     const            {  return m_status == 1; }
      int  status() const            {  return m_status;      }
      const std::string& error() const { return m_error;      }
      void setError(const std::string& err)  { m_error = err; }
      void setStatus(int sc)                 { m_status = sc; }
      std::string str(const std::string& prefix="")  const;
    };

    /**@class Result  IRundatabaseReader.h RunDatabase/IRundatabaseReader.h
      *
      *  @author  M. Frank
      *  @version 1.0
      */
    template<typename T> class Result : public ResultBase {
    public:
      typedef typename T DATA;
      DATA data;
      Result() : ResultBase(0)       {                        }
      Result(int sc) : ResultBase(sc){                        }
      int read(const std::string& s);
      std::string str(const std::string& prefix="")  const;
    };

    /**@class Result<void>  IRundatabaseReader.h RunDatabase/IRundatabaseReader.h
      *
      *  @author  M. Frank
      *  @version 1.0
      */
    template<> class Result<void> : public ResultBase {
    public:
      Result() : ResultBase(0)       {                        }
      Result(int sc) : ResultBase(sc){                        }
      int  read(const std::string& s);
    };

    /**@class Parameter  IRundatabaseReader.h RunDatabase/IRundatabaseReader.h
      *
      *  @author  M. Frank
      *  @version 1.0
      */
    class Parameter  {
    public:
      int ID;
      std::string Name, Value, Type;
      std::string str(const std::string& prefix="")  const;
    };

    /**@class FileRecord  IRundatabaseReader.h RunDatabase/IRundatabaseReader.h
      *
      *  @author  M. Frank
      *  @version 1.0
      */
    struct FileRecord  {
      int         ID;
      int         RunNumber;
      std::string FileName;
      std::string FileStatus;
      std::string Stream;
      time_t      StartTime;
      time_t      EndTime;
      std::string MD5Sum;
      std::string LogicalName;
      std::string LogicalStatus;
      int         EventStat;
      int         FileSize;
      std::string str(const std::string& prefix="")  const;
    };

    /**@class RunRecord  IRundatabaseReader.h RunDatabase/IRundatabaseReader.h
      *
      *  @author  M. Frank
      *  @version 1.0
      */
    struct RunRecord  {
      int         RunNumber;
      int         FillNumber;
      time_t      StartTime;
      time_t      EndTime;
      double      IntegLumi;
      std::string Partition;
      std::string Activity;
      std::string ProgramName;
      std::string ProgramVersion;
      std::string str(const std::string& prefix="")  const;
    };

    typedef PyRPC::Container<3>                      Options;
    typedef Result<void>                              Status;
    typedef Result<Handle<RunRecord> >                Run;
    typedef Result<std::vector<Handle<RunRecord> > >  Runs;
    typedef Result<Handle<FileRecord> >               File;
    typedef Result<std::vector<Handle<FileRecord> > > Files;
    typedef Result<Handle<Parameter> >                Param;
    typedef Result<std::vector<Handle<Parameter> > >  Params;
  };

  class IRunDatabaseReader : virtual public IRunDatabaseTypes {
  public:
    virtual Status existsRun     (int run_no) throw() = 0;
    virtual Run    run           (int run_no) throw() = 0;
    virtual Runs   runs          (int run_no_low, int run_no_high) throw() = 0;
    virtual Runs   runsForFill   (int fill_no) throw() = 0;
    virtual Runs   runsForUTC    (time_t from, time_t to) throw() = 0;
    virtual Runs   runsForUTC    (tm& from, tm& to) throw() = 0;
    virtual Param  runParam      (int run_no, const std::string& name) throw() = 0;
    virtual Params runParams     (int run_no) throw() = 0;
    virtual Params runParams     (int run_no, const Options& args) throw() = 0;
    virtual Files  files         (int run_no) throw() = 0;
    virtual Params fileParams    (int file_id) throw() = 0;
    virtual Params fileParams    (const std::string& file_name) throw() = 0;
    virtual Params fileParams    (const Options&     opts) throw() = 0;
  };
}
#endif // RUNDATABASE_IRUNDATABASEREADER_H
