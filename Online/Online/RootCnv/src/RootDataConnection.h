// $Id: RootDataConnection.h,v 1.5 2010-08-17 17:16:24 frankb Exp $
#ifndef GAUDIROOT_ROOTDATACONNECTION_H
#define GAUDIROOT_ROOTDATACONNECTION_H

// Framework include files
#include "GaudiUtils/IIODataManager.h" // for IDataConnection class definition
#include <string>
#include <vector>
#include <map>

#include "TFile.h"
#include "RootRefs.h"

// Forward declarations
class TTree;
class TClass;
class TBranch;
class TTreePerfStats;
class DataObject;
class IRegistry;
class DataObject;

/*
 *  Gaudi namespace declaration
 */
namespace Gaudi  {
  // Forward declarations
  class RootRef;
  class RootObjectRefs;


  class RootConnectionSetup {
  protected:
    /// Standard destructor      
    virtual ~RootConnectionSetup();
    /// Object refrfence count
    int refCount;
    /// Reference to message service
    IMessageSvc* m_msgSvc;

  public:
    /// RootCnvSvc Property: Root data cache size
    int          cacheSize;
    /// RootCnvSvc Property: ROOT cache learn entries
    int          learnEntries;
    /// RootCnvSvc Property: AutoFlush parameter for ROOT TTree (Number of events between auto flushes)
    int          autoFlush;

    /// Standard constructor
    RootConnectionSetup();
    /// Increase reference count
    void addRef();
    /// Decrease reference count
    void release();
    /// Set message service reference
    void setMessageSvc(IMessageSvc* m);
    /// Retrieve message service
    IMessageSvc* msgSvc() const {  return m_msgSvc; }
  };

  /** @class RootDataConnection RootDataConnection.h GAUDIROOT/RootDataConnection.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    20/12/2009
    */
  class RootDataConnection : virtual public Gaudi::IDataConnection  {
  public:
    /// Type definition for string maps
    typedef std::vector<std::string>                                 StringVec;
    /// Type definition for the parameter map
    typedef std::vector<std::pair<std::string,std::string> >         ParamMap;
    /// Type definition for the chain map
    typedef std::map<std::string, std::vector<std::pair<int,int> > > ChainMap;
    /// Definition of tree sections
    typedef std::map<std::string,TTree*>                             Sections;

    /// Allow access to printer service
    IMessageSvc* msgSvc() const {  return m_setup->msgSvc(); }


  protected:
    /// Reference to ROOT file
    TFile*               m_file;
    /// Map containing external database file names (fids)
    StringVec            m_dbs;
    /// Map containing external container names
    StringVec            m_conts;
    /// Map containing internal links names
    StringVec            m_links;
    /// Parameter map for file parameters
    ParamMap             m_params;
    /// Buffer for empty string reference
    std::string          m_empty;
    /// Reference to the setup structure 
    RootConnectionSetup* m_setup;

    /// I/O read statistics from TTree
    TTreePerfStats*      m_statistics;

    /// Tree sections in TFile
    Sections m_sections;
    /// Pointer to the reference tree
    TTree   *m_refs;

    /// Empty string reference
    const std::string& empty() const;

    /// Internal helper to save/update reference tables
    StatusCode saveRefs();

  public:
    /** @class Tool RootDataConnection.h src/RootDataConnection.h
     *
     * Helper class to facilitate an abstraction layer for reading
     * POOL style files with this package.
     * YES: This class obsoletes POOL.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Tool {
    protected:
      typedef RootDataConnection::StringVec StringVec;
      typedef RootDataConnection::ParamMap  ParamMap;
      /// Pointer to containing data connection object
      RootDataConnection* c;
    public:
      TTree* refs()              const { return c->m_refs;    }
      StringVec& dbs()           const { return c->m_dbs;     }
      StringVec& conts()         const { return c->m_conts;   }
      StringVec& links()         const { return c->m_links;   }
      ParamMap&  params()        const { return c->m_params;  }
      IMessageSvc* msgSvc()      const { return c->msgSvc();  }
      const std::string& name()  const { return c->m_name;    }

      /// Default destructor
      virtual ~Tool() {}
      /// Use releasePtr() helper to delete object
      virtual void release() { delete this; }
      /// Access data branch by name: Get existing branch in read only mode
      virtual TBranch* getBranch(const std::string&  section, const std::string& n) = 0;

      /// Read references section when opening data file
      virtual StatusCode readRefs() = 0;
      /// Save references section when closing data file
      virtual StatusCode saveRefs() = 0;
      /// Load references object
      virtual int loadRefs(const std::string& section, const std::string& cnt, unsigned long entry, RootObjectRefs& refs) = 0;
    } *m_tool;
    friend class Tool;

    /// Create file access tool to encapsulate POOL compatibiliy
    Tool* makeTool();

  public:

    /// Standard constructor
    RootDataConnection(const IInterface* own, const std::string& nam, RootConnectionSetup* setup);
    /// Standard destructor      
    virtual ~RootDataConnection();

    /// Direct access to TFile structure
    TFile* file() const                     {     return m_file;      }
    /// Check if connected to data source
    virtual bool isConnected() const        {     return m_file != 0; }
    /// Is the file writable?
    bool isWritable() const                 {  return m_file != 0 && m_file->IsWritable(); }
    /// Enable TTreePerStats
    void enableStatistics(const std::string& section);
    /// Save TTree access statistics if required
    void saveStatistics(const std::string& statisticsFile);

    /// Load object
    int loadObj(const std::string& section, const std::string& cnt, unsigned long entry, DataObject*& pObj);

    /// Load references object
    int loadRefs(const std::string& section, const std::string& cnt, unsigned long entry, RootObjectRefs& refs);

    /// Save object of a given class to section and container
    std::pair<int,unsigned long> saveObj(const std::string& section,const std::string& cnt, TClass* cl, DataObject* pObj);
    /// Save object of a given class to section and container
    std::pair<int,unsigned long> save(const std::string& section,const std::string& cnt, TClass* cl, void* pObj);


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
    /// Create reference object from values
    void makeRef(const std::string& name, long clid, int tech, const std::string& db, const std::string& cnt, int entry, RootRef& ref);

    /// Convert path string to path index
    int makeLink(const std::string& p);

    /// Access database/file name from saved index
    const std::string& getDb(int which) const;
    
    /// Access container name from saved index
    const std::string& getCont(int which) const 
      { return (which>=0)&&(size_t(which)<m_conts.size()) ? *(m_conts.begin()+which) : empty();    }
    
    /// Access link name from saved index
    const std::string& getLink(int which) const 
      { return (which>=0)&&(size_t(which)<m_links.size()) ? *(m_links.begin()+which) : empty();    }
  };
}         // End namespace Gaudi
#endif    // GAUDIROOT_ROOTDATACONNECTION_H
