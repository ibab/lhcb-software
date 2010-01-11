// $Id: RootDataConnection.h,v 1.3 2010-01-11 17:13:39 frankb Exp $
#ifndef GAUDIROOT_ROOTDATACONNECTION_H
#define GAUDIROOT_ROOTDATACONNECTION_H

// Framework include files
#include "GaudiUtils/IIODataManager.h" // for IDataConnection class definition
#include <string>
#include <map>

#include "TFile.h"
// Forward declarations
class TTree;
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
    typedef std::vector<std::string> StringMap;
    TFile*     m_file;
    StringMap  m_dbs;
    StringMap  m_conts;
    StringMap  m_links;
    StringMap  m_paths;
    std::string m_empty;

    typedef std::map<std::string,TTree*> Sections;
    Sections m_sections;
    TTree   *m_refs;
    TBranch *m_dbBranch, *m_cntBranch, *m_lnkBranch, *m_pathBranch;
  public:
    /// Standard constructor
    RootDataConnection(const IInterface* own, const std::string& nam);
    /// Standard destructor      
    virtual ~RootDataConnection();
    /// Direct access to TFile structure
    TFile* file() const                     {     return m_file;      }
    /// Check if connected to data source
    virtual bool isConnected() const        {     return m_file != 0; }
    /// Is the file writable?
    bool isWritable() const {  return m_file != 0 && m_file->IsWritable(); }
    /// Open data stream in read mode
    virtual StatusCode connectRead();
    /// Open data stream in write mode
    virtual StatusCode connectWrite(IoType typ);
    /// Release data stream
    virtual StatusCode disconnect();
    /// Read root byte buffer from input stream
    virtual StatusCode read(void* const, size_t)   { return StatusCode::FAILURE; }
    /// Write root byte buffer to output stream
    virtual StatusCode write(const void*, int)     { return StatusCode::FAILURE; }
    /// Seek on the file described by ioDesc. Arguments as in ::seek()
    virtual long long int seek(long long int, int) { return -1; }

    StatusCode readRefs();
    StatusCode saveRefs();

    TTree* getSection(const std::string& sect, bool create=false);

    /// Access data branch by name
    TBranch* getBranch(const std::string& sect,const std::string& n);
    TBranch* getBranch(const std::string& sect, const std::string& n, TClass* cl);
    void makeRef(IRegistry* pA, RootRef& ref);
    int makePath(const std::string& p);

    const std::string& getPath(int which) const {
      return (which>=0)&&(size_t(which)<m_paths.size()) ? *(m_paths.begin()+which) : m_empty;
    }
    
    const std::string& getDb(int which) const {
      return (which>=0)&&(size_t(which)<m_dbs.size()) ? *(m_dbs.begin()+which) : m_empty;
    }
    
    const std::string& getCont(int which) const {
      return (which>=0)&&(size_t(which)<m_conts.size()) ? *(m_conts.begin()+which) : m_empty;
    }
    
    const std::string& getLink(int which) const {
      return (which>=0)&&(size_t(which)<m_links.size()) ? *(m_links.begin()+which) : m_empty;
    }
  };
}         // End namespace Gaudi
#endif    // GAUDIROOT_ROOTDATACONNECTION_H
