#include "IOTCalibrationIOTool.h"
#include "GaudiAlg/GaudiTool.h"
#include "OTDet/DeOTDetector.h"
#include <iostream>
#include <fstream>
#include <sstream>

class OTCalibrationIOTool : public GaudiTool, public IOTCalibrationIOTool
{
public:
  OTCalibrationIOTool( const std::string& type, const std::string& name, const IInterface* parent ) ;
  ~OTCalibrationIOTool() ;
  StatusCode writeToFile(const std::string& filename, bool combineChannelsInOTIS) const;
  StatusCode readFromFile(const std::string& filename) const;
  StatusCode initialize() ;
private:
  DeOTDetector* m_otdet ;
} ;

#include "GaudiKernel/ToolFactory.h"
DECLARE_TOOL_FACTORY( OTCalibrationIOTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTCalibrationIOTool::OTCalibrationIOTool( const std::string& type,
				const std::string& name,
				const IInterface* parent )
  : GaudiTool( type, name, parent )
{
  declareInterface<IOTCalibrationIOTool>(this) ;
}

//=============================================================================
// Destructor
//=============================================================================
OTCalibrationIOTool::~OTCalibrationIOTool() 
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode OTCalibrationIOTool::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;
  // get otdet
  m_otdet = getDet<DeOTDetector>( DeOTDetectorLocation::Default );
  if( !m_otdet) {
    error() << "Cannot retrieve OTDET" << endmsg ;
    sc = StatusCode::FAILURE ;
  }
  return sc ;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
  os << vec.size() ;
  for(size_t i=0; i<vec.size(); ++i) os << " " << vec[i] ;
  return os ;
}

template<class T>
std::istream& operator>>(std::istream& is, std::vector<T>& vec)
{
  size_t size(0) ;
  is >> size ;
  vec.resize(size) ;
  for(size_t i=0; i<size; ++i) is >> vec[i] ;
  return is ;
}

StatusCode OTCalibrationIOTool::writeToFile(const std::string& filename,
					    bool combineChannelsInOTIS) const
{
  std::ofstream outfile(filename.c_str()) ;
  // write a format string (for if we ever want to change it)
  int format = 0 ;
  outfile << "Fileformat: " << format << std::endl ;
  
  // loop over all modules and write the calibration
  for(DeOTDetector::Modules::const_iterator it = m_otdet->modules().begin() ;
      it != m_otdet->modules().end(); ++it) {
    //
    const DeOTModule& mod = **it ;
    // write a tag identifying the module
    outfile << "Module: "
	    << mod.stationID() << " "
	    << mod.layerID() << " "
	    << mod.quarterID() << " "
	    << mod.moduleID() << std::endl ;
    // write the rt relation
    outfile << "RtRelation: " 
	    << mod.rtRelation().rmax() << " " 
	    << mod.rtRelation().tcoeff() << " "
	    << mod.rtRelation().terrcoeff() << std::endl ;
    // write the t0
    const std::vector<double>& t0vec = mod.getStrawT0s() ;
    if( !t0vec.empty() ) {
      outfile << "T0: " << t0vec << std::endl ;
    } else {
      // fall back on old behaviour if conditions are absent
      if( combineChannelsInOTIS ) {
	// store the t0 of the first straw in every OTIS
	size_t numOTIS = mod.nChannels()/32 ;
	outfile << "T0: " << numOTIS ;
	for( size_t i=0; i< numOTIS; ++i) outfile << " " << mod.strawT0( (i*32+1) ) ;
	outfile << std::endl ;
      } else {
	outfile << "T0: " << mod.nChannels() ;
	for( size_t i=0; i<mod.nChannels(); ++i) outfile << " " << mod.strawT0( (i+1) ) ;
      }
    }
  }
  return StatusCode::SUCCESS ;
}

