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
  , m_maxTime(  0x7fffffff )   // maximum time value...
  , m_lastTime( 0x7fffffff )
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
bool TrendingTool::openWrite( std::string name, std::vector<std::string> tags, int version ) {

  if ( NULL != m_file ) {
    error() << "openWrite: file already opened!" << endmsg;
    return false;
  }

  //== Check that the tags are unique
  bool isBad = false;
  for ( std::vector<std::string>::const_iterator itS = tags.begin(); tags.end() > itS+1; ++itS ) {
    for ( std::vector<std::string>::const_iterator itS1 = itS+1; tags.end() != itS1 ; ++itS1 ) {
      if ( *itS == *itS1 ) {
        error() << "openWrite: tag " << itS-tags.begin() << " and "
                << itS1-tags.begin() << " are identical : " << *itS << endmsg;
        isBad = true;
      }
    }
  }
  if ( isBad ) return true;
  unsigned int maxSize = (unsigned int)(0.1 * DATA_SIZE);
  if ( maxSize < tags.size() ) {
    error() << "Requested to store " << tags.size() << " tags, maximum allowed " << maxSize << endmsg;
    return false;
  }  

  char* prefix = getenv( TREND_AREA );
  std::string fileFullName =  name + ".trend";
  if ( 0 != prefix ) fileFullName = std::string( prefix ) + fileFullName;

  verbose() << "openWrite file name = " << fileFullName << endmsg;

  //== Try top open an existing file, else create one.
  std::string mode = "r+";
  m_file = fopen( fileFullName.c_str(), mode.c_str() );
  if ( 0 == m_file ) {
    mode = "w+";
    m_file = fopen( fileFullName.c_str(), mode.c_str() );
    if ( 0 == m_file ) {
      error() << "openWrite: Can not create file " << fileFullName << endmsg;
      return false;
    }
  } else {
    if ( !loadTags( -1 ) ) return false;   // Get the latest tags, return an error if...
  }
  m_forWriting = true;

  //== First check for consistency: Same tags and version, or new version.

  bool isDifferent = false;
  unsigned int kk;
  if ( tags.size() != m_tags.size() ) {
    verbose() << "Request " << tags.size() << " tags, file has " << m_tags.size() << endmsg;
    isDifferent = true;
  } else {
    for ( kk = 0; tags.size() > kk; ++kk ) {
      if ( tags[kk] != m_tags[kk] ) isDifferent = true;
    }
  }
  if ( 0 >= version && version != m_tagHeader.version ) isDifferent = true;

  if ( !isDifferent ) return true;

  //== Is the requested vesion higher?
  if ( version <= m_tagHeader.version ) {
    error() << "openWrite: The list of tags is incompatible with the one in the file. "
            << "Specify a new version greater than " << m_tagHeader.version << endmsg;
    return false;
  }
  debug() << "Create new version of the tag record." << endmsg;

  //== Get to end of file and create new tag record.

  fseek( m_file, 0, SEEK_END );
  long newAddress = ftell( m_file );
  m_tagHeader.type    = TYPE_TAG;
  m_tagHeader.version = short(version);
  m_tagHeader.nextAddress = 0L;

  //== Compute the length of the tag strings, and copy them in a temp buffer
  int tagLength = 0;
  for ( kk = 0; tags.size() > kk; ++kk ) tagLength += ( tags[kk].size() + 1 );
  char* tagBuffer = (char*) malloc( tagLength );
  char* p = tagBuffer;
  m_tags.clear();
  m_thresholds.clear();
  m_lastData.clear();
  for ( kk = 0; tags.size() > kk; ++kk ) {
    m_tags.push_back( tags[kk] );
    m_thresholds.push_back( 0. );     // default threshold
    m_lastData.push_back( 0. );       // previous value of data
    strcpy( p, tags[kk].c_str() );
    p = p + strlen( p ) + 1;
  }
  m_nbTag = m_tags.size();
  m_nbMask = (m_nbTag/32) + 1;

  //== Write the new tag record
  m_tagHeader.size    = sizeof( Header ) + tagLength;
  fwrite( &m_tagHeader, sizeof( Header ), 1, m_file );
  fwrite( tagBuffer, 1, tagLength, m_file );
  debug() << "openWrite: Written " << tagLength << " bytes of tag + "
          << sizeof( Header ) << " header bytes." << endmsg;
  free( tagBuffer );

  m_firstDirAddress = ftell( m_file );  //== First directory block is after tag block.

  //== Update the previous tag record if any, now that the new one is on file.
  if ( newAddress != 0 ) {
    verbose() << "End of file at " << newAddress
              << " read back header at " << m_tagAddressInFile << endmsg;
    fseek( m_file, m_tagAddressInFile, SEEK_SET );
    Header tagHeader;
    int nRead = fread( &tagHeader, 1, sizeof( Header ), m_file );
    if ( sizeof( Header ) != nRead ) {
      error() << "openWrite: Error reading the header of the tag block: read " << nRead << " bytes" << endmsg;
      return false;
    }
    tagHeader.nextAddress = newAddress;
    fseek( m_file, m_tagAddressInFile, SEEK_SET );
    fwrite( &tagHeader, 1, sizeof( Header ), m_file );
    fseek( m_file, newAddress, SEEK_SET );
  }
  fflush( m_file);  // force writing on disk.

  m_tagAddressInFile = newAddress;

  return true;

}
//=========================================================================
//  Set the thresholds for all variables
//=========================================================================
bool TrendingTool::setThresholds ( std::vector<float> thr ) {
  if ( thr.size() != m_thresholds.size() ) {
    error() << "setThresholds: Need " << m_thresholds.size()
            << " values while " << thr.size() << " given. ** IGNORED **"
            << endmsg;
    return false;
  }
  for ( unsigned int kk = 0; thr.size() > kk ; ++kk ) {
    m_thresholds[kk] = thr[kk];
  }
  return true;
}

