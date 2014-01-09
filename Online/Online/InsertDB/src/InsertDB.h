#ifndef INSERTDB_INSERTDB_H
#define INSERTDB_INSERTDB_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

namespace HistoComp    {

  bool isLess2(OnlineHistogram* h1,OnlineHistogram* h2)
  {
    //    const char* h1name = h1->identifier();
    //const char* h2name = h2->identifier();
    return (std::strcmp(h1->identifier().c_str(),h2->identifier().c_str()) <0);
  }
}

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
  void doMFNew(std::string monitoringtask);

  void fillPages(std::string pageNamePrefix,
		 std::map <std::string, std::vector<std::string> > pages,
		 std::vector<std::string> documentation, 
		 bool fromEFF,
		 std::string taskName = "GauchoJob" );

 void fillLinePages(std::string pageNamePrefix,
		 std::map <std::string, std::vector<std::string> > pages,
		 std::vector<std::string> documentation );


  void placeHistograms(OnlineHistPage* page, 
		       std::vector<OnlineHistogram*>& histos, 
		       std::string doc );
  void doEFFNew();

  void getHistsFromDB(OnlineHistDB *HistDB,
		      std::map <std::string, std::vector<std::string> >& lines,
		      bool fromEFF,
		      std::string monitoringtask = "");

  std::string ReplaceString(const std::string &stringSearchString, 
			    const std::string &stringReplaceString, 
			    std::string stringStringToReplace);
    
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

   std::map <std::string, std::vector<std::string> > m_globalPages;
   std::vector<std::string> m_globalDocu;
   std::map <std::string, std::vector<std::string> > m_linePages;
   std::vector<std::string> m_lineDocu;
   std::map <std::string, std::vector<std::string> > m_hlt1AlgMuPrepPages;
   std::vector<std::string> m_hlt1AlgMuPrepDocu;
   std::string m_hlt1AlgMonName;

   std::map <std::string, std::vector<std::string> > m_lumiPages;
   std::vector<std::string> m_lumiDocu;
   std::map <std::string, std::vector<std::string> > m_lumiLowPages;
   std::vector<std::string> m_lumiLowDocu;
   
  
   bool m_mfonly;
   bool m_effonly;

   bool m_doGlobalPages;
   bool m_doLinePages;
   bool m_doAlgorithmPages;
   bool m_doLumiPages;
   bool m_doExpertPages;
   
};


#endif    // INSERTDB_INSERTDB_H
