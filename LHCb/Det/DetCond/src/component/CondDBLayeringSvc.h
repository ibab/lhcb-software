// $Id: CondDBLayeringSvc.h,v 1.5 2008-06-26 14:22:45 marcocle Exp $
#ifndef COMPONENT_CONDDBLAYERINGSVC_H 
#define COMPONENT_CONDDBLAYERINGSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "DetCond/ICondDBReader.h"
#include <vector>

template <class TYPE> class SvcFactory;

/** @class CondDBLayeringSvc CondDBLayeringSvc.h component/CondDBLayeringSvc.h
 *  
 *
 *  @author Marco CLEMENCIC
 *  @date   2006-07-14
 */
class CondDBLayeringSvc: public virtual Service,
                         public virtual ICondDBReader {
public: 

  /** Query interfaces of Interface
      @param riid       ID of Interface to be retrieved
      @param ppvUnknown Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  /// Initilize COOL (CondDB) Access Layer Service
  virtual StatusCode initialize();
  /// Finalize Service
  virtual StatusCode finalize();

  // --------- ICondDBReader implementation

  /// Try to retrieve an object from the Condition DataBase. If path points to a FolderSet,
  /// channel and when are ignored and data is set ot NULL.
  virtual StatusCode getObject (const std::string &path, const Gaudi::Time &when,
                                DataPtr &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel = 0);

  /// Try to retrieve an object from the Condition DataBase. If path points to a FolderSet,
  /// channel and when are ignored and data is set ot NULL.
  virtual StatusCode getObject (const std::string &path, const Gaudi::Time &when,
                                DataPtr &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, const std::string &channel);

  /// Retrieve the names of the children nodes of a FolderSet.
  virtual StatusCode getChildNodes (const std::string &path, std::vector<std::string> &node_names);
  
  /// Retrieve the names of the children nodes of a FolderSet divided in folders and foldersets.
  virtual StatusCode getChildNodes (const std::string &path,
                                    std::vector<std::string> &folders,
                                    std::vector<std::string> &foldersets);

  /// Tells if the path is available in the database.
  virtual bool exists(const std::string &path);
  
  /// Tells if the path (if it exists) is a folder.
  virtual bool isFolder(const std::string &path);
  
  /// Tells if the path (if it exists) is a folderset.
  virtual bool isFolderSet(const std::string &path);

  // --------- ICondDBInfo implementation

  /** Get the current default database tags
   *  @param  tags vector of DB name, tag pairs. Empty if DB not available
   */ 
  virtual void defaultTags( std::vector<LHCb::CondDBNameTagPair>& tags) const;

protected:

  /// Standard constructor
  CondDBLayeringSvc( const std::string& name, ISvcLocator* svcloc ); 

  virtual ~CondDBLayeringSvc( ); ///< Destructor

protected:

private:

  // -------------------- Data Members

  /// Property CondDBLayeringSvc.Layers: list of Access Service layers.
  /// They will be searched from the first to the last.
  std::vector<std::string> m_layersNames;

  /// Container fo the alternatives.
  std::vector<ICondDBReader*> m_layers;

  /// Enable/disable direct mapping from the database structure to the transient
  /// store using XML persistency format (enabled by default).
  bool m_xmlDirectMapping;

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<CondDBLayeringSvc>;

};
#endif // COMPONENT_CONDDBLAYERINGSVC_H
