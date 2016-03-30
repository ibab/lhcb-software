// From Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// Interface
#include "AlignmentInterfaces/IWriteAlignmentConditionsTool.h"

// From Det
#include "DetDesc/Condition.h"
#include "Kernel/STXMLUtils.h"

// From Rich
#include "RichDet/DeRichSystem.h"

// std
#include <sstream>
#include <fstream>
#include <boost/filesystem.hpp>


class WriteRichCalibrationsTool
  : public GaudiTool, virtual public IWriteAlignmentConditionsTool
{
public:
  /// constructer
  WriteRichCalibrationsTool( const std::string& type,
                             const std::string& name,
                             const IInterface* parent); ///< Standard constructor

  /// destructer
  virtual ~WriteRichCalibrationsTool();

  /// initialize
  StatusCode initialize();

  // Everything configured via options
  StatusCode write() const;

  // Everything configured via options
  StatusCode write(const std::string& version) const;

private:
  StatusCode createXmlFile(const std::string& filename,
                           const std::string& contents,
                           const std::string& version) const;

  StatusCode writeRefIndex(const std::string& version) const;
  StatusCode writeHPDAlign(const std::string& version) const;
  StatusCode writeHPDOcc(const std::string& version) const;
  StatusCode writeInactiveHPDs(const std::string& version) const;

private:

  /// Pointer to Rich Sys Detector Element
  const DeRichSystem * m_RichSys;

  int m_minHPDID;
  int m_maxHPDID;

  std::string m_directory;
  unsigned int m_precision;
  std::string m_author;
  std::string m_description;
  std::string m_version;
  bool m_online;

};


/******************************************************************************/
/* Implementation                                                             */
/******************************************************************************/

DECLARE_TOOL_FACTORY( WriteRichCalibrationsTool )

WriteRichCalibrationsTool::WriteRichCalibrationsTool( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent)
: GaudiTool( type,name,parent)
{
  // interface
  declareInterface<IWriteAlignmentConditionsTool>(this);
  // properties
  declareProperty("minHPDID"   , m_minHPDID =   0 );
  declareProperty("maxHPDID"   , m_maxHPDID = 484 );
  declareProperty("Directory", m_directory = "xml" );
  declareProperty("Precision", m_precision = 8u);
  declareProperty("Author", m_author = "");
  declareProperty("Version", m_version = "");
  declareProperty("Description", m_description = "");
  declareProperty("OnlineMode", m_online = false);
}

WriteRichCalibrationsTool::~WriteRichCalibrationsTool() {}

StatusCode WriteRichCalibrationsTool::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  /// Get pointer to detector
  m_RichSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return StatusCode::SUCCESS;
}


StatusCode WriteRichCalibrationsTool::write() const
{
  return write(m_version);
}


StatusCode WriteRichCalibrationsTool::write(const std::string& version) const
{
  StatusCode sc = StatusCode::SUCCESS;

  sc = writeRefIndex(version );
  sc = writeHPDAlign(version );
  sc = writeHPDOcc(version);
  sc = writeInactiveHPDs(version);

  if (sc.isFailure() ) return Warning( "Failed to write conditions to xml", StatusCode::FAILURE );
  return sc;
}

namespace {
  void stringreplace(std::string& astring,
                     const std::string& in,
                     const std::string& out,
                     size_t pos=0)
  {
    while( pos < astring.size() && (pos = astring.find(in,pos))!=std::string::npos) {
      astring.replace( pos, in.size(), out);
      pos += out.size();
    }
  }

  std::string formattedcondition( const Condition& condition, unsigned int precision )
  {
    std::ostringstream strstream;
    strstream << condition.toXml( "", false , precision ) << std::endl;
    std::string rc = strstream.str();

    stringreplace(rc,"condition>","condition>\n");

    return rc;
  }
}

