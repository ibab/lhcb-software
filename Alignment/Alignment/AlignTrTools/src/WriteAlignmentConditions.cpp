// $Id: WriteAlignmentConditions.cpp,v 1.4 2008-03-18 07:37:34 janos Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCb
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

// local
#include "WriteAlignmentConditions.h"


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
  declareProperty("depth", m_depth = 3u);
  declareProperty("precision", m_precision = 16u);
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
	 << "depth      = " << m_depth << "\n"
         << "precision  = " << m_precision << endreq;
  
  return sc;
}

void WriteAlignmentConditions::children(DetectorElement* parent, std::ofstream& out)
{
  if (parent != 0){
    const Condition* aCon = parent->geometry()->alignmentCondition();
    if (aCon) {
      std::string temp = strip(aCon->toXml("", false, m_precision));
      replaceChars(temp);
      out << temp << std::endl;
    }
    IDetectorElement::IDEContainer::const_iterator iChild = parent->childBegin();
    for ( ; iChild != parent->childEnd(); ++iChild ) {
      DetectorElement* tElem = dynamic_cast<DetectorElement*>(*iChild);
      children(tElem,out);
    }  
  }
}

StatusCode WriteAlignmentConditions::finalize()
{
  info() << "Writing alignment conditions to file" << endreq;
  // Print out the full tree
  info() << "Trying for top element " << m_topElement << endreq;

  DetectorElement* det =getDet<DetectorElement>(m_topElement);

  std::ofstream outputFile(m_outputFileName.c_str());
  if (outputFile.fail() ){
    return Warning("Failed to open output file",StatusCode::FAILURE);
  }

  const Condition* aCon = det->geometry()->alignmentCondition();
  if (aCon) {
    outputFile << header(aCon->toXml("", true, m_precision)) << std::endl;
    children(det, outputFile);
    outputFile << footer() << std::endl;
  }
  else {
    warning() << "head has no condition " << endreq;
  }
  return GaudiAlgorithm::finalize();
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
  replace(conString, "[", blank);
  replace(conString, "]", blank);
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
