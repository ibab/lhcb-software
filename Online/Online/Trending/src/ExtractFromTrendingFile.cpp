// Include files
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/AlgTool.h"

// local
#include "ExtractFromTrendingFile.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ExtractFromTrendingFile
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
  /*
  std::cout << "Arguments n=" << argc;
  for ( int kk = 0; argc > kk; ++kk ) {
    std::cout << " " << argv[kk];
  }
  std::cout << std::endl;
  */
  if ( argc < 5 ) {
    std::cout << "Usage : " << argv[0] << " fileName   tag   startDate  endDate" << std::endl;
    std::cout << "        fileName  is the file to extract, e.g. LHCb_HLT" << std::endl;
    std::cout << "        tag       is the name of the variable to extract" << std::endl;
    std::cout << "        startDate is the first time in ISO format" << std::endl;
    std::cout << "        endDate   is the last time in ISO format" << std::endl;
    exit( 0 );
  }

  ExtractFromTrendingFile extract( trendTool );
  extract.extract( std::string(argv[1]), std::string(argv[2]), std::string(argv[3]), std::string(argv[4]) );
  delete trendTool;
};

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ExtractFromTrendingFile::ExtractFromTrendingFile( ITrendingTool* trend ) :
  m_trend( trend ) {
}
//=============================================================================
// Destructor
//=============================================================================
ExtractFromTrendingFile::~ExtractFromTrendingFile() {}


//=========================================================================
//  Extract in the output the information from the given time in the given range
//=========================================================================
void ExtractFromTrendingFile::extract( std::string file, std::string tag, std::string start, std::string end ) {
  int startTime = timeFromISO( start );
  if ( 0 == startTime ) {
    std::cout << "Invalid start time '" << start << "'" << std::endl;
    return;
  }
  int endTime   = timeFromISO( end );
  if ( 0 == endTime ) {
    std::cout << "Invalid end time '" << end << "'" << std::endl;
    return;
  }

  bool status = m_trend->openRead( file );
  if ( !status ) {
    std::cout << "*** Failed to open Trending file '" << file << "' ***" << std::endl;
    return;
  }
  std::vector<std::string> tags;
  status = m_trend->tags( tags );
  if ( std::find( tags.begin(), tags.end(), tag ) == tags.end() ) {
    m_trend->closeFile();
    std::cout << "*** Tag '" << tag << "'not found in list:" << std::endl;
    for ( unsigned int kk = 0 ; tags.size() != kk; ++kk ) {
      std::cout << "Tag " << kk << " = '" << tags[kk] << "'" << std::endl;
    }
    return;
  }

  status = m_trend->select( startTime, endTime, tag );
  if ( !status ) {
    m_trend->closeFile();
    return;
  }

  unsigned int time;
  float value;
  while( m_trend->nextValue( time, value ) ) {
    std::string iso =  m_trend->timeString( time );
    iso[10] = 'T';
    std::cout << iso << " " << value << std::endl;
  }
  m_trend->closeFile();
}
//=============================================================================
