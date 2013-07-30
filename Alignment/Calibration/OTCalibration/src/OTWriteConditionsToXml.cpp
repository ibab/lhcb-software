// $Id: OTWriteConditionsToXml.cpp,v 1.1.1.1 2009-04-21 10:57:19 jblouw Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// DetDesc
#include "DetDesc/Condition.h"

// from OTDet
#include "OTDet/DeOTDetector.h"

// from BOOST
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/lambda/lambda.hpp>
#include <sstream>

// local
#include "OTWriteConditionsToXml.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTWriteConditionsToXml
//
// 2008-09-18 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTWriteConditionsToXml );

OTWriteConditionsToXml::OTWriteConditionsToXml( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  std::vector<std::string> names = boost::assign::list_of( "T1" )( "T2" )( "T3" )
                                                         ( "X1" )( "U" )( "V" )( "X2" )
                                                         ( "Q0" )( "Q1" )( "Q2" )( "Q3" )
                                                         ( "M1" )( "M2" )( "M3" )( "M4" )( "M5" )( "M6" )( "M7" )( "M8" )( "M9" );
    m_names = names;
#else
  m_names = boost::assign::list_of( "T1" )( "T2" )( "T3" )
                                  ( "X1" )( "U" )( "V" )( "X2" )
                                  ( "Q0" )( "Q1" )( "Q2" )( "Q3" )
                                  ( "M1" )( "M2" )( "M3" )( "M4" )( "M5" )( "M6" )( "M7" )( "M8" )( "M9" );
#endif

  declareProperty( "WriteCalibrationConditions", m_writeCalibration  = true                 );
  declareProperty( "CalibrationXmlPrefix"      , m_calibrationPrefix = "CalibrationModules" );
  declareProperty( "WriteStatusConditions"     , m_writeStatus       = false                );
  declareProperty( "StatusXmlPrefix"           , m_statusPrefix      = "ReadoutModules"     );
  declareProperty( "Precision"                 , m_precision         = 3u                   );
}

OTWriteConditionsToXml::~OTWriteConditionsToXml() {} 

StatusCode OTWriteConditionsToXml::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  /// Get pointer to detector
  DeOTDetector* detector = getDet<DeOTDetector>( DeOTDetectorLocation::Default );
  
  /// Get quarters
  std::vector<DeOTQuarter*> quarters = detector->quarters();
  /// No map quarters to their modules
  for ( std::vector< DeOTQuarter* >::const_iterator q = quarters.begin(), qEnd = quarters.end(); 
        q != qEnd; ++q ) m_mapQtoM[ (*q)->elementID() ] = (*q)->modules();

  if ( m_writeCalibration ) info() << "==> Going to write calibration conditions with prefix " << m_calibrationPrefix 
                                   << " and precision " << m_precision << "." <<  endmsg;

  if ( m_writeStatus      ) info() << "==> Going to write status conditions with prefix " << m_calibrationPrefix 
                                   << " and precision " << m_precision << "." <<  endmsg;

  return StatusCode::SUCCESS;
}

StatusCode OTWriteConditionsToXml::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  return StatusCode::SUCCESS;
  
}

StatusCode OTWriteConditionsToXml::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  
  StatusCode sc = StatusCode::SUCCESS;
  if ( m_writeCalibration ) sc = writeConditions( m_calibrationPrefix, m_precision );
  if ( m_writeStatus      ) sc = writeConditions( m_statusPrefix     , m_precision );
  
  if (sc.isFailure() ) return Warning( "Failed to write conditions to xml", StatusCode::FAILURE );
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

static void stringreplace(std::string& astring,
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


StatusCode OTWriteConditionsToXml::writeConditions( const std::string& prefix, const unsigned& precision ) const {

  const std::string header = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n<DDDB>\n";

  /// Write conditions
  typedef std::map< LHCb::OTChannelID, std::vector< DeOTModule* > >::const_iterator ITER;  
  for ( ITER i = m_mapQtoM.begin(), iEnd = m_mapQtoM.end(); i != iEnd; ++i ) {
    const std::string catalog = prefix + qNickname( i->first );
    const std::string file    = qNickname( i->first ) + "@" + prefix + ".xml";
    std::ofstream output( file.c_str() );
    if ( output.fail() ) {
      return Warning( "Failed to open output file " + file, StatusCode::FAILURE );
    }
    for ( std::vector< DeOTModule* >:: const_iterator j = ( i->second ).begin(), jEnd = ( i->second ).end(); 
          j != jEnd; ++j ) {
      if ( j == ( i->second ).begin() ) {
        // First the header
        output << header;
        // Now the catalog
        output << "<catalog name = \"" + catalog + "\">\n";
      }
      /// Now the conditions 
      /// First get the condition
      const Condition* condition = (*j)->calibrationCondition();
      std::ostringstream strstream ;
      //strstream.setf(std::ios::fixed,std::ios::floatfield);
      //strstream.precision(1) ;
      strstream << condition->toXml( "", false , precision ) + "\n";
      std::string conditionstring = strstream.str() ;
      // let's make this a bit more readable
      // - replace every instance of '>' with ">\n"
      // - replace every instance of '<' with "\n<"
      // - replace every instance of '\n\n' with "\n"
      
      stringreplace(conditionstring,">",">\n") ;
      stringreplace(conditionstring,"<","\n<",1) ;
      stringreplace(conditionstring,"\n\n","\n") ;
      
      //output << condition->toXml( "", false , precision ) + "\n";
      output << conditionstring ;
      /// Close catalog and DDDB
      if ( j + 1u == jEnd ) output << "</catalog>\n</DDDB>\n" << std::endl;
    }
  }
  
  return StatusCode::SUCCESS;
}

