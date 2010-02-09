#ifndef INSERTDB_INSERTDB_H
#define INSERTDB_INSERTDB_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

class InsertDB : public GaudiHistoAlg {
public:
   InsertDB(const std::string& name, ISvcLocator* pSvcLocator);
  
  /// Three mandatory member functions of any algorithm
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  void doEFF();
  void doEFFHltExperts();
  void doMF(std::string monitoringtask);
  void HistogramDisplayOptions(OnlineHistogram* h);

  
private: 
   char *service;
   char *Dimformat;
   int type;
   int icount;
   int mtaskcount;
   
   std::string hSvcname;
   std::string hSvcname2d;
   std::string hSvcnames;
   std::string hSvcnames2d;
   std::string pSvcname;
   std::string pSvcnames;
   std::string rSvcname;
   std::string rSvcnames;
   std::string commentSvcnames;
   std::string commentSvcname;
   std::string taskname;
   std::string algorithmname;
   std::string histogramname;
   std::string m_partitionname;
   std::string m_nodename;
   std::string m_taskname;
   std::string m_addername;
   std::string m_dimclientdns;
   std::string m_nickname;
   std::string m_hltdbfolder;
   std::vector<std::string>  m_monitoringtasks;   
   std::vector<std::string> m_alleys;   
   std::vector<std::string> m_alleysearchstring;  
   std::vector<std::string> m_DMalleys;   
   std::vector<std::string> m_DMalleysearchstring; 
   std::vector<std::vector<std::string> > m_subfolders;
   bool m_mfonly;
   bool m_effonly;
   
};


#endif    // INSERTDB_INSERTDB_H
