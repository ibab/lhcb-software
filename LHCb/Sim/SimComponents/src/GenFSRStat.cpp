// Include files 

 // from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "GenFSRStat.h"

// from Event                                                                                                                                  
#include "Event/GenFSR.h"
#include "Event/GenCountersFSR.h"
#include "Event/CrossSectionsFSR.h"

// other libraries                                                                                                                            
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <stdio.h>
#include <curl/curl.h>
#include "TSystem.h"

#define script_version "20151008"

//-----------------------------------------------------------------------------
// Implementation file for class : GenFSRStat
//
// 2015-08-03 : Davide Fazzini
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GenFSRStat )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GenFSRStat::GenFSRStat( const std::string& name,
                        ISvcLocator* pSvcLocator)
: GaudiAlgorithm (name, pSvcLocator),
  m_fileRecordSvc(NULL),
  m_navigatorTool(NULL)
{
  declareProperty("FileRecordLocation", m_fileRecordName = "/FileRecords");
  declareProperty("FSRName"           , m_FSRName = "/GenFSR");
  declareProperty("HtmFileLocation"   , m_htmlFileLocation = "");
  declareProperty("HtmlFileName"      , m_htmlFileName = "GenerationFSRnew.html");
  declareProperty("prodID"            , m_prodID = "");
  declareProperty("appConfigVersion"  , m_appConfigVersion = "");
  declareProperty("appConfigFile"     , m_appConfigFile = "");
  declareProperty("gaussVersion"      , m_gaussVersion = "");
  declareProperty("simCond"           , m_simCond = "");
  declareProperty("dddb"              , m_dddb = "");
}

//=============================================================================
// Destructor
//=============================================================================
GenFSRStat::~GenFSRStat() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GenFSRStat::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // get the File Records service                                                                                                              
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);

  // prepare navigator tool                                                                                 
  m_navigatorTool = tool<IFSRNavigator>("FSRNavigator", "FSRNavigator");  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GenFSRStat::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GenFSRStat::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  GenFSRStat::printHtmlFSR();

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================


//=============================================================================                                                                
//  Print the GenFSR in a file xml                                                                                                             
//=============================================================================             

