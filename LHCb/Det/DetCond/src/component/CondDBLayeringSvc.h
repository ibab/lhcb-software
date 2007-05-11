// $Id: CondDBLayeringSvc.h,v 1.3 2007-05-11 10:04:56 marcocle Exp $
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

  /// Retrieve the names of the children nodes of a FolderSet.
  virtual StatusCode getChildNodes (const std::string &path, std::vector<std::string> &node_names);

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

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<CondDBLayeringSvc>;

};
#endif // COMPONENT_CONDDBLAYERINGSVC_H
