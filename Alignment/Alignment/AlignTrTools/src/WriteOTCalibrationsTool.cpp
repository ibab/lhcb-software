// $Id: WriteAlignmentConditions.cpp,v 1.9 2010-01-12 12:09:19 wouter Exp $

// std
#include <sstream>
#include <fstream>
#include <boost/filesystem.hpp>

// Interface
#include "AlignmentInterfaces/IWriteAlignmentConditionsTool.h"

// From Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// From Det
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/Condition.h"
#include "OTDet/DeOTDetector.h"
#include "Kernel/STXMLUtils.h"


class WriteOTCalibrationsTool
  : public GaudiTool, virtual public IWriteAlignmentConditionsTool
{
public:
  /// constructer
  WriteOTCalibrationsTool( const std::string& type,
			   const std::string& name,
			   const IInterface* parent); ///< Standard constructor

  /// destructer
  virtual ~WriteOTCalibrationsTool();

  /// initialize
  StatusCode initialize();

  // Everything configured via options
  StatusCode write() const ;

  // Everything configured via options
  StatusCode write(const std::string& version) const ;


private:
  StatusCode createXmlFile(const std::string& filename,
			   const std::string& contents,
			   const std::string& version) const ;
  StatusCode writeGlobalCalibration(const std::string& version) const ;
  StatusCode writeModuleConditions(const std::string& conditionname,
				   const std::string& path,
				   const std::string& prefix,
				   const std::string& version) const ;
private:
  // std::string m_topElement;
  // std::string m_footer;
  // std::string m_startTag;

  // std::string footer() const;
  // std::string header(const std::string& conString) const;
  // std::string strip(const std::string& conString) const;
  //void replaceChars(std::string& conString) const;
  //void replace(const std::string& conString,std::string in, std::string out ) const;
  //void createDirectory( const std::string& dir ) const ;

  bool m_writeModuleCalibration ;
  bool m_writeModuleStatus ;
  bool m_writeGlobalCalibration ;
  std::string m_directory ;
  unsigned int m_precision;
  std::string m_author;
  std::string m_description;
  std::string m_version;
  const DeOTDetector* m_otdetector ;
  bool m_online;

} ;


/*********************************************************************************/
/* Implementation                                                                */
/*********************************************************************************/


DECLARE_TOOL_FACTORY( WriteOTCalibrationsTool )



WriteOTCalibrationsTool::WriteOTCalibrationsTool( const std::string& type,
						  const std::string& name,
						  const IInterface* parent)
: GaudiTool( type,name,parent)
{
  // interface
  declareInterface<IWriteAlignmentConditionsTool>(this);
  // properties
  declareProperty("WriteModuleCalibration" , m_writeModuleCalibration  = false  );
  declareProperty("WriteModuleStatus"     , m_writeModuleStatus      = false );
  declareProperty("WriteGlobalCalibration", m_writeGlobalCalibration = false ) ;
  declareProperty("Directory", m_directory = "xml" );
  declareProperty("Precision", m_precision = 5u);
  declareProperty("Author", m_author = "");
  declareProperty("Version", m_version = "");
  declareProperty("Description", m_description = "");
  declareProperty("OnlineMode", m_online = false);
}


WriteOTCalibrationsTool::~WriteOTCalibrationsTool() {}


StatusCode WriteOTCalibrationsTool::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  /// Get pointer to detector
  m_otdetector = getDet<DeOTDetector>( DeOTDetectorLocation::Default );
  return StatusCode::SUCCESS;
}


StatusCode WriteOTCalibrationsTool::write() const
{
  return write(m_version) ;
}


StatusCode WriteOTCalibrationsTool::write(const std::string& version) const
{
  StatusCode sc = StatusCode::SUCCESS ;
  if( m_writeGlobalCalibration ) sc = writeGlobalCalibration(version) ;
  if( m_writeModuleCalibration ) sc = writeModuleConditions("Calibration","Conditions/OT/Calibration","CalibrationModules",version) ;
  if( m_writeModuleStatus      ) sc = writeModuleConditions("Status","Conditions/OT/ChannelInfo","ReadoutModules",version) ;

  if (sc.isFailure() ) return Warning( "Failed to write conditions to xml", StatusCode::FAILURE );
  return sc ;
}

