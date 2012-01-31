// $Id: $
// Include files

// local
#include "EmptyEventNodeCleaner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EmptyEventNodeCleaner
//
// 2012-01-31 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( EmptyEventNodeCleaner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EmptyEventNodeCleaner::EmptyEventNodeCleaner( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputStream", m_inputStream = "/Event" );
}

//=============================================================================
// Destructor
//=============================================================================
EmptyEventNodeCleaner::~EmptyEventNodeCleaner() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode EmptyEventNodeCleaner::execute()
{
  // Root of the TES
  DataObject * root = get<DataObject*>( m_inputStream );

  // recursively clean
  if ( root ) cleanNodes( root, m_inputStream );

  return StatusCode::SUCCESS;
}

//=============================================================================

void EmptyEventNodeCleaner::cleanNodes( DataObject * obj,
                                        const std::string & location,
                                        unsigned int nRecCount )
{
  // protect against infinite recursion
  if ( ++nRecCount > 99999 ) 
  {
    Error( "Maximum recursion limit reached...." ).ignore();
    return;
  }

  SmartIF<IDataManagerSvc> mgr( eventSvc() );
  typedef std::vector<IRegistry*> Leaves;
  Leaves leaves;

  // Load the leaves
  StatusCode sc = mgr->objectLeaves( obj, leaves );
  if ( sc )
  {

    if ( !leaves.empty() )
    {
      for ( Leaves::const_iterator iL = leaves.begin(); leaves.end() != iL; ++iL )
      {
        const std::string & id = (*iL)->identifier();
        DataObject* tmp(NULL);
        sc = eventSvc()->findObject( id, tmp );
        if ( sc && tmp ) 
        {
          if ( CLID_DataObject == tmp->clID() )
          {
            cleanNodes( tmp, id, nRecCount );
          }
        }
      }
      // Load again, after cleaning, to see what remains
      sc = mgr->objectLeaves( obj, leaves );
    }

    if ( sc && leaves.empty() )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Removing node " << location << endmsg;
      sc = eventSvc()->unlinkObject(location);
      //sc = eventSvc()->unregisterObject(obj);
      //delete obj;
    }

  }

}
