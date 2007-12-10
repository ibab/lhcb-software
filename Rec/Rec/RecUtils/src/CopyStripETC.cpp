// $Id: CopyStripETC.cpp,v 1.3 2007-12-10 16:27:46 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IRegistry.h" 
#include "GaudiKernel/DataTypeInfo.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/SelResult.h"
// ============================================================================
// local
// ============================================================================
#include "CopyStripETC.h"
// ============================================================================
/** @file 
 *  Implementation file for class CopyStripETC
 *  @date 2006-06-26 
 *  @autho Patrick Koppenburg
 */
// ============================================================================
namespace 
{
  /// get the actual type from NTuple item
  template <class TYPE> struct _GetTypeFromItem ;
  /// get the actual type from NTuple item
  template <class TYPE> 
  struct _GetTypeFromItem<NTuple::Item<TYPE> >
  {
    typedef TYPE Type ;
  };
  // the actual type of "interesting" column
  typedef Tuples::TupleObj::Int MyItem         ; ///< the type of "good" column
  // the helper constant
  const DataTypeInfo::Type s_value = 
  DataTypeInfo::ID( _GetTypeFromItem<MyItem>::Type() ) ; ///< "good constant"
}
// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
CopyStripETC::CopyStripETC
( const std::string& name,
  ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_intType( s_value )
{
  declareProperty
    ( "InputCollectionName" , m_inputCollectionName = "TagCreator" ) ;
  declareProperty
    ( "OutputCollectionName", m_outputCollectionName = "<not set>" ) ; 
}
// ============================================================================
// Destructor
// ============================================================================
CopyStripETC::~CopyStripETC() {}
// ============================================================================
// Initialization
// ============================================================================
StatusCode CopyStripETC::initialize() 
{
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg
  
  if ( m_outputCollectionName != "<not set>" ) {
    info() << "Will be writing out an ETC to " << m_outputCollectionName << endmsg ;
  } else {
    err() << "No output ETC to be written" << endmsg ;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}
// =============================================================================
// Main execution
// =============================================================================
StatusCode CopyStripETC::execute() 
{
  
  debug() << "==> Execute" << endmsg ;
  StatusCode sc = StatusCode::SUCCESS ;
  
  // it's a typedef SmartDataPtr<NTuple::Tuple> NTuplePtr
  NTuplePtr ntIN(evtColSvc(), "EventSelector.DataStreamTool_1/"+m_inputCollectionName+"/1" );
  
  if ( !ntIN ) {
    err() << "Nothing at " << "EventSelector.DataStreamTool_1/"+m_inputCollectionName+"/1" << endmsg ;
    return StatusCode::FAILURE ;
  } else {
    if (sc) sc = fillOutputETC(ntIN);
  }
  if (sc) setFilterPassed(true);
  
  return sc ;
}
// ============================================================================
// Filling of Output ETC
// ============================================================================
StatusCode CopyStripETC::fillOutputETC(NTuplePtr& ntIN) const
{
  // This part is stolen from Hugo's StripETC
  Tuple ntOUT = evtCol(m_outputCollectionName);
  // pick up the location of the event - this is what makes the tag collection a collection...
  DataObject* pObject = get<DataObject>("/Event");
  if (pObject!=0) {
    ntOUT->column("Address", pObject->registry()->address() ); 
  } else {
    Error("    not able to retrieve IOpaqueAddress");
  }
  // That's new: the rest is just copying whatever is in ntIN
  const INTuple::ItemContainer ic = ntIN->items() ;
  debug() << "There are " << ic.size() << " items in nTuple" << endmsg ;
  for ( INTuple::ItemContainer::const_iterator i=ic.begin() ; i!=ic.end() ; ++i)
  {
    if ( m_intType == (*i)->type() )
    {
      MyItem val;
      StatusCode status = ntIN->item((*i)->name(), val);
      if ( status ) debug() << "Copying item ``" << (*i)->name() << "'' of type " 
                            << (*i)->typeName() << " and value " << val << endmsg ;
      else {
        err() << "Could not read value of " << *i << endmsg ;
        return StatusCode::FAILURE;
      }
      ntOUT->column((*i)->name(), val );
    } else if ( (*i)->typeName()!="IOpaqueAddress*") {
      Warning("Not copying field ``"+(*i)->name()+"'' of unexpected type ``"+(*i)->typeName()+"''");
    }
  }
  //  ntOUT->write();
  
  return StatusCode::SUCCESS ;  
}
// =============================================================================
//  Finalize
// =============================================================================
StatusCode CopyStripETC::finalize() {
  
  debug() << "==> Finalize" << endmsg;
  
  return GaudiTupleAlg::finalize();  // must be called after all other actions
}
// ============================================================================
/// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyStripETC )
// =============================================================================
// The END 
// =============================================================================
