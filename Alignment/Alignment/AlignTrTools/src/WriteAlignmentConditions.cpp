// $Id: WriteAlignmentConditions.cpp,v 1.9 2010-01-12 12:09:19 wouter Exp $

// std
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sstream>
#include <iostream>


// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCb
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

// local
#include "WriteAlignmentConditions.h"

#include "Kernel/STXMLUtils.h"

DECLARE_ALGORITHM_FACTORY( WriteAlignmentConditions );

//--------------------------------------------------------------------
//
//  WriteAlignmentConditions : Prints out the full geometry tree
//
//--------------------------------------------------------------------

WriteAlignmentConditions::WriteAlignmentConditions( const std::string& name, 
                        ISvcLocator* pSvcLocator ) :
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructer
  declareProperty("topElement",  m_topElement  = "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"  );
  //  declareProperty("topElement",  m_topElement  = ""  );
  declareProperty("footer", m_footer = "</DDDB>");
  declareProperty("startTag", m_startTag = "<condition");
  declareProperty("outputFile",m_outputFileName = "alignment.xml");
  declareProperty("depths", m_depths );
  declareProperty("precision", m_precision = 16u);
  declareProperty("author", m_author = "Joe Bloggs");
  declareProperty("tag", m_tag = "Unknown");
  declareProperty("desc", m_desc = "blahblah");
  declareProperty("RemovePivotPoint", m_removePivot = true ) ;
}

WriteAlignmentConditions::~WriteAlignmentConditions()
{
  // destructer
}

StatusCode WriteAlignmentConditions::execute()
{
  // execute once per event
  return StatusCode::SUCCESS;
}

StatusCode WriteAlignmentConditions::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  
  info() << "topElement = " << m_topElement << "\n"
	 << "footer     = " << m_footer << "\n"
	 << "startTag   = " << m_startTag << "\n"
	 << "outputFile = " << m_outputFileName << "\n"
         << "precision  = " << m_precision << endreq;
  
  return sc;
}

void WriteAlignmentConditions::children(DetectorElement* parent, std::ofstream& out, unsigned int depth)
{
  if (parent != 0){
    const AlignmentCondition* aCon = parent->geometry()->alignmentCondition();
    if ( aCon ) {
      bool wanted = false;
      for ( unsigned int i = 0; i < m_depths.size(); i++ ) { // check if current depth is wanted for output
        if ( depth == m_depths[ i ] ) wanted = true;
      }
      if ( 0 == m_depths.size() ) wanted = true; // in case of empty list print all levels
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
      children(tElem,out,depth);
    }  
  }
}

void WriteAlignmentConditions::createDirectory( const std::string& dirname ) const
{
  // recursively create a directory
  size_t pos = dirname.find_last_of('/') ;
  if( pos != std::string::npos ) createDirectory( std::string(dirname.substr(0,pos)) ) ;
  std::cout << "creating dirname: " << dirname << std::endl ;
  if(mkdir(dirname.c_str(),0xFFFF)==0) {
    info() << "Creating directory: " << dirname << endreq ;
  } else if( errno != EEXIST ) {
    warning() << "Problem creating directory: mkdir returns " << errno << endreq ;
  }
}

StatusCode WriteAlignmentConditions::finalize()
{
  StatusCode sc = write(m_outputFileName) ;
  if( sc.isSuccess() ) sc = GaudiAlgorithm::finalize() ;
  return sc ;
}

StatusCode WriteAlignmentConditions::write(const std::string& filename)
{
  info() << "Writing alignment conditions to file: " << filename << endreq;
  // Print out the full tree
  info() << "Trying for top element " << m_topElement << endreq;

  DetectorElement* det =getDet<DetectorElement>(m_topElement);

  // make output dir if necessary
  size_t pos = m_outputFileName.find_last_of('/') ;
  if(  pos != std::string::npos ) 
    createDirectory( std::string(m_outputFileName.substr(0,pos)) ) ;
   
  std::ofstream outputFile(m_outputFileName.c_str());
  if (outputFile.fail() ){
    return Warning("Failed to open output file",StatusCode::FAILURE);
  }

  const Condition* aCon = det->geometry()->alignmentCondition();
  if (aCon) {
    outputFile << header(aCon->toXml("", true, m_precision)) << std::endl;

    // add some comments describing the file
    std::ostringstream comment;
    ST::XMLUtils::fullComment(comment,m_author, m_tag, m_desc);
    outputFile << comment.str() << std::endl;

    children(det, outputFile, 0);
    outputFile << footer() << std::endl;
  }
  else {
    warning() << "head has no condition " << endreq;
  }
  info() << "Written alignment condition" << endreq ;
  return StatusCode::SUCCESS ;
}

std::string WriteAlignmentConditions::footer() const{
  return m_footer;
} 

std::string WriteAlignmentConditions::header(const std::string& conString) const{

  // get the header
  std::string::size_type startpos = conString.find(m_startTag);
  std::string temp = conString.substr(0,startpos);
  // correct the location of the DTD
  std::string location;
  std::string::size_type pos = temp.find("structure");
  temp.insert(pos,location);
  return temp;
} 

std::string WriteAlignmentConditions::strip(const std::string& conString) const{
  std::string::size_type startpos = conString.find(m_startTag);
  std::string::size_type endpos = conString.find(m_footer);
  return conString.substr(startpos, endpos - startpos);
} 

void WriteAlignmentConditions::replaceChars(std::string& conString) const{

  std::string blank = " ";
  replace(conString, ",", blank);
  replace(conString, "\"/", "\"");
}

void WriteAlignmentConditions::replace(std::string& conString, std::string in, std::string out ) const{

  std::string::size_type pos = 0;
  while (pos != std::string::npos){
    pos = conString.find(in,pos);
    if (pos != std::string::npos) {
      conString.replace(pos,in.size(),out);
    }
  } //pos

} 