StatusCode WriteRichCalibrationsTool::createXmlFile(const std::string& filename,
                                                    const std::string& version,
                                                    const std::string& contents) const
{
  /// create directory if it does not exist
  boost::filesystem::path apath(filename);
  boost::filesystem::create_directories(apath.parent_path());

  /// create the output file
  std::ofstream output( filename );
  if ( output.fail() ) {
    return Warning( "Failed to open output file " + filename, StatusCode::FAILURE );
  }

  // write the header
  if (!m_online) {
    output  << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
            << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n";
    output  << "<DDDB>\n\n";
  }

  // only write the comments if it is actually meanigfull
  if( !m_author.empty() || !version.empty() || !m_description.empty() ) {
    std::ostringstream comment;
    ST::XMLUtils::fullComment(comment,m_author,version,m_description);
    output << comment.str();
  }

  /// write the contents
  output << contents;

  /// write the footer
  if (!m_online) {
    output << "</DDDB>\n";
  }
  /// close the file
  output.close();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Write refractive index scale factor
//=============================================================================
StatusCode WriteRichCalibrationsTool::writeRefIndex(const std::string& version) const
{
  StatusCode sc = StatusCode::SUCCESS;

  const int totalRiches = 2;

  std::string riches[totalRiches] =
    {
      "Rich1",
      "Rich2"
    };

  std::string files[totalRiches] =
    {
      "Rich1/Environment/Gas.xml",
      "Rich2/Environment/Gas.xml"
    };

  for(int richID = 0; richID< totalRiches; richID++ )
  {
    std::string alignLoc = "/dd/Conditions/Environment/"+riches[richID]+"/RefractivityScaleFactor";
    Condition *myCond = get<Condition>( detSvc(), alignLoc );

    std::string fileName = m_directory + "/" + files[richID];
    sc = createXmlFile(fileName,version,formattedcondition( *myCond, m_precision ) );
  }

  return sc;
}

//=============================================================================
//  Write HPD occupancy
//=============================================================================
StatusCode WriteRichCalibrationsTool::writeHPDOcc(const std::string& version) const
{
  StatusCode sc = StatusCode::SUCCESS;

  const int totalRiches = 2;

  std::string riches[totalRiches] =
    {
      "Rich1",
      "Rich2"
    };

  std::string files[totalRiches] =
    {
      "Rich1/Environment/HPDOccupancies.xml",
      "Rich2/Environment/HPDOccupancies.xml"
    };

  for(int richID = 0; richID< totalRiches; richID++ )
  {
    std::string alignLoc = "/dd/Conditions/Environment/"+riches[richID]+"/AverageHPDOccupancies";
    Condition *myCond = get<Condition>( detSvc(), alignLoc );

    std::string fileName = m_directory + "/" + files[richID];
    sc = createXmlFile(fileName,version,formattedcondition( *myCond, m_precision ) );
  }

  return sc;
}

//=============================================================================
//  Write HPD Inactive list
//=============================================================================
StatusCode WriteRichCalibrationsTool::writeInactiveHPDs(const std::string& version) const
{
  StatusCode sc = StatusCode::SUCCESS;

  const int totalRiches = 2;

  std::string riches[totalRiches] =
    {
      "Rich1",
      "Rich2"
    };

  std::string files[totalRiches] =
    {
      "Rich1/InactiveHPDList",
      "Rich2/InactiveHPDList"
    };

  for(int richID = 0; richID< totalRiches; richID++ )
  {
    std::string alignLoc = "/dd/Conditions/Online/"+riches[richID]+"/InactiveHPDList";
    Condition *myCond = get<Condition>( detSvc(), alignLoc );

    std::string fileName = m_directory + "/" + files[richID];
    sc = createXmlFile(fileName,version,formattedcondition( *myCond, m_precision ) );
  }

  return sc;
}

//=============================================================================
//  Write HPD align
//=============================================================================
StatusCode WriteRichCalibrationsTool::writeHPDAlign(const std::string& version) const
{
  StatusCode sc = StatusCode::SUCCESS;

  const int totalPannels = 4;

  std::string files[totalPannels] =
    {
      "Rich1/Alignment/SiSensorsP0.xml",
      "Rich1/Alignment/SiSensorsP1.xml",
      "Rich2/Alignment/SiSensorsP0.xml",
      "Rich2/Alignment/SiSensorsP1.xml"
    };

  std::stringstream RichHpdAlign[totalPannels];

  // ==========================================
  // Loop over HPDs
  // ==========================================
  for(int hpdID = m_minHPDID; hpdID< m_maxHPDID; ++hpdID)
  {
    // HPD copy number
    const Rich::DAQ::HPDCopyNumber copyNumber =  Rich::DAQ::HPDCopyNumber( hpdID );

    // SmartID
    const LHCb::RichSmartID smartID = m_RichSys->richSmartID( copyNumber );

    if ( msgLevel(MSG::DEBUG) ) debug() << "HPD ID " << hpdID
                                        << ", smartID "  << smartID.toString()
                                        << ", smartID.key() " << smartID.key()
                                        << endmsg;

    // Get condition
    std::string subDet;
    if( smartID.rich() == Rich::DetectorType::Rich1 ) subDet = "Rich1";
    else subDet = "Rich2";

    std::string type = "SiSensor";

    std::string alignLoc = "/dd/Conditions/Alignment/"+subDet+"/"+type+ std::to_string( copyNumber.data() )+"_Align";

    Condition *myCond = get<Condition>( detSvc(), alignLoc );

    // Rich1 P0
    if( smartID.rich()     == Rich::DetectorType::Rich1
        && smartID.panel() == Rich::Side::top )
    {
      RichHpdAlign[0] << formattedcondition( *myCond, m_precision );
    }

    // Rich1 P1
    if( smartID.rich()     == Rich::DetectorType::Rich1
        && smartID.panel() == Rich::Side::bottom )
    {
      RichHpdAlign[1] << formattedcondition( *myCond, m_precision );
    }

    // Rich2 P0
    if( smartID.rich()     == Rich::DetectorType::Rich2
        && smartID.panel() == Rich::Side::left )
    {
      RichHpdAlign[2] << formattedcondition( *myCond, m_precision );
    }

    // Rich2 P1
    if( smartID.rich()     == Rich::DetectorType::Rich2
        && smartID.panel() == Rich::Side::right )
    {
      RichHpdAlign[3] << formattedcondition( *myCond, m_precision );
    }

  } // Loop over HPDs

  // ==========================================
  // Loop over pannels
  // ==========================================
  std::string fileName;

  for( int pID=0; pID<totalPannels; pID++ )
  {
    fileName = m_directory + "/" + files[pID];
    sc = createXmlFile( fileName, version, RichHpdAlign[pID].str() );
  }

  return sc;
}
