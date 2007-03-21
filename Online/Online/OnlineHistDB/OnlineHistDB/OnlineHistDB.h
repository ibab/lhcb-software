// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistDB.h,v 1.4 2007-03-21 13:15:14 ggiacomo Exp $
#ifndef ONLINEHISTDB_H
#define ONLINEHISTDB_H 1
/** @class  OnlineHistDB OnlineHistDB.h OnlineHistDB/OnlineHistDB.h
 * 
 *  header for the C++ interface to the Online Monitoring Histogram DB
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 1/12/2006
 */
#include "OnlineHistDB/OnlineHistDBEnv.h"
#include "OnlineHistDB/OnlineHistogram.h"
#include "OnlineHistDB/OnlineHistPage.h"



class  OnlineHistDB : public OnlineHistDBEnv
{
 public:
  OnlineHistDB (std::string passwd, 
		std::string user=OnlineHistDBEnv_constants::ACCOUNT, 
		std::string db=OnlineHistDBEnv_constants::DB);
  virtual ~OnlineHistDB();
  // DB session methods
  bool commit();
  void setHistogramBufferDepth(int N);
  
  // declarations of main DB objects
  
  /// create or update a Task definition
  bool declareTask(std::string Name, 
		   std::string SubDet1="NULL", 
		   std::string SubDet2="NULL", 
		   std::string SubDet3="NULL",
		   bool RunsOnPhysics=false, 
		   bool RunsOnCalib=false, 
		   bool RunsOnEmpty=false);
  /// declare a  subsystem
  bool declareSubSystem(std::string SubSys);
  /// declare an Histogram to the DB by its DIM service name
  void declareHistByServiceName(const std::string &ServiceName);
  /// declare an Histogram to the DB by its attributes
  void declareHistogram(std::string TaskName,
			std::string AlgorithmName,
			std::string HistogramName,
			HistType Type);
  /// force sending the histogram declaration buffer to the DB, returns true if there are no errors
  bool sendHistBuffer();
  /// declare an Histogram to be produced at analysis level
  OnlineHistogram* declareAnalysisHistogram(std::string Algorithm,
					    std::string Name,
					    std::vector<OnlineHistogram*> &Sources,
					    std::vector<float>* Parameters = NULL);
  /// remove an histogram (TEMPORARY METHOD TO BE REMOVED AT PRODUCTION STAGE)
  bool removeHistogram(OnlineHistogram* h,
		       bool RemoveWholeSet = false);		 
  /// declare to the DB an Analysis algorithm implemented in the Analysis library
  bool declareCheckAlgorithm(std::string Name, 
			     int Npars, 
			     std::string* pars=NULL, 
			     std::string doc="NONE");
  /// declare to the DB an available algorithm to produce histograms at analysis time
  bool declareCreatorAlgorithm(std::string Name, 
			       int Ninput=0, 
			       HistType OutputType = H1D,
			       int Npars=0, 
			       std::string* pars=NULL,
			       std::string doc="NONE");
  // access methods
  /// get an OnlineHistPage object, to create a new page or view/edit an existing one
  OnlineHistPage* getPage(std::string Name, std::string Folder="");
  /// get an OnlineHistogram object, holding informations of an existing histogram, that can be used to view/edit an histogram record
  OnlineHistogram* getHistogram(std::string Identifier,
				std::string Page="_NONE_",
				int Instance = 1);
  /// get number of parameters needed by algorithm AlgName (and optionally the number of input histograms)
  int getAlgorithmNpar(std::string AlgName,
		       int* Ninput = NULL) const;
  /// get the name of parameter Ipar of algorithm AlgName
  std::string getAlgParName(std::string AlgName,
		       int Ipar) const;


  // query functions
  /// get the list of histograms on which some check analysis has to be performed 
  int getHistogramsWithAnalysis(std::vector<OnlineHistogram*>& list);
  /// get the list of histograms that have to be produced by analysis task
  int getAnalysisHistograms(std::vector<OnlineHistogram*>& list);
  /// get the list of histograms related to subsystem SubSys
  int getHistogramsBySubsystem(std::string SubSys,std::vector<OnlineHistogram*>& list);
  /// get the list of histograms related to task Task
  int getHistogramsByTask(std::string Task,std::vector<OnlineHistogram*>& list);
  /// get the list of histograms displayed on page Page
  int getHistogramsByPage(std::string Page,std::vector<OnlineHistogram*>& list);
  /// get the list of histograms in a Set
  int getHistogramsBySet(std::string SetName,std::vector<OnlineHistogram*>& list);
  int getHistogramsBySet(const OnlineHistogram& Set,std::vector<OnlineHistogram*>& list);
  /// get the list of page folders, Parent can be "ROOT", a page folder name or "_ALL_" for all folders  
  int getPageFolderNames(std::vector<string>& list, std::string Parent="_ALL_");
  /// get the list of pages in a folder
  int getPageNamesByFolder(std::string Folder,
			   std::vector<string>& list);
  int getPageNames(std::vector<string>& list);
  /// get the list of known subsystems
  int getSubsystems(std::vector<string>& list);
  /// get the list of tasks
  int getTasks(std::vector<string>& list);
  /// get the list of algorithms, type can be "_ALL_", "CHECK", "HCREATOR"
  int getAlgorithms(std::vector<string>& list,std::string type="_ALL_");

 private:
  // private dummy copy constructor and assignment operator
  OnlineHistDB(const OnlineHistDB&) : OnlineHistDBEnv("dummy") {}
  OnlineHistDB& operator= (const OnlineHistDB&)  {return *this;}
  int m_DBschema;
  Statement *m_stmt;
  int m_nit;
  int m_maxNit;
  std::vector<OnlineHistPage*> m_myPage;
  std::vector<OnlineHistogram*> m_myHist;
  int genericStringQuery(std::string command,std::vector<string>& list);
  int getHistograms(std::string query,std::vector<OnlineHistogram*>& list);
};

#endif // ONLINEHISTDB_H
