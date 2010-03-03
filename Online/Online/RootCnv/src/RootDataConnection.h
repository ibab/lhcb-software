// $Id: RootDataConnection.h,v 1.4 2010-03-03 14:30:47 frankb Exp $
#ifndef GAUDIROOT_ROOTDATACONNECTION_H
#define GAUDIROOT_ROOTDATACONNECTION_H

// Framework include files
#include "GaudiUtils/IIODataManager.h" // for IDataConnection class definition
#include <string>
#include <vector>
#include <map>

#include "TFile.h"

// Forward declarations
class TTree;
class TClass;
class TBranch;
class TTreePerfStats;
class DataObject;
class IRegistry;

/*
 *  Gaudi namespace declaration
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
    /// Type definition for the parameter map
    typedef std::vector<std::pair<std::string,std::string> > ParamMap;
    /// Type definition for the chain map
    typedef std::map<std::string, std::vector<std::pair<int,int> > > ChainMap;

    /// Reference to ROOT file
    TFile*      m_file;
    /// Map containing external database file names (fids)
    StringMap   m_dbs;
    /// Map containing external container names
    StringMap   m_conts;
    /// Map containing internal links names
    StringMap   m_links;
    /// Map containing internal object pathes
    StringMap   m_paths;
    /// Parameter map for file parameters
    ParamMap    m_params;
    /// Buffer for empty string reference
    std::string m_empty;
    /// Root data cache size
    int         m_cacheSize;
    /// ROOT cache learn entries
    int         m_learn;

    /// Name of perfstat file
    std::string     m_statisticsFile;
    /// I/O read statistics from TTree
    TTreePerfStats* m_statistics;

    typedef std::map<std::string,TTree*> Sections;
    Sections m_sections;
    TTree   *m_refs;
    TBranch *m_dbBranch, *m_cntBranch, *m_lnkBranch, *m_pathBranch, *m_paramBranch, *m_chainBranch;

    /// Internal helper to read reference tables
    StatusCode readRefs();
    /// Internal helper to save/update reference tables
    StatusCode saveRefs();


  public:

    /// Standard constructor
    RootDataConnection(const IInterface* own, const std::string& nam, int cache_size, int learn);
    /// Standard destructor      
    virtual ~RootDataConnection();

    /// Direct access to TFile structure
    TFile* file() const                     {     return m_file;      }
    /// Check if connected to data source
    virtual bool isConnected() const        {     return m_file != 0; }
    /// Is the file writable?
    bool isWritable() const                 {  return m_file != 0 && m_file->IsWritable(); }
    /// Enable TTreePerStats
    void enableStatistics(const std::string& section, const std::string& fname="");
    /// Save TTree access statistics if required
    void saveStatistics();

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

    /// Access TTree section from section name. The section is created if required.
    TTree* getSection(const std::string& sect, bool create=false);

    /// Access data branch by name: Get existing branch in read only mode
    TBranch* getBranch(const std::string& sect,const std::string& n);
    /// Access data branch by name: Get existing branch in write mode
    TBranch* getBranch(const std::string& sect,const std::string& n,const CLID& clid);
    /// Access data branch by name: Get existing branch in write mode
    TBranch* getBranch(const std::string& sect, const std::string& n, TClass* cl);

    /// Create reference object from registry entry
    void makeRef(IRegistry* pA, RootRef& ref);

    /// Convert path string to path index
    int makePath(const std::string& p);

    /// Access path name from saved index
    const std::string& getPath(int which) const {
      return (which>=0)&&(size_t(which)<m_paths.size()) ? *(m_paths.begin()+which) : m_empty;
    }
    
    /// Access database/file name from saved index
    const std::string& getDb(int which) const {
      return (which>=0)&&(size_t(which)<m_dbs.size()) ? *(m_dbs.begin()+which) : m_empty;
    }
    
    /// Access container name from saved index
    const std::string& getCont(int which) const {
      return (which>=0)&&(size_t(which)<m_conts.size()) ? *(m_conts.begin()+which) : m_empty;
    }
    
    /// Access link name from saved index
    const std::string& getLink(int which) const {
      return (which>=0)&&(size_t(which)<m_links.size()) ? *(m_links.begin()+which) : m_empty;
    }
  };
}         // End namespace Gaudi
#endif    // GAUDIROOT_ROOTDATACONNECTION_H
