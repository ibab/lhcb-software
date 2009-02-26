// $Id: STSummary.cpp,v 1.2 2009-02-26 14:43:53 mneedham Exp $
#include "Event/STSummary.h"

// fillstream method
std::ostream& LHCb::STSummary::fillStream(std::ostream& s) const
{

  std::string syncStatus = "OK";
  if (m_pcnSynch == false) syncStatus = "Not" + syncStatus; 
  s << "Summary Block " << std::endl;
  s << "# Clusters " << m_nClusters << " PCN " << m_pcn << " Sync status " << syncStatus;
  s << std::endl;
  s << "# bytes " << m_rawBufferSize << std::endl;
  s << "# full banks " << m_nFullBanks << std::endl;
  s << "# pedestal banks " << m_nPedestalBanks << std::endl;
  s << "# error banks " << m_nErrorBanks << std::endl;
  if (m_missingBanks.empty() == false) {
    // print the missing in banks
    s << "Missing In Action";
    for (std::vector<unsigned int>::const_iterator iterM = m_missingBanks.begin();
         iterM != m_missingBanks.end(); ++iterM){
      s << "Missing: ";
      s << "Bank " << *iterM << std::endl;      
    } // iterM  
  }

  if (m_corruptedBanks.empty() == false) {
    // print the banks in error
    s << "Corrupted banks: ";
    for (std::vector<unsigned int>::const_iterator iterE = m_corruptedBanks.begin();
         iterE != m_corruptedBanks.end(); ++iterE){
      s << "Corrupted: ";
      s << "Bank " << *iterE << std::endl;      
    } // iterM  
  }
 
 
  if (m_recoveredBanks.empty() == false){
    // print the banks that were recovered
    RecoveredInfo::const_iterator iterRec = m_recoveredBanks.begin();
    s << "Recovered: ";
    for (; iterRec != m_recoveredBanks.end(); ++iterRec){
      s << " Bank " << iterRec->first << " fraction " << iterRec->second;
    } // iterRec
  }
  return s;
}
