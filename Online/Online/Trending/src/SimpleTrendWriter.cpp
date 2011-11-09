// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "SimpleTrendWriter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SimpleTrendWriter
//
// 2011-03-24 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SimpleTrendWriter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimpleTrendWriter::SimpleTrendWriter( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_trend( 0 )
  , m_fileName( "" )
  , m_fileIsOpen( false )
  , m_tagChanged( false )
{
  declareInterface<ISimpleTrendWriter>(this);
}
//=============================================================================
// Destructor
//=============================================================================
SimpleTrendWriter::~SimpleTrendWriter() {}

//=========================================================================
//  Initialize: Retieve the tool
//=========================================================================
StatusCode SimpleTrendWriter::initialize( ) {
  StatusCode sc = GaudiTool::initialize();
  if ( !sc ) return sc;
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Set partition and name. Open file and load latest tags
//=========================================================================
void SimpleTrendWriter::setPartitionAndName( std::string& partition, std::string& name ) {
  m_fileName = partition + "_" + name;
  if (0 == m_trend)
  {
    SmartIF<IToolSvc> tools;
    serviceLocator()->service("ToolSvc", tools.pRef());
//    m_trend = tool<ITrendingTool>(m_fileName );
    tools->retrieveTool("TrendingTool","m_fileName",m_trend,this);
  }
  if ( 0 == m_trend ) return ;

  if ( m_fileIsOpen ) close();
  bool status = m_trend->openRead( m_fileName );
  if ( status ) {
    m_trend->tags( m_tags );  // get the tags;
    m_values.resize( m_tags.size(), 0. );
    m_tagChanged = false;

    //== get the most recent values.

    unsigned int start = m_trend->firstTimeThisTag();
    if ( m_trend->select( start ) ) {
      while( m_trend->nextEvent( start, m_values ) ) { }
    }
    m_trend->closeFile();
  }
}
//=========================================================================
//  Start to collect data for a new event
//=========================================================================
void SimpleTrendWriter::startEvent( ) {
  m_tagChanged = false;
}

//=========================================================================
//  Add a new value for an existing tag.
//=========================================================================
void SimpleTrendWriter::addEntry( std::string tag, double value ) {
  for ( unsigned int i = 0; m_tags.size() > i; ++i ) {
    if ( tag == m_tags[i] ) {
      m_values[i] = float(value);
      return;
    }
  }
  error() << "Add new tag ='" << tag << "' value " << value << endmsg;
  m_tags.push_back( tag );
  m_values.push_back( float(value) );
  m_tagChanged = true;
}

//=========================================================================
//  Set the time for averaging
//=========================================================================
void SimpleTrendWriter::setAverageTime( unsigned int seconds ) {
  m_trend->setAverageTime( seconds );
}

//=========================================================================
//  Set maximum time without writing
//=========================================================================
void SimpleTrendWriter::setMaxTimeNoWrite ( unsigned int seconds) {
  m_trend->setMaxTimeNoWrite( seconds );
}
//=========================================================================
//  Write the event in the file. Update the tags if needed.
//=========================================================================
void SimpleTrendWriter::saveEvent ( ) {
  if ( m_tagChanged ) {
    info() << "Closing and reopening file as tags have changed." << endmsg;
    if ( m_fileIsOpen ) close();
  }

  if ( !m_fileIsOpen ) {
    m_fileIsOpen = m_trend->openWrite( m_fileName, m_tags );
    if ( !m_fileIsOpen ) {
      error() << "Can't open file " << m_fileName << " for writing"
              << " Tag size " << m_tags.size() << endmsg;
      return;
    }
  }
  m_trend->write( m_values );
}

//=========================================================================
//  Close the file and setthe appropriate flag
//=========================================================================
void SimpleTrendWriter::close ( ) {
  if ( m_fileIsOpen ) m_trend->closeFile();
  m_fileIsOpen = false;
}
//=============================================================================
