// $Id: TrendingTool.h,v 1.1.1.1 2010-06-11 09:58:56 ocallot Exp $
#ifndef TRENDWRITER_H 
#define TRENDWRITER_H 1

// Include files
#include <ctime>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>

#include "Trending/TrendParams.h"

/** @class TrendWriter TrendWriter.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2011-03-18
 */
class TrendWriter {
public:

  /// Standard constructor
  TrendWriter( );

  virtual ~TrendWriter( ); ///< Destructor

  bool openWrite( std::string name, std::vector<std::string> tags );

  bool setThresholds( std::vector<float> thresholds );
  
  bool setThreshold( std::string tag, float thr );
  
  void setAverageTime( unsigned int seconds )    { m_averageTime = seconds; }

  void setMaxTimeNoWrite( unsigned int seconds ) { m_maxTimeNoWrite = seconds;};

  void addValue( std::string tag, float value );

  bool write( std::vector<float> values, unsigned int time = 0 );
  
  void closeFile();

private:

  bool loadLastTags( );
  
  bool nextDataBlock( );

  void createDirectoryRecord ( DirectoryRecord* dir );
  
  bool readDirectoryRecord ( long address );

  unsigned int unpackAnEvent ( );
  
  void writeEntry ( unsigned int now, std::vector<float>& data, bool writeEachEntry );

  void addDataEntry( unsigned int now, std::vector<float>& data, bool writeEachEntry, bool full );

  bool getDataContaining ( unsigned int time );

  bool nextEvent( unsigned int& time, std::vector<float>& data ) ;

private:
  bool                     m_debug;
  bool                     m_forWriting;
  Header                   m_tagHeader;
  std::vector<std::string> m_tags;
  std::vector<float>       m_thresholds;
  std::vector<float>       m_lastData;
  int                      m_nbTag;
  int                      m_nbMask;
  DirectoryRecord          m_dir;
  DataRecord               m_data;
  long                     m_firstDirAddress;
  long                     m_tagAddressInFile;
  long                     m_dirAddressInFile;
  long                     m_dataAddressInFile;
  FILE*                    m_file;
  int                      m_ptDir;
  int                      m_ptData;
  bool                     m_hasSelection;
  //int                      m_requestedTagNumber;
  //int                      m_tagNumber;
  unsigned int             m_lastTime;
  unsigned int             m_averageTime;
  std::vector<float>       m_sumForAverage;
  std::vector<int>         m_nbForAverage;
  unsigned int             m_maxTimeNoWrite;
};
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrendWriter::TrendWriter( ) :
  m_debug( false )
  , m_forWriting( false )
  , m_firstDirAddress( -1 )
  , m_tagAddressInFile( 0 )
  , m_dirAddressInFile( -1 )
  , m_dataAddressInFile( -1 )
  , m_file( NULL )
  , m_hasSelection( false )
			//, m_requestedTagNumber( -1 )
			//, m_tagNumber( -1 )
  , m_lastTime( 0xffffffff )
  , m_averageTime( 0 )
  , m_maxTimeNoWrite( 0)
{
  m_data.size       = 0;
  m_data.data[0].i  = 0;
  m_dir.size        = 0;
  m_dir.nextAddress = 0;
  m_dir.nbEntries   = 0;
  m_dir.entry[0].firstTime = 0;
}
//=============================================================================
// Destructor
//=============================================================================
TrendWriter::~TrendWriter() {
  if ( NULL != m_file ) closeFile();
}


