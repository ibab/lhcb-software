// $Id: OTWriteConditionsToXml.cpp,v 1.1.1.1 2009-04-21 10:57:19 jblouw Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// DetDesc
#include "DetDesc/Condition.h"

// from OTDet
#include "OTDet/DeOTDetector.h"

// from BOOST
#include <boost/filesystem.hpp>
#include <sstream>

// local

// Include files
// from STD
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Kernel
#include "Kernel/OTChannelID.h"

// from OTDet
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

/** @class OTWriteMonoAlignmentToXml OTWriteMonoAlignmentToXml.h
 *  Alg to write mono alignment conditions to xml.
 *
 *  @author Jan Amoraal, Wouter Hulsbergen
 *  @date   2014-12-19
 */

class OTWriteMonoAlignmentToXml : public GaudiAlgorithm
{
public: 
  /// Standard constructor
  OTWriteMonoAlignmentToXml( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~OTWriteMonoAlignmentToXml( ); ///< Destructor
  virtual StatusCode execute  () { return StatusCode::SUCCESS ; }    ///< Algorithm execute
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  std::string  m_catalog;
  std::string  m_path;
  std::string  m_filename ;
  unsigned int m_precision;
};

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTWriteMonoAlignmentToXml )

OTWriteMonoAlignmentToXml::OTWriteMonoAlignmentToXml( const std::string& name,
						      ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "CatalogPrefix" , m_catalog = "MonoAlignment" );
  declareProperty( "Precision"     , m_precision = 3u ); // 1 micron ?!
  declareProperty( "Path"          , m_path = "xml/Conditions/OT/Alignment" ) ;
  declareProperty( "Filename"      , m_filename = "MonoAlignment.xml" ) ;
}

OTWriteMonoAlignmentToXml::~OTWriteMonoAlignmentToXml() {} 

namespace 
{
  void stringreplace(std::string& astring,
		     const std::string& in,
		     const std::string& out,
		     size_t pos=0)
  {
    while ( pos < astring.size() &&
	    (pos = astring.find(in,pos))!=std::string::npos) {
      astring.replace( pos, in.size(), out) ;
      pos += out.size() ;
    }
  }
}

StatusCode OTWriteMonoAlignmentToXml::finalize()
{

  const std::string header = 
    "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n<DDDB>\n";

  /// create directory if it does not exist
  boost::filesystem::create_directories(m_path);

  // open the file
  const std::string file = m_path + "/" + m_filename ;
  std::ofstream output( file.c_str() );
  if ( output.fail() ) {
    return Warning( "Failed to open output file " + file, StatusCode::FAILURE );
  }
  
  // First the header
  output << header;
  // Now the catalog
  output << "<catalog name = \"" + m_catalog + "\">\n";

  //
  /// Now the conditions 
  const DeOTDetector* detector = getDet<DeOTDetector>( DeOTDetectorLocation::Default );
  for( auto module : detector->modules() ) {
    /// First get the condition
    const Condition* condition = module->monoalignCondition();
    if(condition) {
      std::ostringstream strstream ;
      //strstream.setf(std::ios::fixed,std::ios::floatfield);
      //strstream.precision(1) ;
      strstream << condition->toXml( "", false , m_precision ) + "\n";
      std::string conditionstring = strstream.str() ;
      // let's make this a bit more readable
      // - replace every instance of '>' with ">\n"
      // - replace every instance of '<' with "\n<"
      // - replace every instance of '\n\n' with "\n"
      
      stringreplace(conditionstring,">",">\n") ;
      stringreplace(conditionstring,"<","\n<",1) ;
      stringreplace(conditionstring,"\n\n","\n") ;
      
      output << conditionstring ;
    }
  }
  output << "</catalog>\n</DDDB>\n" << std::endl;
  output.close() ;
  
  return StatusCode::SUCCESS;
}
