// $Id: RootOutputStream.h,v 1.1 2010-09-03 08:52:08 frankb Exp $
#ifndef GAUDISVC_PERSISTENCYSVC_OUTPUTSTREAM_H
#define GAUDISVC_PERSISTENCYSVC_OUTPUTSTREAM_H


// Required for inheritance
#include "GaudiKernel/Algorithm.h"

// STL include files
#include <memory>
#include <vector>
#include <string>
#include <set>

// forward declarations
template <class ConcreteAlgorithm> class AlgFactory;
namespace { template <class P, class S> class Factory; }

class IIncidentSvc;
class IConversionSvc;
class IDataManagerSvc;
class DataStoreItem;


/*
 * Gaudi namespace declaration
 */
namespace Gaudi {

  // Forward declarations
  class RootCnvSvc;

  /** A small class to stream Data I/O.
   *
   *  Author:  M.Frank
   *  author R. Lambert
   *  Version: 1.0
   */
  class RootOutputStream : public Algorithm     {
    friend class AlgFactory<RootOutputStream>;
    friend class Factory<RootOutputStream,IAlgorithm* (std::string,ISvcLocator *)>;

  public:
    typedef std::vector<DataStoreItem*> Items;
    typedef std::vector<std::string>    ItemNames;
  protected:
    /// Reference to the indicent service
    SmartIF<IIncidentSvc>     m_incidentSvc;
    /// Flag to indicate that item consistency should be checked
    bool                      m_verifyItems;
    /// Name of the service managing the data store
    std::string               m_storeName;
    /// Name of the persistency service capable to write data from the store
    std::string               m_persName;
    /// Name of the output file specification
    std::string               m_output;
    /// Name of the output file
    std::string               m_outputName;
    /// Output type: NEW(NEW,CREATE,WRITE,RECREATE), UPDATE)
    std::string               m_outputType;
    /// Keep reference to the data provider service
    SmartIF<IDataProviderSvc> m_pDataProvider;
    /// Keep reference to the data manager service
    SmartIF<IDataManagerSvc>  m_pDataManager;
    /// Keep reference to the data conversion service
    SmartIF<IConversionSvc>   m_pConversionSvc;
    RootCnvSvc*               m_cnvSvc;
    /// Vector of item names
    ItemNames                 m_itemNames;
    /// Vector of items to be saved to this stream
    Items                     m_itemList;
    /// Vector of item names
    ItemNames                 m_optItemNames;
    /// Vector of optional items to be saved to this stream
    Items                     m_optItemList;
    /// Number of events written to this output stream
    int                       m_events;
    /// Vector of names of Algorithms that this stream accepts
    StringArrayProperty       m_acceptNames;
    /// Vector of names of Algorithms that this stream requires
    StringArrayProperty       m_requireNames;
    /// Vector of names of Algorithms that this stream is vetoed by
    StringArrayProperty       m_vetoNames;
    /// Vector of Algorithms that this stream accepts
    std::vector<Algorithm*>   m_acceptAlgs;
    /// Vector of Algorithms that this stream requires
    std::vector<Algorithm*>   m_requireAlgs;
    /// Vector of Algorithms that this stream is vetoed by
    std::vector<Algorithm*>   m_vetoAlgs;

  public:
    /// Standard algorithm Constructor
    RootOutputStream(const std::string& name, ISvcLocator* pSvcLocator);
    /// Standard Destructor
    virtual ~RootOutputStream();

  protected:
    /// Decode list of Algorithms that this stream accepts
    StatusCode decodeAcceptAlgs( );
    /// Handler for AcceptAlgs Property
    void acceptAlgsHandler( Property& theProp );
    /// Decode list of Algorithms that this stream requires
    StatusCode decodeRequireAlgs( );
    /// Handler for RequireAlgs Property
    void requireAlgsHandler( Property& theProp );
    /// Decode list of Algorithms that this stream is vetoed by
    StatusCode decodeVetoAlgs( );
    /// Handler for VetoAlgs Property
    void vetoAlgsHandler( Property& theProp );
    /// Decode specified list of Algorithms
    StatusCode decodeAlgorithms( StringArrayProperty& theNames,
				 std::vector<Algorithm*>* theAlgs );
    /// Test whether this event should be output
    bool isEventAccepted( ) const;
    /// Find single item identified by its path (exact match)
    DataStoreItem* findItem(const std::string& path);
    /// Select the different objects and write them to file
    virtual StatusCode writeObjects();
    /// Select the different objects and write them to file
    virtual StatusCode writeObjects(const Items& items,std::set<DataObject*>& unique, bool required);
    /// Select the different objects and write them to file
    virtual StatusCode writeObjectRefs(const Items& items,std::set<DataObject*>& unique, bool required);

    ///should I fire incidents for writing opening/closing etc?
    bool m_fireIncidents;

    /// Tell if the instance has been configured with input items or not.
    virtual bool hasInput() const;

  public:
    /// Initialize RootOutputStream
    virtual StatusCode initialize();
    /// Terminate RootOutputStream
    virtual StatusCode finalize();
    /// Working entry point
    virtual StatusCode execute();
    // Connect to proper conversion service
    virtual StatusCode connectConversionSvc();
    /// Clear item list
    void clearItems(Items& itms);
    /// Add item to output streamer list
    void addItem(Items& itms, const std::string& descriptor);
  };
}

#endif // GAUDISVC_PERSISTENCYSVC_OUTPUTSTREAM_H