void GenFSRStat::printHtmlFSR()
{
  if (msgLevel(MSG::DEBUG)) debug() << "write to file: " << m_htmlFileName << endmsg;

  // root of store                                               
  std::string fileRecordRoot = m_fileRecordName;
  int nFSRs = 0;

  GenFSRStat::getEvtTypeDesc(100);

  // make an inventory of the FileRecord store                                                                                        
  std::vector< std::string > addresses = m_navigatorTool->navigate(fileRecordRoot, m_FSRName);  

  for(std::vector< std::string >::iterator iAddr=addresses.begin(); iAddr!=addresses.end(); iAddr++)
    nFSRs += 1;
  
  if(nFSRs != 0)
  {
    for(std::vector< std::string >::iterator iAddr=addresses.begin(); iAddr!=addresses.end(); iAddr++)
    {
      std::string genRecordAddress = *iAddr;
      std::string line = "";
      
      // read GenFSR          
      LHCb::GenFSR* genFSR = getIfExists<LHCb::GenFSR>(m_fileRecordSvc, genRecordAddress);
      if (genFSR==NULL)
      {
        Warning("A genFSR record was not found").ignore();
        if (msgLevel(MSG::DEBUG)) debug() << genRecordAddress << " not found" << endmsg;        
      }
      else
      {
        int evType = genFSR->getSimulationInfo("evtType", 0);
        int njobs = genFSR->getSimulationInfo("nJobs", 0);
        std::string genMethod = genFSR->getSimulationInfo("generationMethod", "");
        std::string genName = genFSR->getSimulationInfo("hardGenerator", "");
        std::string decFiles = genFSR->getSimulationInfo("decFiles", "");

        std::string evType_str =  dynamic_cast<std::ostringstream*>(&(std::ostringstream() << evType))->str();
        std::string njobs_str =  dynamic_cast<std::ostringstream*>(&(std::ostringstream() << njobs))->str();

        int after = 0, before = 0;
        double value = 0;
        int count1 = 0, count2 = 0, count3 = 0, count4 = 0, count5 = 0;
        std::string time = "";
        bool flag_evType = false;
        std::size_t pos_end, pos_ref;
        
        time = getCurrentTime();

        LHCb::GenFSR::GeneratorCounter::iterator icounter;
        LHCb::GenFSR::GeneratorCounter genCounters = genFSR->getGenCounters();
        LHCb::GenFSR::CrossSection::iterator icross;        
        LHCb::GenFSR::CrossSection crossSections = genFSR->getCrossSections();
        int key = 0;        

        // look in the FSR what kind if counters are stored
        for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
        {
          key = icounter->first;
          
          if(key ==  7 || key == 27 || key == 91 || key == 92 || key == 94 || key == 96)
          {
            before = genFSR->getDenominator(key);
            after = genFSR->getGenCounterInfo(key).second;
            if(before == 0 || after == 0) continue;
            count1 += 1;
          }
          else if (key < 24 && key != 6 && key != 7)
          {
            before = genFSR->getDenominator(key);
            after = genFSR->getGenCounterInfo(key).second;
            if(before == 0 || after == 0) continue;
            count2 += 1;
          }
          else if((key>=30 && key<=40) || (key>=55 && key<=63) || (key>=75 && key<=77) || (key>=85 && key<=87))
          {
            before = genFSR->getDenominator(key);
            after = genFSR->getGenCounterInfo(key).second;
            if(before == 0 || after == 0) continue;
            count3 += 1;
          }
          else if ((key>=41 && key<=51) || (key>=64 && key<=72) || (key>=78 && key<=80) || (key>=88 && key<=90))
          {
            before = genFSR->getDenominator(key);
            after = genFSR->getGenCounterInfo(key).second;
            if(before == 0 || after == 0) continue;
            count4 += 1;
          }
        }

        // look in the FSR if there are any generator cross-section is stored        
        for(icross = crossSections.begin(); icross != crossSections.end(); icross++)          
        {
          value = icross->second.second;
          if(value == 0 || isnan(value) != 0) continue;
          count5 += 1;
        }
        
        // read the already existing file 
        std::fstream readFile;
        std::string readFileName = m_htmlFileLocation + "GenerationFSR_" + m_appConfigFile + ".html";
     
        readFile.open(readFileName.c_str(), std::fstream::in);
        // open the new file
        m_htmlFile.open((m_htmlFileLocation + m_htmlFileName).c_str(), std::fstream::out);
    
        if (m_htmlFile.is_open())
        {
          if (msgLevel(MSG::DEBUG)) debug() << "Html file: " << m_htmlFileName.c_str() << " - created in the following directory: " 
                                            << m_htmlFileLocation.c_str() << endmsg;
          
          if (readFile.is_open())
          {
            if (msgLevel(MSG::DEBUG)) debug() << "Read file GenerationFSR_"+ m_appConfigFile + ".html opened" << endmsg;
            
            // hyper-link related to this simulation conditions
            std::string ref_evType = "<a href=\"GenerationFSR_"+m_appConfigFile+".html#"+evType_str+"\">"+evType_str+"</a>";

            // link related to this simulation conditions
            std::string link = "<a name=\""+evType_str+"\"></a>";

            // read the existing file GenerationFSR.html
            while(getline(readFile, line))
            {
              // look for the hyper-link in the current line
              if(!flag_evType) pos_ref = line.find(ref_evType);
              
              if(pos_ref != std::string::npos && flag_evType == false)
                flag_evType = true;

              // if there is not already a simulation for this production, here we need to add the hyperlink to the table
              if(line == "</p>")
              {
                if(flag_evType) m_htmlFile << line << std::endl;
                else            m_htmlFile << ref_evType << std::endl << line << std::endl;
              }
              // add the tables if the current line contains the right link or if the end of the file is reached
              else if((line == link && flag_evType == true) || (flag_evType == false && line == "</body>"))
              {
                // There is already a simulation for this production       
                // but if we are here it's because we have to overwrite the previous results
                m_htmlFile << link << std::endl;  // write link

                std::string evtDesc = getEvtTypeDesc(evType);
                
                // Write the tables       
                m_htmlFile << "<table>\n<th colspan=\"2\"><font color=\"#0000FF\">" << evType_str << " (" << evtDesc << ")</font> ";
                m_htmlFile << "Hard Generator <font color=\"#088A08\">" << genName << "</font> ";
                m_htmlFile << "Gauss <font color=\"#088A08\">" << m_gaussVersion << "</font>";                
                m_htmlFile << "<br> DecFiles <font color=\"#088A08\">" << decFiles << "</font>";
                m_htmlFile << "<br> DDDB <font color=\"#FF0000\">" << m_dddb << "</font> - ";
                m_htmlFile << "SIMCOND <font color=\"#FF0000\">" << m_simCond << "</font><br>";
                m_htmlFile << "APPCONFIG <font color=\"#088A08\">" << m_appConfigVersion << "</font> ";
                m_htmlFile << "<font color=\"#FF0000\">" << m_appConfigFile << "</font><br>";                
                m_htmlFile << time << "</th>\n<tr>\n\n";
                
                if(count2 != 0) writeGeneratorCounters(genFSR, count2);
                if(count3 != 0) writeGenHadronCounters(genFSR, count3);
                if(count4 != 0) writeAccHadronCounters(genFSR, count4);
                if(count5 != 0) writeGeneratorCrossSections(genFSR, count5);
                if(count1 != 0) writeCutEfficiencies(genFSR, count1);

                m_htmlFile << "</table>\n";

                // Write the comments
                if(count2 == 0) m_htmlFile << "<p><font color=\"#FF0000\">Warning: No Counters Interactions lines found</font></p>\n";
                if(count3 == 0) m_htmlFile << "<p><font color=\"#FF0000\">Warning: No Gnerated Hadron Counters lines found</font></p>\n";
                if(count4 == 0) m_htmlFile << "<p><font color=\"#FF0000\">Warning: No Accepted Hadron Counters lines found</font></p>\n";
                if(count5 == 0) m_htmlFile << "<p><font color=\"#FF0000\">Warning: No Generator Cross-Sections lines found</font></p>\n";
                if(count1 == 0) m_htmlFile << "<p><font color=\"#FF0000\">Warning: No Cut Efficiencies Counters lines found</font></p>\n";

                writeFinalComments(genFSR);                

                m_htmlFile << "<p> Statistics done (script version " << script_version << ") with " << njobs_str <<
                  " jobs from the following ProdID:<font color=\"#0000FF\">" << m_prodID << "</font></p>\n";

                // skip the lines which contain the previous table because they have been just updated
                if(flag_evType) getline(readFile, line);
                
                pos_end = line.find("<a name=");

                while(pos_end == std::string::npos && line != "</body>")
                {
                  getline(readFile, line);
                  pos_end = line.find("<a name=");
                }
                
                m_htmlFile << line << std::endl;
              }
              else  m_htmlFile << line << std::endl;              
            }
          }
          // the file does not exist, create it              
          else
          {
            if (msgLevel(MSG::DEBUG)) debug() << "GenerationFSR.html does not exist, creating it now." << endmsg;
            
            // write the web-header
            m_htmlFile << "<html>\n<head>\n";            
            m_htmlFile << "<link rel=\"stylesheet\" href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/css/lhcb.css\" type=\"text/css\" media=\"screen\">\n";           
            m_htmlFile << "<link rel=\"stylesheet\" href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/gauss/css/css.css\" type=\"text/css\" media=\"screen\">\n";
            m_htmlFile << "<link rel=\"stylesheet\" href=\"css.css\" type=\"text/css\" media=\"screen\">\n";
            m_htmlFile << "<title>Table for Generation</title>\n";
            m_htmlFile << "</head>\n";

            m_htmlFile << "\n<body>\n";
            m_htmlFile << "<p> Event Type:\n<a href=\"GenerationFSR_" << m_appConfigFile << ".html#" << evType_str << "\">" << evType_str << "</a>\n</p>\n";
            // link
            m_htmlFile << "<a name=\"" << evType_str << "\"></a>\n" << std::endl;
            
            std::string evtDesc = getEvtTypeDesc(evType);
            // Write the tables                                                                                                             
            m_htmlFile << "<table>\n<th colspan=\"2\"><font color=\"#0000FF\">" << evType_str << " (" << evtDesc << ")</font> ";
            m_htmlFile << "Hard Generator <font color=\"#088A08\">" << genName << "</font> ";
            m_htmlFile << "Gauss <font color=\"#088A08\">"<< m_gaussVersion << ": ";
            m_htmlFile << "</font>";
            m_htmlFile << "<br> DecFiles <font color=\"#088A08\">" << decFiles << "</font>";
            m_htmlFile << "<br> DDDB <font color=\"#FF0000\">" << m_dddb << "</font> - ";
            m_htmlFile << "SIMCOND <font color=\"#FF0000\">" << m_simCond << "</font><br>";
            m_htmlFile << "APPCONFIG <font color=\"#088A08\">" << m_appConfigVersion << "</font>";
            m_htmlFile << "<font color=\"#FF0000\">" << m_appConfigFile << "</font><br>";
            m_htmlFile << time << "</th>\n<tr>\n\n";

            if(count2 != 0) writeGeneratorCounters(genFSR, count2);
            if(count3 != 0) writeGenHadronCounters(genFSR, count3);
            if(count4 != 0) writeAccHadronCounters(genFSR, count4);
            if(count5 != 0) writeGeneratorCrossSections(genFSR, count5);            
            if(count1 != 0) writeCutEfficiencies(genFSR, count1);

            m_htmlFile << "</table>\n";

            // Write the warnings                                                                                                         
            if(count2 == 0) m_htmlFile << "<p><font color=\"#FF0000\">Warning: No Hadron Counters lines found</font></p>\n";
            if(count3 == 0) m_htmlFile << "<p><font color=\"#FF0000\">Warning: No Signal Counters lines found</font></p>\n";
            if(count4 == 0) m_htmlFile << "<p><font color=\"#FF0000\">Warning: No Accepted Hadron Counters lines found</font></p>\n";
            if(count5 == 0) m_htmlFile << "<p><font color=\"#FF0000\">Warning: No Generator Cross-Sections lines found</font></p>\n";
            if(count1 == 0) m_htmlFile << "<p><font color=\"#FF0000\">Warning: No Counters Interactions lines found</font></p>\n";

            writeFinalComments(genFSR);
            
            m_htmlFile << "<p> Statistics done (script version " << script_version << ") with " << njobs_str << 
              " jobs from the following ProdID:<font color=\"#0000FF\">" << m_prodID << "</font></p>\n";

            m_htmlFile << "\n</body>\n</html>\n";
          }
          m_htmlFile.close();

          // remove the old file
          std::string originalFile = m_htmlFileLocation + "GenerationFSR_" + m_appConfigFile + ".html";

          remove(originalFile.c_str());
          std::string newFile = m_htmlFileLocation + m_htmlFileName;

          // create the new GenerationFSR.html 
          rename(newFile.c_str(),originalFile.c_str());
        }
      }
    }
  }
}

