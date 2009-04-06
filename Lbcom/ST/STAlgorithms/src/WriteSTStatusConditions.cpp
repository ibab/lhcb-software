// $Id: WriteSTStatusConditions.cpp,v 1.1 2009-04-06 07:50:48 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"


// local
#include "WriteSTStatusConditions.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"


DECLARE_ALGORITHM_FACTORY( WriteSTStatusConditions );

//--------------------------------------------------------------------
//
//  WriteSTStatusConditions
//
//--------------------------------------------------------------------

WriteSTStatusConditions::WriteSTStatusConditions( const std::string& name, 
                        ISvcLocator* pSvcLocator ) :
  ST::AlgBase(name, pSvcLocator)
{
  // constructer
  declareProperty("footer", m_footer = "</DDDB>");
  declareProperty("startTag", m_startTag = "<condition");
  declareProperty("outputFile",m_outputFileName = "ReadoutSectors.xml");
  declareProperty("depths", m_depth = 3u );
  declareProperty("precision", m_precision = 16u);
  declareProperty("removeCondb", m_removeCondb = true);
}

WriteSTStatusConditions::~WriteSTStatusConditions()
{
  // destructer
}

StatusCode WriteSTStatusConditions::execute()
{
  // execute once per event
  return StatusCode::SUCCESS;
}

StatusCode WriteSTStatusConditions::initialize() {
  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()){
    return Error("Failed to intialize");
  }  
  
  info() << "detector = " << detType() << "\n"
	 << "footer     = " << m_footer << "\n"
	 << "startTag   = " << m_startTag << "\n"
	 << "outputFile = " << m_outputFileName << "\n"
         << "precision  = " << m_precision << endreq;
  
  return sc;
}


StatusCode WriteSTStatusConditions::finalize()
{
  info() << "Writing alignment conditions to file" << endreq;
  // Print out the full tree
  info() << "Trying for top element " << tracker()->name() << endreq;

  // get the sectors 
  const DeSTDetector::Sectors& sectors = tracker()->sectors();

  std::ofstream outputFile(m_outputFileName.c_str());
  if (outputFile.fail() ){
    return Warning("Failed to open output file",StatusCode::FAILURE);
  }

  // write the xml headers 
  outputFile << header(sectors.front()->statusCondition()->toXml("", true, m_precision)) << std::endl;

  for (DeSTDetector::Sectors::const_iterator iterS = sectors.begin();
         iterS != sectors.end(); ++iterS){
    const Condition* aCon = (*iterS)->statusCondition();
    std::string temp = strip(aCon->toXml("", false, m_precision));
    replaceChars(temp);
    outputFile << temp << "\n"  << std::endl; 
  } // sectors

  // write the footer
  outputFile << footer() << std::endl;

  return ST::AlgBase::finalize();
}

std::string WriteSTStatusConditions::footer() const{
  return m_footer;
} 

std::string WriteSTStatusConditions::header(const std::string& conString) const{

  // get the header
  std::string::size_type startpos = conString.find(m_startTag);
  std::string temp = conString.substr(0,startpos);
  if (m_removeCondb) replace(temp,"conddb:", " ");
  // correct the location of the DTD
  std::string location;
  for (unsigned int i = 0;  i< m_depth; ++i) location += "../";
  std::string::size_type pos = temp.find("/DTD/");
  temp.insert(pos,location);
  replace(temp,"//", "/");
  return temp;
} 

std::string WriteSTStatusConditions::strip(const std::string& conString) const{
  std::string::size_type startpos = conString.find(m_startTag);
  std::string::size_type endpos = conString.find(m_footer);
  return conString.substr(startpos, endpos - startpos);
} 

void WriteSTStatusConditions::replaceChars(std::string& conString) const{

  std::string blank = " ";
  replace(conString, ",", blank);
  replace(conString, "[", blank);
  replace(conString, "]", blank);
  replace(conString, "\"/", "\"");
}

void WriteSTStatusConditions::replace(std::string& conString, std::string in, std::string out ) const{

  std::string::size_type pos = 0;
  while (pos != std::string::npos){
    pos = conString.find(in,pos);
    if (pos != std::string::npos) {
      conString.replace(pos,in.size(),out);
    }
  } //pos

} 
