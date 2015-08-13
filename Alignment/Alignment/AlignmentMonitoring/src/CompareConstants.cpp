// AlignmentMonitoring
#include "AlignmentMonitoring/CompareConstants.h"
#include "AlignmentMonitoring/ParseConstants.h"
#include "AlignmentMonitoring/Utilities.h"
// STL
#include <algorithm>
#include <iostream>
// Boost
#include "boost/regex.hpp"
// ROOT
#include "TFile.h"

using namespace Alignment::AlignmentMonitoring;

// Constructors
CompareConstants::CompareConstants()
  : CompareConstants("","","")
{
}

CompareConstants::CompareConstants(std::map<std::string,double> iVals, std::map<std::string,double> fVals)
  : CompareConstants("","","")
{
  m_constRef = iVals;
  m_constAlt = fVals;
}

CompareConstants::CompareConstants(const char* system, const char* ref, const char* dir)
: m_verbose(false)
, m_system(system)
, m_ref(ref)
, m_dir(dir)
{
  m_dofs.push_back("Tx");
  m_dofs.push_back("Ty");
  m_dofs.push_back("Tz");
  m_dofs.push_back("Rx");
  m_dofs.push_back("Ry");
  m_dofs.push_back("Rz");
  SetRanges();
}

// Methods
void
CompareConstants::Compare()
{
  if (m_verbose) std::cout  << "nElements: " << m_constRef.size() << " " << m_constAlt.size() << std::endl;
  if (m_constAlt.size() != m_constRef.size()) {
    std::cout << "Beware, the number of elements for the two maps is different!\n";
    return;
  }
  for (auto iel : m_constRef) {
    m_mapWarnings[iel.first] = CheckConstant(iel.first.c_str(), iel.second - m_constAlt[iel.first]);
    if (m_verbose)
      std::cout << iel.first << ": "
                << iel.second << " - " << m_constAlt[iel.first] << " = " << iel.second - m_constAlt[iel.first]
                << " : " << m_mapWarnings[iel.first]
                << std::endl;
    FillDeltaHist(iel.first.c_str());
  }
  return;
}

void
CompareConstants::Compare(const char* ver)
{
  std::map<std::string, WarningLevel> wls;
  ParseConstants parser;
  m_constRef = parser.Parse((m_dir+"/"+m_system+"/"+m_ref+".xml").c_str());
  m_constAlt = parser.Parse((m_dir+"/"+m_system+"/"+std::string(ver)+".xml").c_str());
  Compare();
  return;
}

void CompareConstants::CompareWithRun(const char* dir, const char* runNo){
  std::map<std::string, WarningLevel> wls;
  ParseConstants parser;
  m_constRef = parser.Parse((m_dir+"/"+m_system+"/"+m_ref+".xml").c_str());
  //std::cout << "reference path: " << m_dir+"/"+m_system+"/"+m_ref+".xml"  << std::endl;
  m_constAlt = parser.Parse((std::string(dir)+"/"+std::string(runNo)+"/xml/"+m_system+".xml").c_str());
  //std::cout << "new path: " << std::string(dir)+"/"+std::string(runNo)+"/xml/"+m_system+".xml" << std::endl;
  if (m_verbose) std::cout  << "nElements: " << m_constRef.size() << " " << m_constAlt.size() << std::endl;
  if (m_constAlt.size() != m_constRef.size()) {
    std::cout << "Beware, the number of elements for the two files\n"
              << "\t" << m_dir+"/"+m_system+"/"+m_ref+".xml" << "\n"
              << "\t" << std::string(dir)+"/"+std::string(runNo)+"/xml/"+m_system+".xml" << "\n"
              << "is not the same. Please check your options!\n";
    return;
  }
  for (auto iel : m_constRef) {
    std::vector<double> limits = GetLimits(iel.first.c_str());
    m_mapWarnings[iel.first] = CheckConstant(iel.second - m_constAlt[iel.first], limits[0], limits[1]);
    if (m_verbose)
      std::cout << iel.first << ": "
                << iel.second << " - " << m_constAlt[iel.first] << " = " << iel.second - m_constAlt[iel.first]
                << " : " << m_mapWarnings[iel.first]
                << std::endl;
    FillDeltaHist(iel.first.c_str());
  }
  return;
}

WarningLevel
CompareConstants::CheckConstant(const char* name, int dof, double delta)
{
  return CheckConstant(name, m_dofs[dof].c_str(), delta);
}

WarningLevel
CompareConstants::CheckConstant(const char* name, const char* dof, double delta)
{
  return CheckConstant( ( std::string(name)+std::string(".")+std::string(dof) ).c_str(), delta );
}

WarningLevel
CompareConstants::CheckConstant(const char* name, double delta)
{
  std::vector<double> limits = GetLimits( name );
  WarningLevel wnl = CheckConstant(delta, limits[0], limits[1]);
  return wnl;
}

