// $Id: TrendingTool.cpp,v 1.6 2010-09-17 09:23:13 frankb Exp $
// Include files
#include <ctime>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TrendingTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrendingTool
//
// 2010-06-08 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrendingTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrendingTool::TrendingTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_forWriting( false )
  , m_firstDirAddress( -1 )
  , m_tagAddressInFile( -1 )
  , m_dirAddressInFile( -1 )
  , m_dataAddressInFile( -1 )
  , m_file( NULL )
  , m_hasSelection( false )
  , m_requestedTagNumber( -1 )
  , m_maxTime(  0xffffffff )   // maximum time value...
  , m_lastTime( 0xffffffff )
  , m_firstTimeThisTag( 0 )
{
  declareInterface<ITrendingTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
TrendingTool::~TrendingTool() {
  if ( NULL != m_file ) closeFile();
}

//=========================================================================
//  Open the Trending file for writing
//=========================================================================
bool TrendingTool::openWrite( std::string name, std::vector<std::string> tags ) {
  return m_trendWriter.openWrite( name, tags );
}
//=========================================================================
//  Set the thresholds for all variables
//=========================================================================
bool TrendingTool::setThresholds ( std::vector<float> thr ) {
  return m_trendWriter.setThresholds( thr );
}

//=========================================================================
//  Set the threshold for a single tag
//=========================================================================
bool TrendingTool::setThreshold ( std::string tag, float thr) {
  return m_trendWriter.setThreshold( tag, thr );
}

//=========================================================================
//  Set the averaging time
//=========================================================================
void TrendingTool::setAverageTime ( unsigned int seconds  ) {
  m_trendWriter.setAverageTime( seconds );  
}

//=========================================================================
//  Set the max time without write
//=========================================================================
void TrendingTool::setMaxTimeNoWrite ( unsigned int seconds  ) {
  m_trendWriter.setMaxTimeNoWrite( seconds );  
}

//=========================================================================
//  Add an entry for a single tag, mainly for averaging
//=========================================================================
void TrendingTool::addValue( std::string tag, float value ) {
  m_trendWriter.addValue( tag, value );
}

//=========================================================================
//  Store an event in the file
//=========================================================================
bool TrendingTool::write( std::vector<float> data, unsigned int now ) {
  return m_trendWriter.write( data, now );
}

//=========================================================================
// Close the file
//=========================================================================
void TrendingTool::closeFile() {
  m_trendWriter.closeFile();
  
  if ( 0 != m_file ) {
    int status = fclose( m_file );
    if ( 0 != status ) error() << "fclose returned status " << status << std::endl;
    m_file = NULL;
  }
  m_firstDirAddress    = -1;
  m_tagAddressInFile   = -1;
  m_dirAddressInFile   = -1;
  m_dataAddressInFile  = -1;
  m_hasSelection       = false;
  m_requestedTagNumber = -1;
  m_lastTime           = 0xffffffff;
  m_maxTime            = 0xffffffff;
  m_firstTimeThisTag   = 0;
}
//=========================================================================
// Open an existing file for reading
//=========================================================================
bool TrendingTool::openRead( std::string name ) {

  if ( NULL != m_file ) {
    error() << "openRead: file already opened!" << endmsg;
    return false;
  }

  std::string fileFullName =  name + ".trend";
  if ( name[0] != '/' ) fileFullName = TREND_AREA + name + ".trend";

  verbose() << "openRead file name = " << fileFullName << endmsg;

  //== Open the existing file
  std::string mode = "r";
  m_file = fopen( fileFullName.c_str(), mode.c_str() );
  if ( 0 == m_file ) {
    error() << "openRead: Can not open file " << fileFullName << endmsg;
    return false;
  }
  if ( !loadTags( -1 ) ) return false;   // Get the latest tags, return an error if...
  m_forWriting = false;

  return true;
}
//=========================================================================
//  Select the first record with the appropriate time, set the end time
//=========================================================================
bool TrendingTool::select( unsigned int startTime, unsigned int endTime, std::string tag ) {
  m_lastTime = 0;
  if ( NULL == m_file ) {
    error() << "select: file not opened!" << endmsg;
    return false;
  }
  if ( 0 == endTime ) endTime = (int)::time( 0 );

  if ( startTime > endTime ) {
    error() << "select: End time '" << timeString( endTime ) << "' is before startTime '"
            << timeString( startTime ) << "'" << endmsg;
    return false;
  }

  //== If a tag is given, search for it and gets its index
  m_tagNumber = -1;
  if ( "" != tag ) {
    for ( std::vector<std::string>::const_iterator itS = m_tags.begin(); m_tags.end() != itS ; ++itS ) {
      if ( (*itS) == tag ) m_tagNumber = itS - m_tags.begin();
    }
    if ( 0 > m_tagNumber ) {
      error () << "select: Requested tag '" << tag << "' not found." << endmsg;
      return false;
    }
  }

  m_hasSelection = true;

  debug() << "select: startTime " << startTime << " first in dir " <<  m_dir.entry[0].firstTime << endmsg;

  if ( !getDataContaining( startTime ) ) return false;

  debug() << "Skip until startTime, current data time = " << m_data.data[m_ptData].i << endmsg;
  while ( m_data.data[m_ptData].i < startTime &&
          m_data.data[m_ptData].i > 0 ) unpackAnEvent();  // Skip entries before

  m_lastTime = startTime;
  m_hasSelection = true;
  m_maxTime = endTime;
  return true;
}
//=========================================================================
//  Get the next event block.
//=========================================================================
bool TrendingTool::nextEvent( unsigned int& time, std::vector<float>& data ) {

  if ( !m_hasSelection ) {
    error() << "nextEvent: no previous selection!" << endmsg;
    return false;
  }
  //== Do we need to read the next block
  if ( 0 ==  m_data.data[m_ptData].i ) {
    if ( !nextDataBlock() ) return false;
  }

  //== is it after the last wanted time...
  if ( m_maxTime < m_data.data[m_ptData].i ) {
    m_hasSelection = false;
    return false;
  }

  //== Make sure the output has enough memory...
  if ( data.size() < m_lastData.size() ) {
    data.resize( m_lastData.size(), 0. );
  }

  if ( m_lastTime != 0 ) {
    time  = m_lastTime;
    for ( unsigned int kk = 0; m_lastData.size() > kk; ++kk ) {
      data[kk] = m_lastData[kk];
    }
    m_lastTime = 0;
    return true;
  }

  time = m_data.data[m_ptData++].i;
  int ptMask = m_ptData;
  m_ptData  += m_nbMask;
  int mask = 0;
  for ( int kk = 0 ; m_nbTag > kk ; ++kk ) {
    if ( 0 == kk%32 ) mask = m_data.data[ptMask++].i;
    if ( mask & 1 ) {
      data[kk] = m_data.data[m_ptData++].f;
      m_lastData[kk] = data[kk];
    } else {
      data[kk] = m_lastData[kk];
    }
    mask = mask>>1;
  }
  verbose() << "nextEvent: Returned event at time " << time << endmsg;

  return true;
}

//=========================================================================
// Get the next real value for the selected tag.
//=========================================================================
bool TrendingTool::nextValue( unsigned int& time, float& value ) {

  if ( !m_hasSelection ) {
    error() << "nextValue: no previous selection." << endmsg;
    return false;
  }
  if ( 0 > m_tagNumber ) {
    error() << "nextValue: no tag selected" << endmsg;
    return false;
  }

  bool notFound = true;
  while( notFound ) {

    //== Do we need to read the next block
    if ( 0 ==  m_data.data[m_ptData].i ) {
      verbose() << "Get next data block " << endmsg;
      if ( !nextDataBlock() ) {
        verbose() << "nextValue: No new data block" << endmsg;
        m_hasSelection = false;
        return false;
      }
    }
    verbose() << "nextValue: at ptData " << m_ptData << " time " << m_data.data[m_ptData].i
              << " max " << m_maxTime << endmsg;

    //== is it after the last wanted time...
    if ( m_maxTime < m_data.data[m_ptData].i ) {
      m_hasSelection = false;
      return false;
    }

    if ( m_lastTime != 0 ) {
      time  = m_lastTime;
      value = m_lastData[m_tagNumber];
      m_lastTime = 0;
      return true;
    }

    time = m_data.data[m_ptData++].i;
    int ptMask = m_ptData;
    m_ptData  += m_nbMask;
    int mask = 0;
    for ( int kk = 0 ; m_nbTag > kk ; ++kk ) {
      if ( 0 == kk%32 ) {
        mask = m_data.data[ptMask++].i;
      }
      if ( mask & 1 ) {
        if ( m_tagNumber == kk ) {
          notFound = false;
          value = m_data.data[m_ptData].f;
        }
        m_ptData++;
      }
      mask = mask>>1;
    }
  }
  return true;
}
//=========================================================================
// Get the tags for the specified version.
//=========================================================================
bool TrendingTool::tags( std::vector<std::string>& tags, int version ) {

  if ( NULL == m_file ) {
    error() << "tags: file not opened!" << endmsg;
    return false;
  }
  if ( loadTags( version ) ) {
    tags = m_tags;
    return true;
  }
  return false;
}
//=========================================================================
//  Read the latest tags..
//=========================================================================
bool TrendingTool::loadTags( int wantedVersion ) {

  if ( NULL == m_file ) {
    error() << "loadTags: file not opened!" << endmsg;
    return false;
  }
  long nextTagAddress = 0;
  m_tagHeader.version = -999;
  while ( m_tagHeader.version != wantedVersion ) {
    m_tagAddressInFile = nextTagAddress;
    fseek( m_file, nextTagAddress, SEEK_SET );
    int nRead = fread( &m_tagHeader, 1, sizeof( Header ), m_file );
    if ( sizeof( Header ) != nRead ) {
      error() << "openWrite: Error reading the header of the tag block: read " << nRead << " bytes" << endmsg;
      return false;
    }
    verbose() << "openWrite: tag header: size " << m_tagHeader.size << " type " << m_tagHeader.type
              << " version " << m_tagHeader.version << " next " << m_tagHeader.nextAddress << endmsg;
    nextTagAddress = m_tagHeader.nextAddress;
    if ( 0 == nextTagAddress ) break;
  }
  int nToRead = m_tagHeader.size - sizeof( Header );

  char* temp = (char*)malloc( nToRead );
  int nn = fread( temp, 1, nToRead, m_file );
  if ( nn != nToRead ) {
    error() << "openWrite: reading tags, need " << nToRead << " bytes, received " << nn << endmsg;
    free( temp );
    return false;
  }

  m_firstDirAddress = ftell( m_file );  //== First directory block is after tag block.

  m_tags.clear();
  m_thresholds.clear();
  m_lastData.clear();
  char* p = temp;
  while ( 0 < nn ) {
    std::string tag( p );
    int ll = strlen( p ) + 1;
    if ( 0 == ll ) break;
    p  += ll;
    nn -= ll;
    m_tags.push_back( tag );
    m_thresholds.push_back( 0. );
    m_lastData.push_back( 0. );
    verbose() << "loadTags: loaded tag " << tag << " n byte left = " << nn << endmsg;
  }
  m_nbTag = m_tags.size();
  m_nbMask = (m_nbTag/32) + 1;

  free( temp );  // release memory...

  //== read the first directory record...
  m_dirAddressInFile = m_firstDirAddress;
  fseek( m_file, m_dirAddressInFile, SEEK_SET );
  fread( &m_dir, 1, sizeof(DirectoryRecord), m_file );
  m_firstTimeThisTag = m_dir.entry[0].firstTime;
  m_dataAddressInFile = -1;

  debug() << "loadTags: Found " << m_tags.size()
          << " tags for version " << m_tagHeader.version << endmsg;

  return true;
}

//=========================================================================
//  Get the next data block with adjusted pointers. false if no more data
//  Directory record should be OK... Internal function.
//=========================================================================
bool TrendingTool::nextDataBlock ( ) {
  verbose() << "nextDataBlock: ptDir " << m_ptDir << " nbEntries " << m_dir.nbEntries << endmsg;

  if ( m_ptDir == m_dir.nbEntries - 1 ) {
    if ( 0 == m_dir.nextAddress ) return false;  // no more data
    m_dirAddressInFile = m_dir.nextAddress;
    fseek( m_file, m_dirAddressInFile, SEEK_SET );
    fread( &m_dir, 1, sizeof(DirectoryRecord), m_file );
    m_ptDir = 0;
    verbose() << "nextDataBlock: read dir block at " << m_dirAddressInFile << " nbEntries " << m_dir.nbEntries << endmsg;
  } else {
    m_ptDir++;
  }
  if ( 0 == m_dir.entry[m_ptDir].firstTime ) return false;

  m_dataAddressInFile = m_dirAddressInFile + m_dir.entry[m_ptDir].fileOffset;
  fseek( m_file, m_dataAddressInFile, SEEK_SET );
  fread( &m_data, 1, sizeof(DataRecord), m_file );
  m_ptData = 0;
  if ( 0 == m_data.data[0].i ) return false;
  return true;
}

//=========================================================================
//  Convert the time int to a printable string.
//=========================================================================
std::string TrendingTool::timeString ( unsigned int time ) {
  std::string fmt = "%Y-%m-%d %H:%M:%S";
  char result[40];
  time_t tmp(time);
  strftime( result, 40, fmt.c_str(), localtime( &tmp ) );
  return std::string( result );
}

//=========================================================================
//  Find the proper directory and data block containing a given time.
//=========================================================================
bool TrendingTool::getDataContaining ( unsigned int time ) {
  //== Is it before the first entry in the current directory?
  if ( time < m_dir.entry[0].firstTime ) {
    if ( m_dirAddressInFile != m_firstDirAddress ) {
      m_dirAddressInFile = m_firstDirAddress;
      fseek( m_file, m_dirAddressInFile, SEEK_SET );
      fread( &m_dir, 1, sizeof(DirectoryRecord), m_file );
    }
  }
  m_ptDir = 0;
  if ( 0 == m_dir.entry[0].firstTime ) return false;  // empty file

  while ( 0 !=   m_dir.entry[m_ptDir+1].firstTime &&
          time > m_dir.entry[m_ptDir+1].firstTime    ) {
    debug() << "getDataContaining: m_ptDir " << m_ptDir << " time(ptdir+1) " <<  m_dir.entry[m_ptDir+1].firstTime << endmsg;
    if ( m_ptDir == m_dir.nbEntries-1 ) {
      if ( 0 == m_dir.nextAddress ) return false;  // no more data
      m_dirAddressInFile = m_dir.nextAddress;
      fseek( m_file, m_dirAddressInFile, SEEK_SET );
      fread( &m_dir, 1, sizeof(DirectoryRecord), m_file );
      m_ptDir = 0;
    } else {
      m_ptDir++;
    }
  }
  if ( m_dataAddressInFile != m_dirAddressInFile + m_dir.entry[m_ptDir].fileOffset ) {
    m_dataAddressInFile = m_dirAddressInFile + m_dir.entry[m_ptDir].fileOffset;
    fseek( m_file, m_dataAddressInFile, SEEK_SET );
    fread( &m_data, 1, sizeof( DataRecord ), m_file );
  }
  m_ptData = 0;
  return true;
}

//=========================================================================
//  Unpack an event in the data block. return the time of the event
//=========================================================================
unsigned int TrendingTool::unpackAnEvent ( ) {
  unsigned int time = m_data.data[m_ptData++].i;
  int ptMask = m_ptData;
  m_ptData  += m_nbMask;
  int mask = 0;
  for ( int kk = 0 ; m_nbTag > kk ; ++kk ) {
    if ( 0 == kk%32 ) mask = m_data.data[ptMask++].i;
    if ( mask & 1 ) {
      m_lastData[kk] = m_data.data[m_ptData++].f;
    }
    mask = mask>>1;
  }
  return time;
}

//=========================================================================
// Return the version of the tags. 
//=========================================================================
int TrendingTool::tagVersion ( ) {
  return m_tagHeader.version;
}

//=========================================================================
//  
//=========================================================================
unsigned int TrendingTool::firstTimeThisTag( ) {
  return m_firstTimeThisTag;
}
//=============================================================================
