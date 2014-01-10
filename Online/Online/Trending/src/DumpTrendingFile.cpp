// Include files
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/AlgTool.h"

// local
#include "Trending/TrendParams.h"
#include "DumpTrendingFile.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DumpTrendingFile
//
// 2011-05-31 : Olivier Callot
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
  // load trending tool
  ISvcLocator * iface = Gaudi::svcLocator() ;
  IService * isvc ;
  iface -> getService( "ToolSvc" , isvc ) ;
  const IInterface * a3( isvc ) ;
  const std::string & name( "TrendingTool" ) ;
  IAlgTool * intf = AlgTool::Factory::create(name, name, name, a3);
  ITrendingTool* trendTool = dynamic_cast< ITrendingTool * >( intf ) ;

  std::cout << "Arguments n=" << argc;
  for ( int kk = 0; argc > kk; ++kk ) {
    std::cout << " " << argv[kk];
  }
  std::cout << std::endl;

  if ( argc < 2 ) {
    std::cout << "Usage : " << argv[0] << " fileName" << std::endl;
    std::cout << "        fileName  is the file to extract, e.g. LHCb_HLT" << std::endl;
    exit( 0 );
  }
  bool verbose = (argc == 3 );
  DumpTrendingFile dump( trendTool );
  dump.dump( std::string( argv[1]), verbose );
  delete trendTool;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DumpTrendingFile::DumpTrendingFile(  ITrendingTool* trend ) :
  m_trend( trend )
{
}
//=============================================================================
// Destructor
//=============================================================================
DumpTrendingFile::~DumpTrendingFile() {}


