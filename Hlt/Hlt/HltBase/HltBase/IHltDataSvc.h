#ifndef IHLTDATASVC_H
#define IHLTDATASVC_H
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// GaudiKrrnel
// ============================================================================
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltTypes.h"
// ============================================================================
// forward declarations 
// ============================================================================
class stringKey  ;
class IAlgorithm ;
namespace Hlt { class Selection ; }
// ============================================================================
class IHltDataSvc : virtual public INamedInterface 
{
public:
  // ==========================================================================
  //@TODO: have two interfaces: one to only 'query' the structure, the other to
  //       get access to everything, the latter inheriting from the former...
  //@TODO: extend the query capabilities: who uses what as input, who produced which output..
  //@TODO add interface for retrieving dependency graph of selections
  //      implementations could save this as a property...
  //@TODO: move the {register,retrieve}{,T}selection overhere...
  virtual StatusCode addSelection ( Hlt::Selection*   sel             ,
                                    const IAlgorithm* parent          , 
                                    const bool        fromTES = false ) = 0 ;
  virtual bool hasSelection ( const stringKey& id ) const = 0;
  //@TODO: make typesafe, i.e. allow getting of derived classes 
  //@TODO: return pointer instead of reference -- 0 being don't know id...
  //@TODO: should only have const access to requested input...
  virtual /* const*/  Hlt::Selection* selection ( const stringKey&  id     , 
                                            const IAlgorithm* parent ) =0 ;
  //template <typename T>
  //Hlt::TSelection<T>& selection( const stringKey& id, const IAlgorithm* parent) {
      //Hlt::Selection* sel = &selection(id,parent);
      //return sel->template down_cast<T>();
  //}
  // 
  virtual std::vector<stringKey> selectionKeys() const = 0;
  // 
  virtual StatusCode inputUsedBy(const stringKey& key, std::vector<std::string>& inserter) const =0;
  // ==========================================================================
public:
  // ============================================================================
  /// the unique interface identifier  
  static const InterfaceID& interfaceID();
  // ============================================================================  
protected:
  // ==========================================================================
  virtual ~IHltDataSvc();
  // ==========================================================================  
};
// ============================================================================
// The END  
// ============================================================================
#endif
// ============================================================================

