#ifndef IHLTDATASVC_H
#define IHLTDATASVC_H
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// forward declarations 
// ============================================================================
class IAlgorithm ;
namespace Gaudi { class StringKey  ; }
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
  virtual bool hasSelection ( const Gaudi::StringKey& id ) const = 0;
  //@TODO: should only have const access to requested input...
  virtual const  Hlt::Selection* selection ( const Gaudi::StringKey&  id     , 
                                             const IAlgorithm* parent ) =0 ;
  //@TODO: make typesafe, i.e. allow getting of derived classes 
  //template <typename T>
  //Hlt::TSelection<T>& selection( const Gaudi::StringKey& id, const IAlgorithm* parent) {
      //Hlt::Selection* sel = selection(id,parent);
      //return sel->template down_cast<T>();
  //}
  // 
  virtual std::vector<Gaudi::StringKey> selectionKeys() const = 0;
  // 
  virtual StatusCode inputUsedBy(const Gaudi::StringKey& key, std::vector<std::string>& inserter) const =0;
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
