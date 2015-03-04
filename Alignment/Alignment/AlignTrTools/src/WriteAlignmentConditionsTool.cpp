// $Id: WriteAlignmentConditions.cpp,v 1.9 2010-01-12 12:09:19 wouter Exp $

#include "AlignmentInterfaces/IWriteAlignmentConditionsTool.h"
#include "GaudiAlg/GaudiTool.h"

#include "DetDesc/DetectorElement.h"

class WriteAlignmentConditionsTool
  : public GaudiTool, virtual public IWriteAlignmentConditionsTool
{
public:

  /// constructer
  WriteAlignmentConditionsTool( const std::string& type,
        const std::string& name,
        const IInterface* parent); ///< Standard constructor

  /// destructer
  virtual ~WriteAlignmentConditionsTool();

  /// initialize
  StatusCode initialize();

  // write to file with name name
  StatusCode write(const std::string& filename) const ;

  // Everything configured via options
  StatusCode write() const ;

  // Everything configured via options
  StatusCode write(std::string& version) const ;

  // Everything configured via arguments
  StatusCode write( const std::string& filename,
        const std::string& topelement,
        const std::vector<unsigned int>& depths,
        const std::string& tag = "",
        const std::string& author = "",
        const std::string& description = "") const ;

private:
  std::string m_topElement;
  std::string m_footer;
  std::string m_startTag;

  /// Recursively print out the tree of the child DetectorElements
  void children(DetectorElement* parent, std::ofstream& out,
    const std::vector<unsigned int>& depths, unsigned int depth) const ;

  std::string footer() const;
  std::string header(const std::string& conString) const;
  std::string strip(const std::string& conString) const;
  void replaceChars(std::string& conString) const;
  void replace(std::string& conString,std::string in, std::string out ) const;
  void createDirectory( const std::string& dir ) const ;

  std::string m_outputFileName;
  unsigned int m_precision;
  std::vector<unsigned int> m_depths;

  std::string m_author;
  std::string m_desc;
  std::string m_version;
  bool m_removePivot;
  bool m_online;

} ;


/*********************************************************************************/
/* Implementation                                                                */
/*********************************************************************************/

// std
//#include <fcntl.h>
#include <sys/stat.h>
//#include <errno.h>
//#include <sstream>
#include <fstream>
//#include <boost/filesystem.hpp>
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( WriteAlignmentConditionsTool )

// Gaudi
// #include "GaudiKernel/AlgToolFactory.h"

// LHCb
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "Kernel/STXMLUtils.h"

//--------------------------------------------------------------------
//
//  WriteAlignmentConditionsTool : Prints out the full geometry tree
//
//--------------------------------------------------------------------

WriteAlignmentConditionsTool::WriteAlignmentConditionsTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent)
: GaudiTool( type,name,parent)
{
  // interface
  declareInterface<IWriteAlignmentConditionsTool>(this);
  // constructer
  declareProperty("topElement",  m_topElement  = "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"  );
  //  declareProperty("topElement",  m_topElement  = ""  );
  declareProperty("footer", m_footer = "</DDDB>");
  declareProperty("startTag", m_startTag = "<condition");
  declareProperty("outputFile",m_outputFileName = "alignment.xml");
  declareProperty("depths", m_depths );
  declareProperty("precision", m_precision = 16u);
  declareProperty("author", m_author = "Joe Bloggs");
  declareProperty("version", m_version = "Unknown");
  declareProperty("desc", m_desc = "blahblah");
  declareProperty("RemovePivotPoint", m_removePivot = true ) ;
  declareProperty("OnlineMode", m_online = false);
}

WriteAlignmentConditionsTool::~WriteAlignmentConditionsTool()
{
  // destructer
}

StatusCode WriteAlignmentConditionsTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  info() << "topElement = " << m_topElement << "\n"
  << "footer     = " << m_footer << "\n"
  << "startTag   = " << m_startTag << "\n"
  << "outputFile = " << m_outputFileName << "\n"
         << "precision  = " << m_precision << endreq;
  return sc;
}