//=========================================================================
//  Set the threshold for a single tag
//=========================================================================
bool TrendingTool::setThreshold ( std::string tag, float thr) {
  for ( unsigned int kk = 0 ; m_thresholds.size() > kk ; ++kk ) {
    if ( m_tags[kk] == tag ) {
      m_thresholds[kk] = thr;
      debug() << "setThreshold: Threshold for '" << tag << "' set to " << thr << endmsg;
      return true;
    }
  }
  error() << "setThreshold: Can't change threshold for '" << tag << "' : not found" << endmsg;
  return false;
}

//=========================================================================
//  Store an event in the file
//=========================================================================
bool TrendingTool::write( std::vector<float> data, int now ) {

  if ( NULL == m_file || !m_forWriting ) {
    error() << "write: File not opened for writing!" << endmsg;
    return false;
  }

  if ( 0 != m_tagHeader.nextAddress ) {
    error() << "Writing is only possible for tags of the highest version" << endmsg;
    return false;
  }
    
  if ( 0 == now ) now = (int)::time( 0 );

  //== Get the last entry before that time. First open the first dir if needed.
  if ( 0 > m_dirAddressInFile ) {
    debug() << "write: Read first directory block at " << m_firstDirAddress << endmsg;
    m_dirAddressInFile = m_firstDirAddress;
    m_dir.size = 0;
    fseek( m_file, m_dirAddressInFile, SEEK_SET );
    fread( &m_dir, 1, sizeof(DirectoryRecord), m_file );
    if ( 0 == m_dir.size ) {       // No directory block. Create one...
      fseek( m_file, m_dirAddressInFile, SEEK_SET );
      createDirectoryRecord( &m_dir, now );
    }
  }
  debug() << "write: Directory address in file = " << m_dirAddressInFile
          << " first time in dir " << m_dir.entry[0].firstTime
          << " want time = " << now << endmsg;

  //== Check if the time is accessible from the current directory, i.e. after first and before last
  bool hasDataAfter = false;
  if ( now < m_lastTime ) {   // New entry is not after the last written -> Find where to write
    getDataContaining( now );

    if ( 0 != m_dir.entry[m_ptDir+1].firstTime ) {
      hasDataAfter = true;
    }
    if ( 0 == m_dir.entry[m_ptDir].firstTime ) { // No data block at all
      m_dataAddressInFile = ftell( m_file );  // As we have just read the directory, this is the free position...
      m_data.size = 0;
    } else {
      while ( m_data.data[m_ptData].i < now &&
              m_data.data[m_ptData].i > 0 ) unpackAnEvent( );
      if ( m_data.data[m_ptData].i != 0 ) hasDataAfter = true;
    }
  }
  
  //== We are positionned after the last entry before. If no data after, this is simple.

  verbose() << "write: ptDir = " << m_ptDir << " first time " << m_dir.entry[m_ptDir].firstTime
            << " nbEntries in dir " << m_dir.nbEntries
            << " PtData " << m_ptData << " next time " <<  m_data.data[m_ptData].i
            << " hasDataAfter " << hasDataAfter
            << endmsg;

  if ( !hasDataAfter ) {
    writeEntry( now, data, true );

  } else {   //== Data after: Save context, read all data, and rewrite them after.

    long savedDirAdd  = m_dirAddressInFile;
    long savedDataAdd = m_dataAddressInFile;
    int  savedPtDir   = m_ptDir;
    int  savedPtData  = m_ptData;

    std::vector<int> savedTime;
    std::vector< std::vector<float> > savedData;
    m_hasSelection = true;
    m_maxTime = 0x7fffffff;
    int tTime;
    std::vector<float> tData( m_nbTag );
    while( nextEvent( tTime, tData ) ) {
      savedTime.push_back( tTime );
      savedData.push_back( tData );
    }
    debug() << "Saved " << savedTime.size() << " records after " << now << endmsg;

    m_dirAddressInFile  = savedDirAdd;
    m_dataAddressInFile = savedDataAdd;
    m_ptDir  = savedPtDir;
    m_ptData = savedPtData;
    fseek( m_file, m_dirAddressInFile, SEEK_SET );
    fread( &m_dir, 1, sizeof(DirectoryRecord), m_file );
    //== Clear the directory as if there was no data after
    m_dir.nbEntries = m_ptDir+1;
    m_dir.nextAddress = 0L;
    for ( int jj = m_ptDir+1 ; MAX_ENTRY > jj ; ++jj ) {
      m_dir.entry[jj].firstTime = 0;
      m_dir.entry[jj].fileOffset = 0;
    }
    fseek( m_file, m_dataAddressInFile, SEEK_SET );
    fread( &m_data, 1, sizeof(DataRecord), m_file );
    m_data.data[m_ptData].i = 0;  // indicate no data...
    m_data.size = sizeof( int ) * (m_ptData+3);   // +3 as 8 bytes header + 1 extra int with a zero...

    writeEntry( now, data, false );
    for ( unsigned int kk = 0 ; kk < savedTime.size() ; ++kk ) {
      writeEntry( savedTime[kk], savedData[kk], false );
    }
    //== Write on disk the last block...
    fseek( m_file, m_dataAddressInFile, SEEK_SET );
    fwrite( &m_data, 1, sizeof(DataRecord), m_file );
    fseek( m_file, m_dirAddressInFile , SEEK_SET );
    fwrite( &m_dir, sizeof(DirectoryRecord), 1, m_file );
  }
  fflush( m_file);  // force writing on disk.
  return true;
}