// Follow some functions to write the lines related to each table
// Each function contains different loops for each information requested
// (name, value, error)

//=============================================================================                                                                
//  Add generator level counters in the html file       
//=============================================================================                                                                

void GenFSRStat::writeGeneratorCounters(LHCb::GenFSR* genFSR, int count)
{
  LHCb::GenFSR::GeneratorCounter::iterator icounter; 
  LHCb::GenFSR::GeneratorCounter genCounters = genFSR->getGenCounters();
  int key = 0;  
  std::string name = "", fullName = "";
  longlong after = 0, before = 0;
  double fraction = 0, error = 0, C = 1;
  bool flag = true;

  LHCb::CrossSectionsFSR crossFSR;
  std::map<std::string,std::string> mapCross = crossFSR.getFullNames();

  // write the table options
  m_htmlFile << "<tr>\n<td align=center>\n<table border=2>\n<th colspan=" << count;
  m_htmlFile << "><div class=firstcell>Counters for Interactions</div></th>\n" << std::endl;

  m_htmlFile << "<tr>\n";
  
  // write the list of cross-section names
  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {    
    key = icounter->first;
    if(key > 24) continue;
    if(key == 6 || key == 7) continue;
    name =  LHCb::CrossSectionsFSR::CrossSectionKeyToString(key);
    fullName = mapCross[name];
    before = genFSR->getDenominator(key);
    after = genFSR->getGenCounterInfo(key).second;
    if(before == 0 || after == 0) continue;
    if(key >= 10) m_htmlFile << "<td><font size=2>" << fullName << " (mb)</font></td>\n";
    else m_htmlFile << "<td><font size=2>" << fullName << "</font></td>\n";
  }
  m_htmlFile <<"</tr>\n\n<tr>\n";
  
  // write the value of each fraction
  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;
    if(key > 24) continue; 
    if(key == 6 || key == 7) continue;
    before = genFSR->getDenominator(key);
    if(before == 0) continue;
    if(key >= 10)
    { 
      int keycs = LHCb::CrossSectionsFSR::CrossSectionKeyToType("MBCrossSection"); 
      C = genFSR->getCrossSectionInfo(keycs).second;
    }
    else C = 1;
    
    if(key == 2) after = genFSR->getGenCounterInfo(key+1).second;
    else after = icounter->second.second;
    if(after == 0) continue;
    fraction = genFSR->getEfficiency(after, before, C);
  
    m_htmlFile << "<td>" << fraction << "</td>\n";
  }
  m_htmlFile <<"</tr>\n\n<tr>\n";

  // write the related error of each fraction
  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;
    if(key > 24) continue; 
    if(key == 6 || key == 7) continue;
    before = genFSR->getDenominator(key);
    if(before == 0) continue;
    if(key >= 10)
    {
      int keycs = LHCb::CrossSectionsFSR::CrossSectionKeyToType("MBCrossSection");
      C = genFSR->getCrossSectionInfo(keycs).second; 
    }
    else
      C = 1;
    
    if(key == 2) after = genFSR->getGenCounterInfo(key+1).second;
    else after = icounter->second.second;  
    if(after == 0) continue;
    name =  LHCb::CrossSectionsFSR::CrossSectionKeyToString(key);
    if(name == "MeanNoZeroPUInt" || name == "MeanPUInt" || name == "MeanPUIntAcc") flag = false;
    else flag = true;

    error = genFSR->getEfficiencyError(after, before, C, flag);

    m_htmlFile << "<td>&plusmn; " << error << "</td>\n"; 
  }
  m_htmlFile << "</tr>\n</table>\n</td>\n</tr>" << std::endl;
}

