// $Id: CondDBDispatcherSvc.h,v 1.4 2007-05-11 10:04:56 marcocle Exp $
#ifndef COMPONENT_CONDDBDISPATCHERSVC_H 
#define COMPONENT_CONDDBDISPATCHERSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "DetCond/ICondDBReader.h"
#include <vector>
#include <map>

template <class TYPE> class SvcFactory;

/** @class CondDBDispatcherSvc CondDBDispatcherSvc.h component/CondDBDispatcherSvc.h
 *  
 *
 *
 *  @author Marco Clemencic
 *  @date   2006-07-10
 */
class CondDBDispatcherSvc: public virtual Service,
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
  CondDBDispatcherSvc( const std::string& name, ISvcLocator* svcloc ); 

  virtual ~CondDBDispatcherSvc( ); ///< Destructor


private:

  ICondDBReader *alternativeFor(const std::string &path);

  // -------------------- Data Members

  /// Property CondDBDispatcherSvc.MainAccessSvc: the AccessSvc instance to use to retrieve all the
  /// objects for which an alternative is not specified (default to "CondDBAccessSvc").
  std::string m_mainAccessSvcName;

  /// Property CondDBDispatcherSvc.Alternatives: list of alternative Access Services in the form of
  /// "/path/for/alternative=ServiceType/ServiceName".
  std::vector<std::string> m_alternativesDeclaration;
  
  /// Pointer to the main access service.
  ICondDBReader* m_mainDB;
  
  /// Container fo the alternatives.
  std::map<std::string,ICondDBReader*> m_alternatives;

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<CondDBDispatcherSvc>;

};
#endif // COMPONENT_CONDDBDISPATCHERSVC_H