//=========================================================================
//  Write one entry at the current position in the file.
//=========================================================================
void TrendingTool::writeEntry ( int now, std::vector<float>& data, bool forceWrite ) {
  if ( 0 != m_dir.entry[m_ptDir].firstTime ) {  // There is some data block...

    //== If we don't have enough space, create a new data record.
    if ( m_data.size > sizeof(DataRecord) - (m_nbTag+m_nbMask+2)* sizeof(int) ) {
      if ( !forceWrite) {
        fseek( m_file, m_dataAddressInFile, SEEK_SET );
        fwrite( &m_data, 1, sizeof(DataRecord), m_file );
      }
      m_dataAddressInFile = ftell( m_file );  // After the last data block
      m_data.size = 0;
      m_ptData    = 0;
      m_ptDir++;
      debug() << "writeEntry: Start a new data block, ptDir = " << m_ptDir << endmsg;
      if ( m_ptDir == MAX_ENTRY-1 ) {
        long newDirAddress = m_dataAddressInFile;  // This is the next free in file, as obtained above.
        // Create a new fresh directrory block at the current location
        DirectoryRecord newDir;
        createDirectoryRecord( &newDir, now );

        // update old dir to have the chaining, include tag of first in next
        m_dir.nextAddress  = newDirAddress;
        m_dir.entry[m_ptDir].firstTime = now;
        fseek( m_file, m_dirAddressInFile , SEEK_SET );
        fwrite( &m_dir, sizeof(DirectoryRecord), 1, m_file );

        // re-read the new dir block to be positioned for the next data.
        m_dirAddressInFile = newDirAddress;
        fseek( m_file, m_dirAddressInFile, SEEK_SET );
        fread( &m_dir, 1, sizeof(DirectoryRecord), m_file );
        m_dataAddressInFile = ftell( m_file );
        m_ptDir = 0;
        m_dir.nbEntries = 0;
      }
      m_dir.nbEntries++;
      m_dir.entry[m_ptDir].firstTime = now;
      m_dir.entry[m_ptDir].fileOffset = m_dataAddressInFile - m_dirAddressInFile;
      if ( forceWrite ) {
        fseek( m_file, m_dirAddressInFile, SEEK_SET );
        fwrite( &m_dir, 1, sizeof(DirectoryRecord), m_file );
      }
    }
  }

  debug() << "writeEntry: Length of data block at "
          << m_dataAddressInFile << " = " << m_data.size << " m_ptData " << m_ptData << endmsg;

  bool full = ( 0 == m_ptData );
  addDataEntry( now, data, forceWrite, full );
}
//=========================================================================
//  Write a data item, with zero suppression if needed
//=========================================================================
void TrendingTool::addDataEntry( int now, std::vector<float>& data, bool forceWrite, bool full ){
    
  m_data.data[m_ptData++].i = now;
  int mask = 0;
  int maskAddr = m_ptData;
  int nbItem = 0;
  m_ptData += m_nbMask;  // first word to write
  for ( unsigned int kk = 0 ;  data.size() > kk ; ++kk ) {
    if ( full || fabs( data[kk] - m_lastData[kk] ) > m_thresholds[kk] ) {
      mask |= (1 << (kk%32));
      m_data.data[m_ptData++].f = data[kk];
      m_lastData[kk] = data[kk];
      nbItem++;
    }
    if ( (kk%32) == 31 ) {
      m_data.data[maskAddr++].i = mask;
      mask = 0;
    }
  }
  if ( (data.size()%32) != 31 ) m_data.data[maskAddr].i = mask;
  m_data.data[m_ptData].i = 0;   // tag the end of the data block!

  verbose() << "addDataEntry: Selected " << nbItem << " data to write, full = " << full << endmsg;

  if ( 0 < nbItem ) {
    m_lastTime          = now;
    m_data.size         = sizeof( int ) * (m_ptData+3);   // +3 as 8 bytes header + 1 extra int with a zero...
    m_data.type         = TYPE_DATA;
    m_data.version      = m_tagHeader.version;
    if ( forceWrite ) {
      fseek( m_file, m_dataAddressInFile, SEEK_SET );
      fwrite( &m_data, 1, sizeof(DataRecord), m_file );
      verbose() << "-- written " << m_data.size << " bytes  at " << m_dataAddressInFile << endmsg;
    }
  } else {
    m_ptData = m_ptData - m_nbMask - 1;  // restore the data pointer.
    m_data.data[m_ptData].i = 0;   // tag the end of the data block!
  }
}