//=============================================================================                      
//  Add generator level cut efficiencies in the xml file                                                                                       
//=============================================================================    
void GenFSRStat::writeCutEfficiencies(LHCb::GenFSR* genFSR, int count)
{
  LHCb::GenFSR::GeneratorCounter::iterator icounter; 
  LHCb::GenFSR::GeneratorCounter genCounters = genFSR->getGenCounters();
  int key = 0;
  std::string name = "", fullName = "";
  longlong after = 0, before = 0;
  double fraction = 0, error = 0;
  
  LHCb::CrossSectionsFSR crossFSR;
  std::map<std::string,std::string> mapCross = crossFSR.getFullNames();

  m_htmlFile << "<tr>\n<td align=center>\n<table border=2>\n<th colspan=" << count;
  m_htmlFile << "><div class=firstcell>Generator Efficiencies</div></th>\n";
  m_htmlFile << "<tr>\n";
  
  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;
    if(key !=  7 && key != 27 && key != 94 && key != 96) continue;
    name =  LHCb::CrossSectionsFSR::CrossSectionKeyToString(key);
    fullName = mapCross[name];
    before = genFSR->getDenominator(key);
    if(before == 0) continue;
    after    = icounter->second.second;
    if(after == 0) continue;
    m_htmlFile << "<td><font size=2>" << fullName << "</font></td>\n";
  }  
  m_htmlFile <<"</tr>\n\n<tr>\n";

  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;
    if(key !=  7 && key != 27 && key != 94 && key != 96) continue;
    before = genFSR->getDenominator(key);
    if(before == 0) continue;
    after    = icounter->second.second;
    if(after == 0) continue;
    fraction = genFSR->getEfficiency(after, before);

    m_htmlFile << "<td>" << fraction << "</td>\n";
  }
  m_htmlFile <<"</tr>\n\n<tr>\n";

  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;  
    if(key !=  7 && key != 27 && key != 94 && key != 96) continue;
    before = genFSR->getDenominator(key);
    if(before == 0) continue;
    after = icounter->second.second;
    if(after == 0) continue;
    error = genFSR->getEfficiencyError(after, before);
    
    m_htmlFile << "<td>&plusmn; " << error << "</td>\n";
  }
  m_htmlFile << "</tr>\n</table>\n</td>\n</tr>" << std::endl;
}