namespace {
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

  std::string formattedcondition( const Condition& condition, unsigned int precision )
  {
    std::ostringstream strstream ;
    strstream << condition.toXml( "", false , precision ) << std::endl ;
    std::string rc = strstream.str() ;
    // let's make this a bit more readable
    // - replace every instance of '>' with ">\n"
    // - replace every instance of '<' with "\n<"
    // - replace every instance of '\n\n' with "\n"
    stringreplace(rc,">",">\n") ;
    stringreplace(rc,"<","\n<",1) ;
    stringreplace(rc,"\n\n","\n") ;
    return rc ;
  }

  std::string qNickName( const DeOTQuarter& quarter )
  {
    const LHCb::OTChannelID id = quarter.elementID() ;
    const char layernames[4][256] = { "X1","U","V","X2" } ;
    char name[256] ;
    sprintf(name,"T%d%sQ%d", id.station(),layernames[id.layer()],id.quarter()) ;
    return std::string(name) ;
  }
}

StatusCode WriteOTCalibrationsTool::createXmlFile(const std::string& filename,
						  const std::string& version,
						  const std::string& contents) const
{
  /// create directory if it does not exist
  boost::filesystem::path apath(filename) ;
  boost::filesystem::create_directories(apath.parent_path());
  /// create the output file
  std::ofstream output( filename );
  if ( output.fail() ) {
    return Warning( "Failed to open output file " + filename, StatusCode::FAILURE );
  }
  // write the header
  if (!m_online) {
     output << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<!DOCTYPE DDDB SYSTEM "
            << "\"conddb:/DTD/structure.dtd\">\n<DDDB>\n";
  }
  // only write the comments if it is actually meanigfull
  if( !m_author.empty() || !version.empty() || !m_description.empty() ) {
    std::ostringstream comment ;
    ST::XMLUtils::fullComment(comment,m_author,version,m_description);
    output << comment.str() ;
  }
  /// write the contents
  output << contents ;
  /// write the footer
  if (!m_online) {
     output << "</DDDB>\n" ;
  }
  /// close the file
  output.close() ;
  return StatusCode::SUCCESS ;
}

StatusCode WriteOTCalibrationsTool::writeGlobalCalibration(const std::string& version) const
{
  StatusCode sc = StatusCode::SUCCESS ;
  if(  m_otdetector->hasCondition("Calibration") ) {
    /// construct the filename
    std::string filename = m_directory + "/OT/Calibration/CalibrationGlobal.xml" ;
    /// create the file
    sc = createXmlFile(filename,version,formattedcondition( *(m_otdetector->condition("Calibration")), m_precision ) ) ;
  } else {
    warning() << "Cannot find Calibration condition in OTDetector" << endmsg ;
    sc = StatusCode::FAILURE ;
  }
  return sc ;
}

StatusCode WriteOTCalibrationsTool::writeModuleConditions(const std::string& conditionname,
							 const std::string& path,
							 const std::string& prefix,
							 const std::string& version) const
{
  StatusCode sc = StatusCode::SUCCESS ;
  /// create the path
  std::string fullpath = m_directory + "/" + path ;
  /// Write condition file for every quarter
  typedef std::map< LHCb::OTChannelID, std::vector< DeOTModule* > >::const_iterator ITER;
  for( const auto& quarter: m_otdetector->quarters() ) {
    /// get the OT nickname for a quarter: cannot we get this from the geometry?
    const std::string qname = qNickName( *quarter ) ;
    /// create the filename
    const std::string filename    = fullpath + "/" + qname + "@" + prefix + ".xml";
    /// create the catalog name
    const std::string catalog = prefix + qname;
    /// fill the contents
    std::stringstream contents ;
    // Open the catalog
    contents << "<catalog name=\"" + catalog + "\">\n";
    // loop over modules
    for( const auto& module: quarter->modules() ) {
      /// First get the condition
      const Condition* condition = module->condition(conditionname);
      contents << formattedcondition(*condition,m_precision) ;
    }
    contents << "</catalog>\n" ;
    sc = createXmlFile( filename, version, contents.str() ) ;
  }
  return sc;
}
