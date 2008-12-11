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
  //@TODO: move the {register,retrieve}{,T}selection overhere...
  virtual StatusCode addSelection 
  ( Hlt::Selection*   sel             ,
    const IAlgorithm* parent          , 
    const bool        fromTES = false ) = 0 ;
  virtual bool hasSelection ( const stringKey& id ) const = 0;
  //@TODO: make typesafe, i.e. allow getting of derived classes 
  virtual Hlt::Selection& selection 
  ( const stringKey&  id     , 
    const IAlgorithm* parent ) =0 ;
  // 
  virtual std::vector<stringKey> selectionKeys() = 0;
  // 
  virtual Hlt::Configuration& config() = 0;
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