//=========================================================================
//  Dump a file
//=========================================================================
void DumpTrendingFile::dump (std::string file, bool verbose ) {
  std::string fileFullName = TREND_AREA + file + ".trend";
  std::string mode = "r";
  m_file = fopen( fileFullName.c_str(), mode.c_str() );
  if ( 0 == m_file ) {
    std::cout << "Can not open file " << fileFullName << std::endl;
    return;
  }
  char line[2048];

  int maxPrint = 100;
  if ( verbose ) maxPrint = 100000000;

  long nextTags = 1;
  m_tagAddressInFile = 0;
  while ( 0 != nextTags ) {
    fseek( m_file, m_tagAddressInFile, SEEK_SET );
    int nRead = fread( &m_tagHeader, 1, sizeof( Header ), m_file );
    if ( sizeof( Header ) != nRead ) {
      std::cout << "Error reading the header of the tag block: read " << nRead << " bytes" << std::endl;
      return;
    }
    std::cout << "Tag header: size " << m_tagHeader.size << " type " << m_tagHeader.type
              << " version " << m_tagHeader.version << " next " <<  m_tagHeader.nextAddress << std::endl;
    nextTags =  m_tagHeader.nextAddress;
    m_tagAddressInFile = nextTags;
    int nToRead = m_tagHeader.size - sizeof( Header );
    char* temp = (char*)malloc( nToRead );
    int nn = fread( temp, 1, nToRead, m_file );
    if ( nn != nToRead ) {
      std::cout << "Reading tags, need " << nToRead << " bytes, received " << nn << std::endl;
      free( temp );
      return;
    }

    m_firstDirAddress = ftell( m_file );  //== First directory block is after tag block.
    m_tags.clear();
    char* p = temp;
    while ( 0 < nn ) {
      std::string tag( p );
      int ll = strlen( p ) + 1;
      if ( 0 == ll ) break;
      p  += ll;
      nn -= ll;
      m_tags.push_back( tag );
      m_lastData.push_back( 0. );
      //std::cout << "   loaded tag " << m_tags.size() << " = '" << tag
      //                         << "'   rest " << nn << " bytes." << std::endl;
    }
    m_nbTag = m_tags.size();
    m_nbMask = (m_nbTag/32) + 1;
    free( temp );  // release memory...
    m_dir.nextAddress = m_firstDirAddress;

    std::cout << "Loaded " << m_nbTag << " tags. nbMask " << m_nbMask << std::endl;

    while ( 0 != m_dir.nextAddress ) {

      m_dirAddressInFile = m_dir.nextAddress;
      fseek( m_file, m_dirAddressInFile, SEEK_SET );
      unsigned int nn = fread( &m_dir, 1, sizeof(DirectoryRecord), m_file );
      if ( nn < sizeof(DirectoryRecord) ) {
        std::cout << "*** Requested " << sizeof(DirectoryRecord) << " bytes, read " << nn << " in directory" << std::endl;
        break;
      }

      std::cout << "*** Directory: size " << m_dir.size << " type " << m_dir.type << " version " << m_dir.version
                << " next " << m_dir.nextAddress << " nbEntries " << m_dir.nbEntries << std::endl;
      if ( m_dir.nbEntries > MAX_ENTRY ) return;
      /*
      if ( 0 == m_dir.nextAddress ) {
        verbose = true;
        maxPrint = 10000000;
      }
      */

      if ( verbose ) {
        for ( int kk = 0 ; kk <= m_dir.nbEntries ; ++kk ) {
          sprintf( line, "   entry%5d time%12d = %s address %15d", kk,  m_dir.entry[kk].firstTime,
                   m_trend->timeString( m_dir.entry[kk].firstTime ).c_str(),  m_dir.entry[kk].fileOffset );
          std::cout << line << std::endl;
        }
      }

      std::cout << "*** Data analysis *** " << std::endl;
      unsigned int firstTime = 0;
      for ( int kk = 0 ; kk < m_dir.nbEntries ; ++kk ) {
        m_dataAddressInFile = m_dirAddressInFile + m_dir.entry[kk].fileOffset;
        fseek( m_file, m_dataAddressInFile, SEEK_SET );
        fread( &m_data, 1, sizeof(DataRecord), m_file );
        bool headPrinted = false;
        m_ptData = 0;
        int maxPtData = ( int(m_data.size)-8 ) /4;
        if ( m_data.data[m_ptData].i < firstTime ||
             m_dir.entry[kk].firstTime < firstTime ) {
          if ( !headPrinted ) {
            sprintf( line, "   entry%5d time%12d = %s address %15d size%6d bytes type%2d version%2d",
                     kk,  m_dir.entry[kk].firstTime,
                     m_trend->timeString( m_dir.entry[kk].firstTime ).c_str(),
                     m_dir.entry[kk].fileOffset,
                     m_data.size, m_data.type, m_data.version );
            std::cout << line << std::endl;
            headPrinted = true;
          }
          std::cout << " -- Time ordering   " << firstTime << " = " <<  m_trend->timeString( firstTime )
                    << " ** After **" << std::endl;
        }
        firstTime = m_data.data[m_ptData].i;
        if ( firstTime != m_dir.entry[kk].firstTime ) {
          if ( !headPrinted ) {
            sprintf( line, "   entry%5d time%12d = %s address %15d size%6d bytes type%2d version%2d",
                     kk,  m_dir.entry[kk].firstTime,
                     m_trend->timeString( m_dir.entry[kk].firstTime ).c_str(),
                     m_dir.entry[kk].fileOffset,
                     m_data.size, m_data.type, m_data.version );
            std::cout << line << std::endl;
            headPrinted = true;
          }
          std::cout << " -- First time data " << firstTime << " = " << m_trend->timeString( firstTime )
                    << " ** Mismatch ** " << std::endl;
        }
        int lastGood = 0;
        int lastLastGood = 0;
        while ( m_ptData < maxPtData ) {
          int prevPt = m_ptData;
          unsigned int time = m_data.data[m_ptData++].i;
          if ( 0 == time ) {
            if ( m_ptData != maxPtData ) {
              if ( !headPrinted ) {
                sprintf( line, "   entry%5d time%12d = %s address %15d size%6d bytes type%2d version%2d",
                         kk,  m_dir.entry[kk].firstTime,
                         m_trend->timeString( m_dir.entry[kk].firstTime ).c_str(),
                         m_dir.entry[kk].fileOffset,
                         m_data.size, m_data.type, m_data.version );
                std::cout << line << std::endl;
                headPrinted = true;
              }
              std::cout << " ** Mismatch record length: " << maxPtData - m_ptData << " words at " << m_ptData
                        << " last good " << lastGood << std::endl;
              if ( maxPtData > m_ptData && maxPtData-m_ptData < 10 ) {
                for ( int kl = lastGood; maxPtData >= kl ; ++kl ) {
                  std::cout << "   at " << kl << " data " << m_data.data[kl].i << std::endl;
                }
              }
            }
            break;
          }
          lastGood = lastLastGood;
          lastLastGood = prevPt;
          int ptMask = m_ptData;
          m_ptData  += m_nbMask;
          int mask = 0;
          int nbData = 0;
          for ( int ll = 0 ; m_nbTag > ll ; ++ll ) {
            if ( 0 == ll%32 ) mask = m_data.data[ptMask++].i;
            if ( mask & 1 ) {
              m_lastData[ll] = m_data.data[m_ptData++].f;
              ++nbData;
            }
            mask = mask>>1;
          }
          if ( time < firstTime ) {
            if ( !headPrinted ) {
              sprintf( line, "   entry%5d time%12d = %s address %15d size%6d bytes type%2d version%2d",
                       kk,  m_dir.entry[kk].firstTime,
                       m_trend->timeString( m_dir.entry[kk].firstTime ).c_str(),
                       m_dir.entry[kk].fileOffset,
                       m_data.size, m_data.type, m_data.version );
              std::cout << line << std::endl;
              headPrinted = true;
            }
            std::cout << " ** Mismatch time ordering: Previous " << firstTime << " current " << time << " = "
                      << m_trend->timeString( time ) << " read " << nbData << " values, next pointer " << m_ptData
                      << std::endl;
            maxPrint--;
          } else if ( time - firstTime > 3600 ) {
            if ( !headPrinted ) {
              sprintf( line, "   entry%5d time%12d = %s address %15d size%6d bytes type%2d version%2d",
                       kk,  m_dir.entry[kk].firstTime,
                       m_trend->timeString( m_dir.entry[kk].firstTime ).c_str(),
                       m_dir.entry[kk].fileOffset,
                       m_data.size, m_data.type, m_data.version );
              std::cout << line << std::endl;
              headPrinted = true;
            }
            std::cout << " ** Gap>1 hrs from " << firstTime << " = " << m_trend->timeString( firstTime ) << std::endl
                      << "                to " << time << " = " << m_trend->timeString( time ) << std::endl;
            maxPrint--;
          } else if ( verbose ) {
            if ( !headPrinted ) {
              sprintf( line, "   entry%5d time%12d = %s address %15d size%6d bytes type%2d version%2d",
                       kk,  m_dir.entry[kk].firstTime,
                       m_trend->timeString( m_dir.entry[kk].firstTime ).c_str(),
                       m_dir.entry[kk].fileOffset,
                       m_data.size, m_data.type, m_data.version );
              std::cout << line << std::endl;
              headPrinted = true;
            }
            std::cout << "     data at " << time << " = " << m_trend->timeString( time )
                      << " nb " << nbData << " prevPt " << prevPt << std::endl;
            maxPrint--;
          }
          firstTime = time;
          if ( maxPrint < 0 ) {
            std::cout << "**** Too many errors... Abort ****" << std::endl;
            return;
          }
        }
      }
    }
  }
}
//=============================================================================