void WriteAlignmentConditionsTool::children(DetectorElement* parent, std::ofstream& out,
              const std::vector<unsigned int>& depths,
              unsigned int depth ) const
{
  if (parent != 0){
    const AlignmentCondition* aCon = parent->geometry()->alignmentCondition();
    if ( aCon ) {
      bool wanted = false;
      for ( unsigned int i = 0; i < depths.size(); i++ ) { // check if current depth is wanted for output
        if ( depth == depths[ i ] ) wanted = true;
      }
      if ( 0 == depths.size() ) wanted = true; // in case of empty list print all levels
      if ( wanted ) {
  // if we want to remove the pivot point, first set it to zero
  bool removePivot = m_removePivot && aCon->exists("pivotXYZ") ;
  if( removePivot ) (const_cast<AlignmentCondition*>(aCon))->setPivotPoint( Gaudi::XYZPoint() ) ;
  // now get the condition as a single string
        std::string temp = strip(aCon->toXml("", false, m_precision));
  // now remove the pivot point from the output string
  if( removePivot ) {
    static const std::string stringToRemove = "<paramVector name=\"pivotXYZ\" type=\"double\">0 0 0</paramVector>" ;
    size_t pos = temp.find( stringToRemove ) ;
    if( pos != std::string::npos )
      temp.replace( pos, stringToRemove.size(), "") ;
  }
        replaceChars(temp);
        out << temp << std::endl;
      }
    }
    depth++;
    IDetectorElement::IDEContainer::const_iterator iChild = parent->childBegin();
    for ( ; iChild != parent->childEnd(); ++iChild ) {
      DetectorElement* tElem = dynamic_cast<DetectorElement*>(*iChild);
      children(tElem,out,depths,depth);
    }
  }
}

void WriteAlignmentConditionsTool::createDirectory( const std::string& dirname ) const
{
  // recursively create a directory
  size_t pos = dirname.find_last_of('/') ;
  if( pos != std::string::npos ) createDirectory( std::string(dirname.substr(0,pos)) ) ;
  //std::cout << "creating dirname: " << dirname << std::endl ;
  if (dirname.size()==0) return;
  if(mkdir(dirname.c_str(),0xFFFF)==0) {
    debug() << "Creating directory: " << dirname << endreq ;
  } else if( errno != EEXIST ) {
    warning() << "Problem creating directory: mkdir returns " << errno << endreq ;
  }
}

StatusCode WriteAlignmentConditionsTool::write() const
{
  return write(m_outputFileName,m_topElement,m_depths,m_version,m_author,m_desc) ;
}

StatusCode WriteAlignmentConditionsTool::write(const std::string& version) const
{
  return write(m_outputFileName,m_topElement,m_depths,version,m_author,m_desc) ;
}

 // Everything configured via arguments
StatusCode WriteAlignmentConditionsTool::write( const std::string& filename,
            const std::string& topelement,
            const std::vector<unsigned int>& depths,
            const std::string& tag,
            const std::string& author,
            const std::string& description) const
{
  info() << "Writing alignment conditions to file: " << filename << endreq;
  // Print out the full tree
  debug() << "Trying for top element " << topelement << endreq;

  DetectorElement* det =getDet<DetectorElement>(topelement);

  // make output dir if necessary
  size_t pos = filename.find_last_of('/') ;
  if(  pos != std::string::npos )
    createDirectory( std::string(filename.substr(0,pos)) ) ;

  std::ofstream outputFile(filename.c_str());
  if (outputFile.fail() ){
    return Warning("Failed to open output file",StatusCode::FAILURE);
  }

  const Condition* aCon = det->geometry()->alignmentCondition();
  if (aCon) {
    outputFile << header(aCon->toXml("", !m_online, m_precision)) << std::endl;

    // add some comments describing the file
    std::ostringstream comment;
    ST::XMLUtils::fullComment(comment,author, tag, description);
    outputFile << comment.str() << std::endl;

    children(det, outputFile, depths, 0);
    if (!m_online) outputFile << footer();
    outputFile << std::endl;
  }
  else {
    warning() << "head has no condition " << endreq;
  }
  debug() << "Written alignment condition" << endreq ;
  return StatusCode::SUCCESS ;
}

std::string WriteAlignmentConditionsTool::footer() const{
  return m_footer;
}

std::string WriteAlignmentConditionsTool::header(const std::string& conString) const{

  // get the header
  std::string::size_type startpos = conString.find(m_startTag);
  std::string temp = conString.substr(0,startpos);
  // correct the location of the DTD
  std::string location;
  std::string::size_type pos = temp.find("structure");
  if (pos != std::string::npos) {
     temp.insert(pos,location);
  }
  return temp;
}

std::string WriteAlignmentConditionsTool::strip(const std::string& conString) const{
  std::string::size_type startpos = conString.find(m_startTag);
  std::string::size_type endpos = conString.find(m_footer);
  return conString.substr(startpos, endpos - startpos);
}

void WriteAlignmentConditionsTool::replaceChars(std::string& conString) const{

  std::string blank = " ";
  replace(conString, ",", blank);
  replace(conString, "\"/", "\"");
}

void WriteAlignmentConditionsTool::replace(std::string& conString, std::string in, std::string out ) const{

  std::string::size_type pos = 0;
  while (pos != std::string::npos){
    pos = conString.find(in,pos);
    if (pos != std::string::npos) {
      conString.replace(pos,in.size(),out);
    }
  } //pos

}
