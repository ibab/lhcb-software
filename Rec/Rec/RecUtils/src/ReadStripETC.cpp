// $Id: ReadStripETC.cpp,v 1.4 2007-09-07 12:21:13 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// event model
#include "Event/SelResult.h"

// local
#include "ReadStripETC.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ReadStripETC
//
// 2006-06-26 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ReadStripETC );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ReadStripETC::ReadStripETC( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_usedSelectionsFilled(false)
  , m_longType(8)
    , m_writeSelResult(0)
{
  m_vetoedFields.push_back("entry");
  m_vetoedFields.push_back("nChargedProto");
  m_vetoedFields.push_back("nCharged");
  m_vetoedFields.push_back("nPrim");
  
  declareProperty( "CollectionName", m_inputCollectionName = "TagCreator");
  declareProperty( "VetoedFields", m_vetoedFields);
  declareProperty( "Selections", m_selections); /// empty means all

}
//=============================================================================
// Destructor
//=============================================================================
ReadStripETC::~ReadStripETC() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ReadStripETC::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg

  debug() << "==> Initialize" << endmsg;
  if ( m_selections.empty() ) {
    info() << "Will be writing out SelResults for all selections" ;
    if ( !(m_vetoedFields.empty()) ){ 
      info() << " except " ;
      for ( std::vector<std::string>::const_iterator i = m_vetoedFields.begin() ;
            i != m_vetoedFields.end(); ++i ) info() << *i << " " ;
      info() << "." << endmsg ;
    }
  } else {
    info() << "Will be writing out SelResults for  " ;
    for ( std::vector<std::string>::const_iterator i = m_selections.begin() ;
          i != m_selections.end(); ++i ) info() << *i << " " ;
    info() << endmsg ;
  }

  m_writeSelResult = tool<IWriteSelResult>("WriteSelResult"); // not private

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ReadStripETC::execute() {

  debug() << "==> Execute" << endmsg ;
  StatusCode sc = StatusCode::SUCCESS ;

  // it's a typedef SmartDataPtr<NTuple::Tuple> NTuplePtr
  NTuplePtr ntIN(evtColSvc(), "EventSelector.DataStreamTool_1/"+m_inputCollectionName+"/1" );

  if ( !ntIN ) {
    err() << "Nothing at " << "EventSelector.DataStreamTool_1/"+m_inputCollectionName+"/1" << endmsg ;
    return StatusCode::FAILURE ;
  } else {
    if ( !m_usedSelectionsFilled ) sc = fillUsedSelections(ntIN);
    if (sc) sc = fillSelResults(ntIN); 
  }
  if (sc) setFilterPassed(true);

  return sc ;
}
//=============================================================================
// First filling of to be used fields
//=============================================================================
StatusCode ReadStripETC::fillUsedSelections(NTuplePtr& ntIN){
  m_usedSelectionsFilled = true ;
  const INTuple::ItemContainer ic = ntIN->items() ;
  debug() << "There are " << ic.size() << " items in nTuple" << endmsg ;

  bool taken = false ;
  for ( INTuple::ItemContainer::const_iterator i=ic.begin() ; i!=ic.end() ; ++i){
    verbose() << "There is item " << (*i)->name() << " of type " << (*i)->typeName() << endmsg ;
    if ( (*i)->type()==m_longType ){ // long
      if ( m_selections.empty() ){
        taken = true ;
        for ( std::vector<std::string>::const_iterator iv = m_vetoedFields.begin() ;
              iv != m_vetoedFields.end(); ++iv ) {
          if ( (*i)->name() == *iv ) taken = false ;
        }
      } else {
        for ( std::vector<std::string>::const_iterator is = m_selections.begin() ;
              is != m_selections.end(); ++is ) {
          if ( (*i)->name() == *is ) taken = true ;
        }
      }
      if (taken) {    
        debug() << "  Taking " << (*i)->name() << endmsg ;
        m_usedSelections.push_back((*i)->name());
      } else {
        debug() << "Skipping " << (*i)->name() << endmsg ;
      }
    }
  }
  info() << "Will be writing out SelResults for  " ;
  for ( std::vector<std::string>::const_iterator i = m_usedSelections.begin() ;
        i != m_usedSelections.end(); ++i ) info() << *i << " " ;
  info() << endmsg ;
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Filling of SelResults
//=============================================================================
StatusCode ReadStripETC::fillSelResults(NTuplePtr& ntIN)const{
  for ( std::vector<std::string>::const_iterator i = m_usedSelections.begin() ;
        i != m_usedSelections.end(); ++i ) {
    NTuple::Item<long> val;
    StatusCode status = ntIN->item(*i, val);
    if ( status ) debug() << "Value of " << *i << " is " << val << endmsg ;
    else {
      err() << "Could not read value of " << *i << endmsg ;
      return StatusCode::FAILURE;
    }
    m_writeSelResult->write(*i,(val!=0));
  }
  return StatusCode::SUCCESS ;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode ReadStripETC::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