//=============================================================================                                                                
//  Add generated hadron counters in the html file             
//=============================================================================                                                                

void GenFSRStat::writeGenHadronCounters(LHCb::GenFSR* genFSR, int countGen)
{
  LHCb::GenFSR::GeneratorCounter::iterator icounter;
  LHCb::GenFSR::GeneratorCounter genCounters = genFSR->getGenCounters();
  int key = 0;  
  std::string name = "", fullName = "";
  longlong after = 0, before = 0;  
  double fraction = 0, error = 0;
  LHCb::CrossSectionsFSR crossFSR;
  std::map<std::string,std::string> mapCross = crossFSR.getFullNames();

  m_htmlFile << "<tr>\n<td align=center>\n<table border=2>\n<th colspan=" << countGen;
  m_htmlFile << "><div class=firstcell> Generated Hadron Counters</div></th>\n" << std::endl;
  m_htmlFile << "<tr>\n";

  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {    
    key = icounter->first;
    // Generated Hadron Fractions                                                                                                         
    if((key>=30 && key<=40) || (key>=55 && key<=63) || (key>=75 && key<=77) || (key>=85 && key<=87))
    {
      name = LHCb::CrossSectionsFSR::CrossSectionKeyToString(key);
      fullName = mapCross[name];
      before = genFSR->getDenominator(key);
      if(before == 0) continue;
      after = genFSR->getGenCounterInfo(key).second;
      if(after == 0) continue;
      m_htmlFile << "<td><font size=2>" << fullName << "</font></td>\n";
    }
  }
  m_htmlFile <<"</tr>\n\n<tr>\n";

  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;
    // Generated Hadron Fractions                                                                                                              
    if((key>=30 && key<=40) || (key>=55 && key<=63) || (key>=75 && key<=77) || (key>=85 && key<=87))
    {
      before = genFSR->getDenominator(key);
      if(before == 0) continue;     
      after = icounter->second.second;
      if(after == 0) continue;
      fraction = genFSR->getEfficiency(after, before);

      m_htmlFile << "<td>" << fraction << "</td>\n";
    }    
  }
  m_htmlFile <<"</tr>\n\n<tr>\n";

  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;
    // Generated Hadron Fractions                                                                                                              
    if((key>=30 && key<=40) || (key>=55 && key<=63) || (key>=75 && key<=77) || (key>=85 && key<=87))
    {      
      before = genFSR->getDenominator(key);
      if(before == 0) continue;
      after = icounter->second.second;
      if(after == 0) continue;
      error = genFSR->getEfficiencyError(after, before);

      m_htmlFile << "<td>&plusmn; " << error << "</td>\n"; 
    } 
  }
  
  m_htmlFile << "</tr>\n</table>\n</td>\n</tr>" << std::endl;
}