StatusCode
OTCalibrationIOTool::readFromFile(const std::string& filename) const
{
  StatusCode sc = StatusCode::SUCCESS ;
  std::ifstream infile(filename.c_str()) ;
  if( !infile ) {
    error() << "Cannot find input file " << filename << endreq ;
    sc = StatusCode::FAILURE ;
  } else {
    info() << "Reading OT calibration from file " << filename << endreq ;
    char line[512] ;
    infile.getline(line,512) ;
  
    // Read the FileFormat key, if there is any
    int format(0) ;
    std::string linestring(line) ;
    size_t pos = linestring.find("Fileformat") ;
    if(pos != std::string::npos) {
      sscanf(line,"Fileformat: %d",&format) ;
    } else {
      // 'rewind' the file
      infile.close() ;
      infile.open(filename.c_str()) ;
    }
    
    // Now start reading lines. We expect blocks to look like
    // 'identifier' (module or higher), 'rt-relation', 't0'. When we
    // find an identifier, we first select all modules that are
    // compatible with it. After that, we read the data and apply to the
    // selecte modules.
    std::vector<DeOTModule*> selectedmodules ;
    while( infile.getline(line,512) )
      if(line[0] != '#') {
	std::string linestring(line) ;
	if( (pos = linestring.find("Module")) != std::string::npos) {
	  int stationID(-2), layer(-1), quarter(-1), module(-1) ;
	  sscanf(line,"Module: %d %d %d %d",&stationID,&layer,&quarter,&module) ;
	  // now select all compatible modules
	  selectedmodules.clear() ;
	  if(module != -1) {
	    LHCb::OTChannelID id(stationID,layer,quarter,module,0) ;
	    DeOTModule* module = m_otdet->findModule(id);
	    if(module) selectedmodules.push_back(module) ;
	    else {
	      error() << "cannot find module: " << line << endreq ;  
	      sc = StatusCode::FAILURE ;
	    }
	  } else if(stationID > 0) {
	    LHCb::OTChannelID id(stationID,0,0,0,0) ;
	    DeOTStation* station = m_otdet->findStation(id);
	    if(station) {
	      for(DeOTStation::Layers::const_iterator ilay = station->layers().begin() ;
		  ilay != station->layers().end(); ++ilay)
		for(DeOTLayer::Quarters::const_iterator iqu = (*ilay)->quarters().begin() ;
		    iqu != (*ilay)->quarters().end() ; ++iqu)
		  for(DeOTQuarter::Modules::const_iterator imod = (*iqu)->modules().begin() ;
		      imod != (*iqu)->modules().end() ; ++imod)
		    selectedmodules.push_back( const_cast<DeOTModule*>(*imod) ) ;
	      info() << "OTCalibrationIOTool: Reading calibration for station: " << stationID << endreq ;
	    } else {
	      error() << "OTCalibrationIOTool: cannot find station with stationID=" << stationID << endreq ; 
	      sc = StatusCode::FAILURE ;
	    }
	  } else {
	    info() << "OTCalibrationIOTool: Reading a single calibration entry for all modules." << endreq ;
	    for(DeOTDetector::Modules::const_iterator imod = m_otdet->modules().begin() ;
		imod != m_otdet->modules().end(); ++imod)
	      selectedmodules.push_back( const_cast<DeOTModule*>(*imod) ) ;
	  }
	} else if( (pos = linestring.find("RtRelation")) != std::string::npos) {
	  // this must be a line with constants
	  std::vector<double> tcoeff,terrcoeff ;
	  double rmax(0) ;
	  std::istringstream is(line+11) ;
	  is >> rmax >> tcoeff >> terrcoeff ;
	  OTDet::RtRelation rt(rmax,tcoeff,terrcoeff ) ;
	  for( std::vector<DeOTModule*>::const_iterator it = selectedmodules.begin() ;
	       it != selectedmodules.end(); ++it) 
	    (*it)->setRtRelation( rt ) ;
	} else if( (pos = linestring.find("T0")) != std::string::npos) {
	  std::vector<double> t0vec ;
	  std::istringstream is(line+3) ;
	  is >> t0vec ;
	  for( std::vector<DeOTModule*>::const_iterator it = selectedmodules.begin() ;
	       it != selectedmodules.end(); ++it) 
	    (*it)->setStrawT0s( t0vec ) ;
	}
      }	
  }

  return sc ;
}
