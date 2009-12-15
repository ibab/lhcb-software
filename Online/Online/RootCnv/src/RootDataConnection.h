// $Id: RootDataConnection.h,v 1.1 2009-12-15 15:37:25 frankb Exp $
#ifndef GAUDIROOT_ROOTDATACONNECTION_H
#define GAUDIROOT_ROOTDATACONNECTION_H

// Framework include files
#include "GaudiUtils/IIODataManager.h" // for IDataConnection class definition
#include <string>
#include <map>

// Forward declarations
class TFile;
class TClass;
class TBranch;
class DataObject;
class IRegistry;

/*
 *  LHCb namespace declaration
 */
namespace Gaudi  {
  // Forward declarations
  class RootRef;

  /** @class RootDataConnection RootDataConnection.h GAUDIROOT/RootDataConnection.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    20/12/2009
    */
  class RootDataConnection : virtual public Gaudi::IDataConnection  {
  protected:
    /// Type definition for string maps
    typedef std::map<int,std::string> StringMap;
    TFile*     m_file;
    StringMap  m_dbs;
    StringMap  m_conts;
    StringMap  m_links;
    StringMap  m_paths;

  public:
    /// Standard constructor
    RootDataConnection(const IInterface* own, const std::string& nam);
    /// Standard destructor      
    virtual ~RootDataConnection();
    /// Direct access to TFile structure
    TFile* file() const                     {     return m_file;      }
    /// Check if connected to data source
    virtual bool isConnected() const        {     return m_file != 0; }
    /// Open data stream in read mode
    virtual StatusCode connectRead();
    /// Open data stream in write mode
    virtual StatusCode connectWrite(IoType typ);
    /// Release data stream
    virtual StatusCode disconnect();
    /// Read root byte buffer from input stream
    virtual StatusCode read(void* const data, size_t len);
    /// Write root byte buffer to output stream
    virtual StatusCode write(const void* data, int len);
    /// Seek on the file described by ioDesc. Arguments as in ::seek()
    virtual long long int seek(long long int where, int origin);
    /// Access data branch by name
    TBranch* getBranch(const std::string& n);
    TBranch* getBranch(TClass* cl, const std::string& n);
    void makeRef(IRegistry* pA, RootRef& ref);
    int makePath(const std::string& p);
    const std::string& getPath(int which);
    const std::string& getDb(int which);
    const std::string& getCont(int which);
    const std::string& getLink(int which);
  };
}         // End namespace Gaudi
#endif    // GAUDIROOT_ROOTDATACONNECTION_H