//=============================================================================                                                                
//  Add accepted hadron counters in the html file                                  
//=============================================================================                                                                

void GenFSRStat::writeAccHadronCounters(LHCb::GenFSR* genFSR, int countAcc)
{
  LHCb::GenFSR::GeneratorCounter::iterator icounter;
  LHCb::GenFSR::GeneratorCounter genCounters = genFSR->getGenCounters();  
  int key = 0;
  longlong after = 0, before = 0;
  double fraction = 0, error = 0;
  std::string name = "", fullName = "";
  LHCb::CrossSectionsFSR crossFSR;
  std::map<std::string,std::string> mapCross = crossFSR.getFullNames();
  
  m_htmlFile << "<tr>\n<td align=center>\n<table border=2>\n<th colspan=" << countAcc;  
  m_htmlFile << "><div class=firstcell> Accepted Hadron Counters</div></th>\n" << std::endl;
  m_htmlFile << "<tr>\n";
  
  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;
    // Accepted Hadron Fractions               
    if ((key>=41 && key<=51) || (key>=64 && key<=72) || (key>=78 && key<=80) || (key>=88 && key<=90))
    {
      name = LHCb::CrossSectionsFSR::CrossSectionKeyToString(key);
      fullName = mapCross[name];
      before = genFSR->getDenominator(key);
      if(before == 0) continue;
      after = genFSR->getGenCounterInfo(key).second;
      if(after == 0) continue;
      m_htmlFile << "<td><font size=2>" << fullName << "</font></td>\n"; 
    } 
  }
  m_htmlFile <<"</tr>\n\n<tr>\n";

  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;
    // Accepted Hadron Fractions   
    if ((key>=41 && key<=51) || (key>=64 && key<=72) || (key>=78 && key<=80) || (key>=88 && key<=90))
    {
      before = genFSR->getDenominator(key);
      if(before == 0) continue;
      after = icounter->second.second;
      if(after == 0) continue;
      fraction = genFSR->getEfficiency(after, before);

      m_htmlFile << "<td>" << fraction << "</td>\n";      
    }
  }
  m_htmlFile << "</tr>\n\n<tr>\n";

  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {
    key = icounter->first;
    // Accepted Hadron Fractions               
    if ((key>=41 && key<=51) || (key>=64 && key<=72) || (key>=78 && key<=80) || (key>=88 && key<=90))
    {
      before = genFSR->getDenominator(key);
      if(before == 0) continue;
      after = icounter->second.second;       
      if(after == 0) continue;
      error    = genFSR->getEfficiencyError(after, before);

      m_htmlFile << "<td>&plusmn; " << error << "</td>\n";  
    }
  }
  m_htmlFile << "</tr>\n</table>\n</td>\n</tr>" << std::endl;
}