void
CompareConstants::PrintWarnings(int level)
{
  if !CheckWarningLevel(level) return;
  
  std::string warning_string = "OK";
  for (auto wrn : m_mapWarnings) {
    //if (!m_verbose && wrn.second == WarningLevel::OK) continue;
    if (wrn.second < level) continue;
    if(wrn.second==WarningLevel::OK)      warning_string = "\033[1;32m OK \033[0m";
    if(wrn.second==WarningLevel::WARNING) warning_string = "\033[1;33m WARNING \033[0m";
    if(wrn.second==WarningLevel::SEVERE)  warning_string = "\033[1;31m SEVERE \033[0m";
    std::cout << wrn.first << ": " << warning_string 
              << "\t(" << m_constAlt[wrn.first] << " - " << m_constRef[wrn.first]  << " = " << m_constAlt[wrn.first]-m_constRef[wrn.first] << ")"
              << std::endl;
  }
  return;
}

WarningLevel
CompareConstants::CheckConstant(double delta, double d1, double d2)
{
  if (m_verbose) std::cout << delta << "\t["<< d1 << "," <<d2<<"]:";
  WarningLevel wl = WarningLevel::SEVERE; // default=SEVERE
  if (delta < d1) wl = WarningLevel::OK; // OK within range
  else if (delta < d2) wl = WarningLevel::WARNING; // WARNING within double range
  if (m_verbose) std::cout << "\t" << wl << std::endl;
  return wl;
}

std::vector<double>
CompareConstants::GetLimits(const char* element)
{
  std::vector<double> limits;
  const std::string s(element);
  for (auto rng : m_rngs) {
    /* debug
    if (std::string(element).find("odule1B.")!= std::string::npos)
      std::cout << element << " " << rng.first << ": "
                << (std::string(element).find(rng.first.c_str()) != std::string::npos) << " "
                << (std::string(element).find("Module1B.") != std::string::npos) << " "
                << rng.first.compare("Module1B.") << std::endl;
     */
    //if (std::string(element).find(rng.first) != std::string::npos) { // needs to be updated with regular expressions (regex) // Old
    boost::regex rgx(rng.first);
    if (boost::regex_search(s.begin(), s.end(), rgx)) {
      limits = rng.second;
      if (m_verbose) std::cout<< "element: "<< element << " \tfound: " << rng.first << " (" << rng.second[0] << "," << rng.second[0] << ")\n";
      break;
    }
     
  }
  if (limits.size() < 1) {
    std::cout << "Limits for element " << element << " not found. Returning vector with 0's.";
    limits = {0,0};
  }
  return limits;
}

void
CompareConstants::FillDeltaHist(const char* element)
{
  Utilities ut;
  const std::string s(element);
  for (auto rng : m_rngs) {
    boost::regex rgx(rng.first);
    if (boost::regex_search(s.begin(), s.end(), rgx)) {
      double delta = m_constRef[element] - m_constAlt[element];
      if (m_verbose) std::cout<< "element: "<< element << " \tfound: " << rng.first << "\tdelta = " << delta << std::endl;
      std::string hname(rng.first);
      hname = ut.find_and_replace(hname, ".*", "");
      hname = ut.find_and_replace(hname, "\\.", "");
      if ( m_hists.find(hname) == m_hists.end() ) m_hists[hname] = new TH1D(hname.c_str(), hname.c_str(), 20, -rng.second[1], rng.second[1]);
      m_hists[hname]->Fill(delta);
      break;
    }
  }
  return;
}

void
CompareConstants::SaveHists(const char* filename)
{
  TFile f(filename,"RECREATE");
  f.cd();
  for (auto his : m_hists)
    his.second->Write();
  f.Close();
}

void
CompareConstants::SetRanges(const char* filename)
{
  Utilities ut;
  std::string line;
  std::ifstream ifile( std::string(filename).size() == 0 ? (std::getenv("ALIGNMENTMONITORINGROOT")+std::string("/files/ConstantsReferences.txt")).c_str() : filename);
  if (ifile.is_open()) {
    while ( std::getline (ifile,line) ) {
      std::vector<std::string> refs = ut.splitString(line,"\t");
      if (refs.size() < 2 ||
          refs[0].find("#") != std::string::npos) continue;
      m_rngs[refs[0]] = {std::stod(refs[1]),std::stod(refs[2])};
    }
    ifile.close();
  }
  return;
}

bool
CompareConstants::CheckWarningLevel(int level)
{
  if (level< 0 || level > 2) {
    std::cout << "You selected a warning level that is not valid (0-2)!\n";
    return false;
  }
  return true;
}

int
CompareConstants::GetNumWarnings(int level)
{
  if !CheckWarningLevel(level) return;
  // get number of warnings of this level
  int nw(0);
  for (auto wrn : m_mapWarnings)
    if (wrn.second != level) ++nw;
  return nw;
}