//=========================================================================
//  Get the next event block.
//=========================================================================
bool TrendingTool::nextEvent( int& time, std::vector<float>& data ) {

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
// Close the file
//=========================================================================
void TrendingTool::closeFile() {
  if ( 0 != m_file ) {
    int status = fclose( m_file );
    if ( 0 != status ) error() << "fclose returned status " << status << std::endl;
    m_file = NULL;
  }
}
//=========================================================================
// Open an existing file for reading
//=========================================================================
bool TrendingTool::openRead( std::string name ) {

  if ( NULL != m_file ) {
    error() << "openRead: file already opened!" << endmsg;
    return false;
  }

  char* prefix = getenv( TREND_AREA );
  std::string fileFullName =  name + ".trend";
  if ( 0 != prefix ) fileFullName = std::string( prefix ) + fileFullName;

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

  //== read the first directory record...
  m_dirAddressInFile = m_firstDirAddress;
  fseek( m_file, m_dirAddressInFile, SEEK_SET );
  fread( &m_dir, 1, sizeof(DirectoryRecord), m_file );
  
  return true;
}
//=========================================================================
//  Select the first record with the appropriate time, set the end time
//=========================================================================
bool TrendingTool::select( int startTime, int endTime, std::string tag ) {

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
  while ( m_data.data[m_ptData].i < startTime ) unpackAnEvent();  // Skip entries before

  m_hasSelection = true;
  m_maxTime = endTime;
  return true;
}
//=========================================================================
// Get the next real value for the selected tag.
//=========================================================================
bool TrendingTool::nextValue( int& time, float& value ) {

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
    fseek( m_file, nextTagAddress, SEEK_SET );
    int nRead = fread( &m_tagHeader, 1, sizeof( Header ), m_file );
    if ( sizeof( Header ) != nRead ) {
      error() << "openWrite: Error reading the header of the tag block: read " << nRead << " bytes" << endmsg;
      return false;
    }
    m_tagAddressInFile = ftell( m_file ) - sizeof( Header );
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
std::string TrendingTool::timeString ( int time ) {
  std::string fmt = "%Y-%m-%d %H:%M:%S";
  char result[40];
  time_t tmp(time);
  strftime( result, 40, fmt.c_str(), localtime( &tmp ) );
  return std::string( result );
}

//=========================================================================
// Create an empty directory record. Tag the first entry...
//=========================================================================
void TrendingTool::createDirectoryRecord ( DirectoryRecord* dir, int now ) {
  dir->size        = sizeof( DirectoryRecord );
  dir->type        = TYPE_DIR;
  dir->version     = m_tagHeader.version;
  dir->nextAddress = 0L;
  dir->nbEntries   = 1;
  dir->entry[0].firstTime = now;
  dir->entry[0].fileOffset = dir->size;
  for ( unsigned int kk = 1 ; MAX_ENTRY > kk ; ++kk ) {
    dir->entry[kk].firstTime = 0;
    dir->entry[kk].fileOffset = 0;
  }
  fwrite( dir, sizeof(DirectoryRecord), 1, m_file );
  debug() << "write: Create directory block at address " << m_dirAddressInFile << endmsg;
}

//=========================================================================
//  Find the proper directory and data block containing a given time.
//=========================================================================
bool TrendingTool::getDataContaining ( int time ) {
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
int TrendingTool::unpackAnEvent ( ) {
  int time = m_data.data[m_ptData++].i;
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
//=============================================================================