//=========================================================================
//  Open the Trending file for writing
//=========================================================================
bool TrendWriter::openWrite( std::string name, std::vector<std::string> tags ) {

  if ( NULL != m_file ) {
    std::cout << "TrendWriter::openWrite: ERROR file already opened!" << std::endl;
    return false;
  }

  //== Check that the tags are unique
  bool isBad = false;
  for ( std::vector<std::string>::const_iterator itS = tags.begin(); tags.end() > itS+1; ++itS ) {
    for ( std::vector<std::string>::const_iterator itS1 = itS+1; tags.end() != itS1 ; ++itS1 ) {
      if ( *itS == *itS1 ) {
        std::cout << "TrendWriter::openWrite: ERROR tag " << itS-tags.begin() << " and "
                  << itS1-tags.begin() << " are identical : " << *itS << std::endl;
        isBad = true;
      }
    }
  }
  if ( isBad ) return false;
  unsigned int maxSize = (unsigned int)(0.1 * DATA_SIZE);
  if ( maxSize < tags.size() ) {
    std::cout << "TrendWriter::openWrite: ERROR Requested to store " << tags.size() 
              << " tags, maximum allowed " << maxSize << std::endl;
    return false;
  }  

  if ( 0 == tags.size() ) {
    std::cout << "TrendWriter::openWrite: ERROR You need to specify a non-empty list of tags." << std::endl;
    return false;
  }  

  std::string fileFullName =  name + ".trend";
  if ( name[0] != '/' ) fileFullName = TREND_AREA + name + ".trend";

  //== Try top open an existing file, else create one.
  std::string mode = "r+";
  m_file = fopen( fileFullName.c_str(), mode.c_str() );
  if ( 0 == m_file ) {
    mode = "w+";
    m_file = fopen( fileFullName.c_str(), mode.c_str() );
    if ( 0 == m_file ) {
      std::cout << "TrendWriter::openWrite: ERROR Can not create file " << fileFullName << std::endl;
      return false;
    }
    m_tagHeader.version = 0;
  } else {
    if ( !loadLastTags( ) ) {
      closeFile();
      return false;   // Get the latest tags, return an error if...
    }
    readDirectoryRecord( m_firstDirAddress );
  }
  m_forWriting = true;

  //== First check for consistency: Same tags.

  bool isDifferent = false;
  unsigned int kk;
  if ( tags.size() != m_tags.size() ) {
    isDifferent = true;
  } else {
    for ( kk = 0; tags.size() > kk; ++kk ) {
      if ( tags[kk] != m_tags[kk] ) isDifferent = true;
    }
  }
  if ( !isDifferent ) return true;

  //== Get to end of file and create new tag record.

  fseek( m_file, 0, SEEK_END );
  long newAddress = ftell( m_file );

  if ( m_debug ) std::cout << "End of file at " << newAddress << " to create tag block there" << std::endl;

  m_tagHeader.type    = TYPE_TAG;
  m_tagHeader.version = m_tagHeader.version + 1;
  m_tagHeader.nextAddress = 0L;

  //== Compute the length of the tag strings, and copy them in a temp buffer
  int tagLength = 0;
  for ( kk = 0; tags.size() > kk; ++kk ) tagLength += ( tags[kk].size() + 1 );
  char* tagBuffer = (char*) malloc( tagLength );
  char* p = tagBuffer;
  m_tags.clear();
  m_thresholds.clear();
  m_lastData.clear();
  m_sumForAverage.clear();
  m_nbForAverage.clear();
  for ( kk = 0; tags.size() > kk; ++kk ) {
    m_tags.push_back( tags[kk] );
    m_thresholds.push_back( 0. );     // default threshold
    m_lastData.push_back( 0. );       // previous value of data
    m_sumForAverage.push_back( 0. );
    m_nbForAverage.push_back( 0 );
    strcpy( p, tags[kk].c_str() );
    p = p + strlen( p ) + 1;
  }
  m_nbTag = m_tags.size();
  m_nbMask = (m_nbTag/32) + 1;

  //== Write the new tag record
  m_tagHeader.size    = sizeof( Header ) + tagLength;
  fwrite( &m_tagHeader, sizeof( Header ), 1, m_file );
  fwrite( tagBuffer, 1, tagLength, m_file );
  free( tagBuffer );
  m_firstDirAddress = ftell( m_file );  //== First directory block is after tag block.
  m_dirAddressInFile = m_firstDirAddress;

  //== Update the previous tag record if any, now that the new one is on file.
  if ( newAddress != 0 ) {
    fseek( m_file, m_tagAddressInFile, SEEK_SET );
    Header tagHeader;
    int nRead = fread( &tagHeader, 1, sizeof( Header ), m_file );
    if ( sizeof( Header ) != nRead ) {
      std::cout << "TrendWriter::openWrite: ERROR reading the header of the tag block: read "
                << nRead << " bytes" << " at address " << m_tagAddressInFile << std::endl;
      return false;
    }
    tagHeader.nextAddress = newAddress;
    fseek( m_file, m_tagAddressInFile, SEEK_SET );
    fwrite( &tagHeader, 1, sizeof( Header ), m_file );
  }
  m_tagAddressInFile = newAddress;

  //== Create a directory record
  fseek( m_file, m_dirAddressInFile, SEEK_SET );
  createDirectoryRecord( &m_dir );
  m_ptDir = 0;
  fflush( m_file);  // force writing on disk.

  if ( m_debug ) std::cout << "openWrite: After new tag block: dir add " << m_dirAddressInFile 
                           << " data " << m_dataAddressInFile << std::endl;

  return true;
}
//=========================================================================
//  Set the thresholds for all variables
//=========================================================================
bool TrendWriter::setThresholds ( std::vector<float> thr ) {
  if ( thr.size() != m_thresholds.size() ) {
    std::cout << "TrendWriter::setThresholds: Need " << m_thresholds.size()
              << " values while " << thr.size() << " given. ** IGNORED **"
              << std::endl;
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
bool TrendWriter::setThreshold ( std::string tag, float thr) {
  for ( unsigned int kk = 0 ; m_thresholds.size() > kk ; ++kk ) {
    if ( m_tags[kk] == tag ) {
      m_thresholds[kk] = thr;
      return true;
    }
  }
  std::cout << "TrendWriter::setThreshold: Can't change threshold for '" << tag << "' : not found" << std::endl;
  return false;
}
//=========================================================================
//  Add a value to the average. If time is later than needed, write.
//=========================================================================
void TrendWriter::addValue ( std::string tag, float value ) {
  if ( 0 == m_averageTime ) return;
  unsigned int now = (int)::time( 0 );
  if ( now < m_lastTime ) m_lastTime = now;  //== First entry, m_lastTime is VERY far in the future
  if ( now > m_lastTime + m_averageTime ) {
    std::vector<float> dum( m_tags.size(), 0. );
    write( dum, now );                                //== write the current average...
    for ( unsigned int k1 = 0; m_tags.size() > k1 ; ++k1 ) {
      m_sumForAverage[k1] = 0.;
      m_nbForAverage[k1]  = 0;
    }
  }
  for ( unsigned int k2 = 0; m_tags.size() > k2; ++k2 ) {
    if ( m_tags[k2] == tag ) {
      m_sumForAverage[k2] += value;
      m_nbForAverage[k2]  += 1;
    }
  }
}

//=========================================================================
//  Store an event in the file
//=========================================================================
bool TrendWriter::write( std::vector<float> data, unsigned int now ) {

  if ( NULL == m_file || !m_forWriting ) {
    std::cout << "TrendWriter::write: File not opened for writing!" << std::endl;
    return false;
  }

  if ( 0 != m_tagHeader.nextAddress ) {
    std::cout << "TrendWriter::write: Writing is only possible for tags of the highest version" << std::endl;
    return false;
  }
    
  if ( 0 == now ) now = (int)::time( 0 );

  //== Get the last entry before that time. First open the first dir if needed.
  if ( 0 > m_dirAddressInFile ) {
    if ( !readDirectoryRecord( m_firstDirAddress ) ) {
      fseek( m_file, m_firstDirAddress, SEEK_SET );
      createDirectoryRecord( &m_dir );
    }
  }

  //== IF average is requested, sum and create an entry if needed...

  if ( m_averageTime > 0 ) { //== Compute averages and store
    if ( now < m_lastTime ) m_lastTime = now;  //== First entry, m_lastTime is VERY far in the future
    if ( now < m_lastTime + m_averageTime ) {  //== Still in accumulation time
      for ( unsigned int k1 = 0 ; m_tags.size() > k1 ; ++k1 ) {
        m_sumForAverage[k1] += data[k1];
        m_nbForAverage[k1]++;
      }
      return true;
    }

    //== Compute averages. If any computed, store
    int nbNew = 0;
    for ( unsigned int k2 = 0; m_tags.size() > k2 ; ++k2 ) {
      if ( m_nbForAverage[k2] > 0 ) {
        m_sumForAverage[k2] = m_sumForAverage[k2]/m_nbForAverage[k2];
        nbNew++;
      } else {
        m_sumForAverage[k2] = m_lastData[k2];
      }
    }
    if ( 0 < nbNew ) {
      unsigned int tt = m_lastTime + m_averageTime;
      getDataContaining( tt );
      writeEntry( tt, m_sumForAverage, true );
      m_lastTime = now;
    }
    for ( unsigned int k3 = 0 ; m_tags.size() > k3 ; ++k3 ) {
      m_sumForAverage[k3] = data[k3];
      m_nbForAverage[k3]  = 1;
    }
    return true;
  }
      
  //== Check if the time is accessible from the current directory, i.e. after first and before last

  bool hasDataAfter = false;
  if ( now < m_lastTime ) {   // First call, or new entry is not after the last written -> Find where to write
    if ( m_debug ) std::cout << "TrendWriter::write Position file for time = " << now << std::endl;
    hasDataAfter = getDataContaining( now );
  }
  
  //== We are positionned after the last entry before. If no data after, this is simple.

  if ( !hasDataAfter ) {
    writeEntry( now, data, true );
  } else {   //== Data after: Save context, read all data, and rewrite them after.
    std::cout << "TrendWriter::write - We have data after " << now << std::endl;
    long savedDirAdd  = m_dirAddressInFile;
    long savedDataAdd = m_dataAddressInFile;
    int  savedPtDir   = m_ptDir;
    int  savedPtData  = m_ptData;

    std::vector<int> savedTime;
    std::vector< std::vector<float> > savedData;
    m_hasSelection = true;
    unsigned int tTime;
    std::vector<float> tData( m_nbTag );
    while( nextEvent( tTime, tData ) ) {
      savedTime.push_back( tTime );
      savedData.push_back( tData );
    }

    m_dataAddressInFile = savedDataAdd;
    readDirectoryRecord( savedDirAdd );
    m_ptDir  = savedPtDir;
    m_ptData = savedPtData;
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
void TrendWriter::writeEntry( unsigned int now, std::vector<float>& data, bool writeEachEntry ) {
  if ( 0 != m_dir.entry[m_ptDir].firstTime ) {  // There is some data block...

    //== If we don't have enough space, create a new data record.
    if ( m_data.size > sizeof(DataRecord) - (m_nbTag+m_nbMask+2)* sizeof(int) ) {
      if ( !writeEachEntry ) {  // Used when including an entry in the middle, for performance reasons
        fseek( m_file, m_dataAddressInFile, SEEK_SET );
        fwrite( &m_data, 1, sizeof(DataRecord), m_file );
      }
      m_dataAddressInFile += sizeof(DataRecord);
      m_data.size = 12;  // 8 bytes header plus 4 bytes for the zero 
      for ( int k = 0; DATA_SIZE > k ; ++k ) m_data.data[k].i = 0;
      m_ptData    = 0;
      m_ptDir++;
      if ( m_ptDir == MAX_ENTRY-1 ) {
        long newDirAddress = m_dataAddressInFile;  // This is the next free in file, as obtained above.
        // Create a new fresh directrory block at the current location
        DirectoryRecord newDir;
        createDirectoryRecord( &newDir );

        // update old dir to have the chaining, include tag of first in next
        m_dir.nextAddress  = newDirAddress;
        m_dir.entry[m_ptDir].firstTime = now;
        fseek( m_file, m_dirAddressInFile , SEEK_SET );
        fwrite( &m_dir, sizeof(DirectoryRecord), 1, m_file );

        // re-read the new dir block to be positioned for the next data.
        readDirectoryRecord( newDirAddress );
        m_dataAddressInFile = ftell( m_file );
        m_dir.nbEntries = 0;
        m_ptDir = 0;
      }
      m_dir.nbEntries++;
      m_dir.entry[m_ptDir].firstTime = now;
      m_dir.entry[m_ptDir].fileOffset = m_dataAddressInFile - m_dirAddressInFile;
      if ( writeEachEntry ) {
        fseek( m_file, m_dirAddressInFile, SEEK_SET );
        fwrite( &m_dir, 1, sizeof(DirectoryRecord), m_file );
      }
    }
  } else {
    m_dir.entry[m_ptDir].firstTime = now;
    fseek( m_file, m_dirAddressInFile , SEEK_SET );
    fwrite( &m_dir, sizeof(DirectoryRecord), 1, m_file );
    m_ptData = 0;
  }

  bool full = ( 0 == m_ptData );
  if ( m_maxTimeNoWrite > 0 && 
       now > m_lastTime + m_maxTimeNoWrite ) full = true;
  addDataEntry( now, data, writeEachEntry, full );
}
//=========================================================================
//  Write a data item, with zero suppression if needed. Negative threshold = fraction. 
//  Space in the buffer is guaranteed
//=========================================================================
void TrendWriter::addDataEntry( unsigned int now, std::vector<float>& data, bool writeEachEntry, bool full ){
  int prevPtData = m_ptData;

  if ( m_debug ) std::cout << "TrendWriter::addDataEntry m_ptData " << m_ptData << std::endl;

  m_data.data[m_ptData++].i = now;
  unsigned int mask = 0;
  int maskAddr = m_ptData;
  int nbItem = 0;
  m_ptData += m_nbMask;  // first word to write
  for ( unsigned int kk = 0 ;  data.size() > kk ; ++kk ) {
    bool valid = full;
    if ( !valid ) {
      if ( 0 <= m_thresholds[kk] ) {
        valid = fabs( data[kk] - m_lastData[kk] ) > m_thresholds[kk];
      } else {
        valid = fabs( data[kk] - m_lastData[kk] ) > fabs( m_thresholds[kk] * m_lastData[kk] );
      }
    }
    if ( valid ) {
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
  if ( (data.size()%32) != 0 ) m_data.data[maskAddr].i = mask;  // Except if written at end of loop...
  m_data.data[m_ptData].i = 0;   // tag the end of the data block!

  if ( 0 < nbItem ) {
    m_lastTime          = now;
    m_data.size         = sizeof( int ) * (m_ptData+3);   // +3 as 8 bytes header + 1 extra int with a zero...
    m_data.type         = TYPE_DATA;
    m_data.version      = m_tagHeader.version;
    if ( writeEachEntry ) {
      if ( m_debug ) std::cout << "TrendWriter::addDataEntry m_dataAddressInFile = " << m_dataAddressInFile << std::endl;
      fseek( m_file, m_dataAddressInFile, SEEK_SET );
      fwrite( &m_data, 1, sizeof(DataRecord), m_file );
      fflush( m_file );
    }
  } else {
    m_ptData = prevPtData;        // restore the data pointer.
    m_data.data[m_ptData].i = 0;  // tag the end of the data block!
  }
}

//=========================================================================
//  Get the next event block.
//=========================================================================
bool TrendWriter::nextEvent( unsigned int& time, std::vector<float>& data ) {

  if ( !m_hasSelection ) {
    std::cout << "TrendWriter::nextEvent: no previous selection!" << std::endl;
    return false;
  }
  //== Do we need to read the next block
  if ( 0 ==  m_data.data[m_ptData].i ) {
    if ( !nextDataBlock() ) return false;
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
  return true;
}

//=========================================================================
// Close the file
//=========================================================================
void TrendWriter::closeFile() {
  if ( 0 != m_file ) {
    int status = fclose( m_file );
    if ( 0 != status ) std::cout << "TrendWriter::closeFile: fclose returned status " << status << std::endl;
    m_file = NULL;
  }
  m_tagAddressInFile  =  0;
  m_dirAddressInFile  = -1;
  m_dataAddressInFile = -1;
  m_lastTime = 0xffffffff;
}
//=========================================================================
//  Read the latest tags..
//=========================================================================
bool TrendWriter::loadLastTags( ) {

  if ( NULL == m_file ) {
    std::cout << "TrendWriter::loadTags: file not opened!" << std::endl;
    return false;
  }
  while ( true ) {
    fseek( m_file, m_tagAddressInFile, SEEK_SET );
    int nRead = fread( &m_tagHeader, 1, sizeof( Header ), m_file );
    if ( sizeof( Header ) != nRead ) {
      std::cout << "TrendWriter::openWrite: Error reading the header of the tag block: read " << nRead << " bytes" << std::endl;
      return false;
    }
    if ( 0 == m_tagHeader.nextAddress) break;
    m_tagAddressInFile = m_tagHeader.nextAddress;
  }
  int nToRead = m_tagHeader.size - sizeof( Header );
  char* temp = (char*)malloc( nToRead );
  if ( m_debug ) std::cout << "loadLastTags: read " << nToRead << " bytes at " << m_tagAddressInFile << std::endl;
  int nn = fread( temp, 1, nToRead, m_file );
  if ( nn != nToRead ) {
    std::cout << "TrendWriter::openWrite: reading tags, need " << nToRead << " bytes, received " << nn << std::endl;
    free( temp );
    return false;
  }
  m_firstDirAddress = ftell( m_file );  //== First directory block is after tag block.
  if ( m_debug ) std::cout << "loadLastTags: directory address " << m_firstDirAddress << std::endl;

  m_tags.clear();
  m_thresholds.clear();
  m_lastData.clear();
  m_sumForAverage.clear();
  m_nbForAverage.clear();
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
    m_sumForAverage.push_back( 0. );
    m_nbForAverage.push_back( 0 );
  }
  m_nbTag = m_tags.size();
  m_nbMask = (m_nbTag/32) + 1;
  free( temp );  // release memory...

  return true;
}

//=========================================================================
//  Get the next data block with adjusted pointers. false if no more data
//  Directory record should be OK... Internal function.
//=========================================================================
bool TrendWriter::nextDataBlock ( ) {
  
  if ( m_ptDir >= m_dir.nbEntries - 1 ) {
    if ( 0 == m_dir.nextAddress ) return false;  // no more data
    readDirectoryRecord( m_dir.nextAddress );
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
// Create an empty directory record. Tag the first entry...
//=========================================================================
void TrendWriter::createDirectoryRecord ( DirectoryRecord* dir ) {
  dir->size        = sizeof( DirectoryRecord );
  dir->type        = TYPE_DIR;
  dir->version     = m_tagHeader.version;
  dir->nextAddress = 0L;
  dir->nbEntries   = 1;
  for ( unsigned int kk = 0 ; MAX_ENTRY > kk ; ++kk ) {
    dir->entry[kk].firstTime = 0;
    dir->entry[kk].fileOffset = 0;
  }
  dir->entry[0].fileOffset = sizeof(DirectoryRecord);
  fwrite( dir, sizeof(DirectoryRecord), 1, m_file );
  m_dataAddressInFile = m_dirAddressInFile + m_dir.entry[m_ptDir].fileOffset;
}

//=========================================================================
//  Find the proper directory and data block containing a given time.
//=========================================================================
bool TrendWriter::getDataContaining ( unsigned int time ) {
  bool hasDataAfter = false;
  //== Is it before the first entry in the current directory?
  if ( time < m_dir.entry[0].firstTime ) {
    if ( m_dirAddressInFile != m_firstDirAddress ) {
      readDirectoryRecord( m_firstDirAddress );
    }
  }
  m_ptData    = 0;
  m_ptDir     = 0;
  
  while ( 0 !=   m_dir.entry[m_ptDir+1].firstTime &&
          time > m_dir.entry[m_ptDir+1].firstTime    ) {
    if ( m_ptDir >= m_dir.nbEntries-1 ) {
      if ( 0 == m_dir.nextAddress ) { // Next block indicated, but not available! This is a BUG!
        if ( 0 != m_ptDir ) m_ptDir--;
        break;
      }
      readDirectoryRecord( m_dir.nextAddress );
    } else {
      m_ptDir++;
    }
  }
  if ( 0 != m_dir.entry[m_ptDir+1].firstTime )  {
    hasDataAfter = true;
    if ( m_debug ) std::cout << "getDataContaining: ptDir " << m_ptDir << " enty times " << m_dir.entry[m_ptDir].firstTime 
                             << " " << m_dir.entry[m_ptDir+1].firstTime << std::endl;
  }
  
  if ( m_dataAddressInFile != m_dirAddressInFile + m_dir.entry[m_ptDir].fileOffset ) {
    m_dataAddressInFile = m_dirAddressInFile + m_dir.entry[m_ptDir].fileOffset;
    fseek( m_file, m_dataAddressInFile, SEEK_SET );
    m_data.size = 0;
    m_data.data[0].i = 0;  // in case of read error.
    fread( &m_data, 1, sizeof( DataRecord ), m_file );
  }
  //== Skip to the correct event entry
  while ( m_data.data[m_ptData].i <= time && 
          m_data.data[m_ptData].i >  0      ) unpackAnEvent( );
  if ( m_data.data[m_ptData].i != 0 ) hasDataAfter = true;
  if ( m_debug ) std::cout << "getDataCOntaining: m_ptData " << m_ptData << " time " << m_data.data[m_ptData].i << std::endl;
  return hasDataAfter;
}
//=========================================================================
//  Unpack an event in the data block. return the time of the event
//=========================================================================
unsigned int TrendWriter::unpackAnEvent ( ) {
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
//  Read a directory record
//=========================================================================
bool TrendWriter::readDirectoryRecord ( long address ) {
  m_dirAddressInFile = address;
  m_ptDir = 0;
  fseek( m_file, m_dirAddressInFile, SEEK_SET );
  int size = fread( &m_dir, 1, sizeof(DirectoryRecord), m_file );
  return size == sizeof(DirectoryRecord);
}
//=============================================================================

#endif // TRENDWRITER_H
