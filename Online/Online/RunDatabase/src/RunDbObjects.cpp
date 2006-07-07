#include "RunDatabase/IRunDatabaseReader.h"
#include "RTL/rtl.h"
#include "PyRPC.h"
#include <sstream>
#include <iomanip>

typedef LHCb::IRunDatabaseReader   RDB;
using namespace PyRPC;
namespace  {

  template <typename T> int readItem(T& obj, const std::string& s)  {
    Reader<typename T::DATA> rdr(s);
    for(typename T::DATA p=rdr.next(); p.get(); )  {
      obj.data = p;
      goto Done;
    }
    obj.setError(s);
Done:
    obj.setStatus(rdr.status());
    return rdr.status();
  }
  
  template <typename T> std::string stringItem(T& obj, const std::string& prefix)  {
    std::stringstream os;
    os << obj.toString(prefix);
    if ( obj.ok() ) os << std::endl << obj.data->str(prefix);
    return os.str();
  }

  template <typename T> int readItem(RDB::Result<std::vector<T> >& obj, const std::string& s)  {
    typedef typename std::vector<T>::value_type value_t;
    typedef typename RDB::Result<value_t>       result_t;
    typedef typename result_t::DATA             handle_t;
    Reader<handle_t> rdr(s);
    obj.data.clear();
    if ( rdr.ok() )  {
      for(handle_t p=rdr.next(); p.get(); p=rdr.next())  {
        obj.data.push_back(handle_t(p));
      }
      goto Done;
    }
    obj.setError(s);
Done:
    obj.setStatus(rdr.status());
    return rdr.status();
  }
  
  template <typename T> 
  std::string stringItem(const RDB::Result<std::vector<T> >& obj, const std::string& prefix) {
    std::stringstream os;
    os << obj.toString(prefix);
    if ( obj.ok() )  {
      typedef typename std::vector<T>       vector_t;
      typedef typename vector_t::value_type value_t;
      typedef typename RDB::Result<value_t> result_t;
      typedef typename result_t::DATA       handle_t;
      if ( !obj.data.empty() ) os << std::endl;
      for(typename vector_t::const_iterator i=obj.data.begin(); i != obj.data.end(); ++i)  {
        os << (*i)->str(prefix) << std::endl;
      }
    }
    return os.str();
  }
}

//_________________________________________________________________________________________________
std::string RDB::ResultBase::toString(const std::string& prefix)  const {
  std::stringstream os;
  os << prefix << std::setw(32) << std::left << "Status:" << m_status;
  if ( !ok() ) os << " Error:" << error();
  return os.str();
}
//_________________________________________________________________________________________________
int RDB::Status::read(const std::string& s)  {
  ResultReader rdr(s);
  if ( rdr.ok() )  {
    return m_status=rdr.status();
  }
  m_error = s;
  return 0;
}
//_________________________________________________________________________________________________
int RDB::Result<int>::read(const std::string& s)  {
  ResultReader rdr(s);
  if ( rdr.ok() )  {
    const char* tok = rdr.next();
    data = atol(tok);
    return m_status = rdr.status();
  }
  m_error = s;
  return 0;
}
//_________________________________________________________________________________________________
std::string RDB::Result<int>::str(const std::string& prefix) const {
  std::stringstream os;
  os << this->ResultBase::str(prefix) << std::endl
     << prefix << std::setw(32) << std::left << "Data=" << data;
  return os.str();
}
//_________________________________________________________________________________________________
int         RDB::Param::read(const std::string& source)        { return readItem(*this, source);    }
std::string RDB::Param::str(const std::string& prefix)   const { return stringItem(*this, prefix);  }
int         RDB::Params::read(const std::string& source)       { return readItem(*this, source);    }
std::string RDB::Params::str(const std::string& prefix)  const { return stringItem(*this, prefix);  }
int         RDB::Run::read(const std::string& source)          { return readItem(*this, source);    }
std::string RDB::Run::str(const std::string& prefix) const     { return stringItem(*this, prefix);  }
int         RDB::Runs::read(const std::string& source)         { return readItem(*this, source);    }
std::string RDB::Runs::str(const std::string& prefix) const    { return stringItem(*this, prefix);  }
int         RDB::File::read(const std::string& source)         { return readItem(*this, source);    }
std::string RDB::File::str(const std::string& prefix) const    { return stringItem(*this, prefix);  }
int         RDB::Files::read(const std::string& source)        { return readItem(*this, source);    }
std::string RDB::Files::str(const std::string& prefix) const   { return stringItem(*this, prefix);  }
//_________________________________________________________________________________________________
std::string RDB::Parameter::str(const std::string& prefix)  const {
  std::stringstream os;
  os << prefix << "\t" << ID << "  " << std::setw(32) << std::left << Name+'['+Type+"]=" << Value;
  return os.str();
}
//_________________________________________________________________________________________________
std::string RDB::RunRecord::str(const std::string& prefix)  const {
  std::stringstream os;
  os << prefix << "\tRun:"  << "RunNumber:     " << std::setw(8)  << std::left << RunNumber
                            << " FillNumber:" << std::setw(8)  << std::left << FillNumber
                            << " Partition: " << std::setw(15) << std::left << Partition
                            << " Activity:  " << std::setw(15) << std::left << Activity
                            << " IntegLumi: " << IntegLumi      << std::endl;
  os << prefix << "\t    "  << "Duration (UTC):" << std::setw(20) << std::left
                            << lib_rtl_gmtimestr("%d-%m-%Y %H:%M.%S",&StartTime)
                            << " to " << lib_rtl_gmtimestr("%d-%m-%Y %H:%M.%S",&EndTime) << std::endl;
  os << prefix << "\t    "  << "Program:       " << ProgramName    << "  " << ProgramVersion << std::endl;
  return os.str();
}
//_________________________________________________________________________________________________
std::string RDB::FileRecord::str(const std::string& prefix)  const {
  std::stringstream os;
  os << prefix << "\tFile:" << "FileID:"      << std::setw(8) << std::left << ID
                            << " RunNumber:"  << std::setw(8) << std::left << RunNumber
                            << " EventStat:"  << std::setw(8) << std::left << EventStat
                            << " FileSize:"   << std::setw(10)<< std::left << FileSize
                            << " Stream:"     << Stream       << std::endl;
  os << prefix << "\t     " << "FileName:     " << std::setw(90) << std::left << FileName
                            << " [" << FileStatus    << "]" << std::endl;
  os << prefix << "\t     " << "LogicalName:  " << std::setw(90) << std::left << LogicalName
                            << " [" << LogicalStatus << "]" << std::endl;
  os << prefix << "\t     " << "MD5Sum:       " << MD5Sum         << std::endl;
  os << prefix << "\t     " << "Duration (UTC):" << std::setw(20) << std::left
                            << lib_rtl_gmtimestr("%d-%m-%Y %H:%M.%S",&StartTime)
                            << " to " << lib_rtl_gmtimestr("%d-%m-%Y %H:%M.%S",&EndTime) << std::endl;
  return os.str();
}

