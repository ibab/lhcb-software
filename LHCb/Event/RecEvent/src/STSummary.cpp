// $Id: STSummary.cpp,v 1.1 2009-01-07 07:54:38 mneedham Exp $
#include "Event/STSummary.h"

// fillstream method
std::ostream& LHCb::STSummary::fillStream(std::ostream& s) const
{

  std::string syncStatus = "OK";
  if (m_pcnSynch == false) syncStatus = "Not" + syncStatus; 
  s << "# Clusters " << m_nClusters << " PCN " << m_pcn << " Sync status " << syncStatus;
  s << std::endl;
  if (m_missingBanks.empty() == false) {
    // print the missing in banks
    s << "Missing In Action";
    for (std::vector<unsigned int>::const_iterator iterM = m_missingBanks.begin();
         iterM != m_missingBanks.end(); ++iterM){
      s << "Missing: ";
      s << std::endl;
      s << "Bank " << *iterM << std::endl;      
    } // iterM  
  }

  if (m_banksWithError.empty() == false) {
    // print the banks in error
    s << "Banks in error";
    for (std::vector<unsigned int>::const_iterator iterE = m_banksWithError.begin();
         iterE != m_banksWithError.end(); ++iterE){
      s << "Missing: ";
      s << std::endl;
      s << "Bank " << *iterE << std::endl;      
    } // iterM  
  }
 
  s << std::endl;
  if (m_recoveredBanks.empty() == false){
    // print the banks that were recovered
    RecoveredInfo::const_iterator iterRec = m_recoveredBanks.begin();
    s << "Recovered: ";
    for (; iterRec != m_recoveredBanks.end(); ++iterRec){
      s << std::endl;
      s << " Bank " << iterRec->first << " fraction " << iterRec->second;
    } // iterRec
  }
  return s;
}