//=============================================================================                                                                
//  Add accepted hadron counters in the html file
//=============================================================================                                                                

void GenFSRStat::writeGeneratorCrossSections(LHCb::GenFSR* genFSR, int count)
{
  LHCb::GenFSR::CrossSection::iterator icross;
  LHCb::GenFSR::CrossSection crossSections = genFSR->getCrossSections();  
  std::string name = "";
  double value = 0;
  
  m_htmlFile << "<tr>\n<td align=center>\n<table border=2>\n<th colspan=" << count;
  m_htmlFile << "><div class=firstcell>Hard Generator Cross-Sections</div></th>\n" << std::endl;
  m_htmlFile << "<tr>\n";
  
  // Generator Cross-Sections                                                                                                             
  for(icross = crossSections.begin(); icross != crossSections.end(); icross++)
  {
    name = icross->second.first;    
    value = icross->second.second;
    if(value == 0 || isnan(value) != 0) continue;

    m_htmlFile << "<td><font size=2>" << name.c_str() << " (mb)</font></td>\n";
  }
  m_htmlFile <<"</tr>\n\n<tr>\n";

  // Generator Cross-Sections                                                                                                                  
  for(icross = crossSections.begin(); icross != crossSections.end(); icross++)
  {
    value = icross->second.second;
    if(value == 0 || isnan(value) != 0) continue;

    m_htmlFile << "<td>" << value << "</td>\n"; 
  }
  m_htmlFile << "</tr>\n</table>\n</td>\n</tr>" << std::endl;
}

void GenFSRStat::writeFinalComments(LHCb::GenFSR* genFSR)
{  
  LHCb::GenFSR::GeneratorCounter::iterator icounter; 
  LHCb::GenFSR::GeneratorCounter genCounters = genFSR->getGenCounters();
  int key = 0;
  std::string name = "";
  longlong countEvtGen = 0, countEvtAcc = 0, countIntGen = 0, countIntAcc = 0;
  double fraction_events = 0, fraction_interactions = 0;
  
  for(icounter = genCounters.begin(); icounter != genCounters.end(); icounter++)
  {    
    key = icounter->first; 
    if(key > 5) continue;
    
    if(key == 2) countEvtGen = icounter->second.second;
    if(key == 3) countIntGen = icounter->second.second;
    if(key == 4) countEvtAcc = icounter->second.second;
    if(key == 5) countIntAcc = icounter->second.second;
  }

  fraction_events = genFSR->getEfficiency(countEvtAcc, countEvtGen);
  fraction_interactions = genFSR->getEfficiency(countIntAcc, countIntGen);

  m_htmlFile << "<p> Number of accepted events/generated events: " << countEvtAcc << "/" << countEvtGen;
  m_htmlFile << " = " <<  fraction_events  << "</p>\n";  
  m_htmlFile << "<p> Number of interactions in accepted events/generated interactions: " << countIntAcc;
  m_htmlFile << "/" << countIntGen << " = " << fraction_interactions << "</p>\n";
}

//=============================================================================                                                                
//  Get current date/time, format is YYYY-MM-DD.HH:mm:ss     
//=============================================================================                                                                

// naive function to get the current time
const std::string GenFSRStat::getCurrentTime()
{
  time_t now = time(0);  
  struct tm  tstruct;
  char time[80];
  tstruct = *localtime(&now);
  strftime(time, sizeof(time), "%Y-%m-%d.%X", &tstruct);

  return time;
}

const std::string GenFSRStat::getEvtTypeDesc(int evtType)
{
  std::string path_decFile = gSystem->Getenv("DECFILESROOT");
  std::string path_desc = "", line_ref = "", line_stop = "";
  std::string line = "", description = "";
  std::fstream file_desc;
  std::string evtType_str = dynamic_cast<std::ostringstream*>(&(std::ostringstream() << evtType))->str();
  std::size_t pos_ref, pos_end;
  int count = 0;

  if(path_decFile != NULL)
  {
    path_desc = path_decFile + "/options/" + evtType_str + ".py";
    file_desc.open(path_desc, std::fstream::in);
    line_ref = "$DECFILESROOT/dkfiles/";
    line_stop = ".dec\"";
    
    while(getline(file_desc,line))
    {
      pos_ref = line.find(line_ref);
      
      if(pos_ref != std::string::npos)
      {
        pos_end = line.find(line_stop);
        pos_ref = pos_ref + line_ref.length();
        count = pos_end - pos_ref;
        description = line.substr(pos_ref, count);
        break;
      }
    }

    file_desc.close();
  }
  
  return description;
}
