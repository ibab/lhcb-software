// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistDB.h,v 1.31 2010-08-19 16:12:58 ggiacomo Exp $
#ifndef ONLINEHISTDB_H
#define ONLINEHISTDB_H 1
/** @class  OnlineHistDB OnlineHistDB.h OnlineHistDB/OnlineHistDB.h
 * 
 *  C++ interface to the Online Monitoring Histogram DB:
 *  DB Session class
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 1/12/2006
 */
#include "OnlineHistDB/OnlineHistDBEnv.h"
#include "OnlineHistDB/OnlineHistogram.h"
#include "OnlineHistDB/OnlineHistPage.h"
#include "OnlineHistDB/OnlineHistTask.h"


class  OnlineHistDB : public OnlineHistDBEnv,
  public OnlineTaskStorage, 
  public OnlineHistogramStorage,
  public OnlinePageStorage
{
 public:
  /// constructor
  OnlineHistDB (std::string passwd=OnlineHistDBEnv_constants::PASSWORD, 
		std::string user=OnlineHistDBEnv_constants::ACCOUNT, 
		std::string db=OnlineHistDBEnv_constants::DB);
  virtual ~OnlineHistDB();
  // DB session methods
  /// commits all changes to the DB. Returns true if there are no errors.
  bool commit();
  /// true if user has write access to the DB
  bool canwrite() {return (m_canwrite>0);}

  // declarations of main DB objects
  /// declares a  subsystem, returning true on success
  bool declareSubSystem(std::string SubSys);
  /// declares an Histogram by its DIM service name. In the LHCb
  /// DAQ, this is intended to be used only by the Experiment Control
  /// System to dynamically update the DB with the published histograms.
  /// Tasks not known to the DB are automatically created.
  /// If histogram already exists, just updates the current DIM service name 
  void declareHistByServiceName(const std::string &ServiceName);
  /// declares an Histogram to the DB by its attributes.  The enum HistType is defined in class OnlineHistDBEnv
  void declareHistogram(std::string TaskName,
			std::string AlgorithmName,
			std::string HistogramName,
			HistType Type); 
  /// declares a trending histogram. optional HistoTitle is the title to be displayed, the default is the tag name
  void declareTrendingHistogram(std::string File,
                                std::string tag,
                                std::string HistoTitle="_default_");
  /// declares a virtual histogram to be produced at display or analysis
  /// level using algorithm Algorithm. Name is the histogram name. Sources
  /// and/or InputSet must contain the pointers to the input histograms or set. 
  /// Parameters is a
  /// pointer to a parameter vector, optionally needed by the algorithm. 
  /// Returns the pointer to the new histogram object.
  OnlineHistogram* declareAnalysisHistogram(std::string Algorithm,
                                            std::string Name,
                                            std::vector<OnlineHistogram*> &Sources,
                                            std::vector<float>* Parameters = NULL,
                                            OnlineHistogram* InputSet = NULL);
  /// declares to the DB an Analysis algorithm implemented in the Analysis
  /// library. NoutPars is the number of algorithm's output parameters to be checked, 
  /// while NinPars is the number of optional input parameters.
  /// outPars and inPars should point to arrays containing the parameter names, 
  /// doc is a short description of the algorithm.
  /// (to be used by OMAlib, where algorithm must be implemented)
  bool declareCheckAlgorithm(std::string Name, 
                             int NoutPars, 
                             std::vector<std::string> *outPars = NULL,
                             std::vector<float> *outDefv = NULL,
                             int NinPars = 0, 
                             std::vector<std::string> *inPars =NULL ,
                             std::vector<float> *inDefv = NULL,
                             std::string *doc =NULL);
  /// declares to the DB an available algorithm to produce histograms at
  /// analysis time. Ninput is the number of input histograms, SetAsInput
  /// if input can be an histogramset, Npars the
  /// number of optional parameters (pars containing their names), doc is a short
  /// description of the algorithm.
  /// (to be used by OMAlib, where algorithm must be implemented)
  bool declareCreatorAlgorithm(std::string Name, 
			       int Ninput,
			       bool SetAsInput=false,
			       HistType OutputType = H1D,
			       int Npars=0, 
			       std::vector<std::string> *pars=NULL,
			       std::vector<float> *defv = NULL,
			       std::string *doc=NULL);
  /// declares an available fit function with Npars parameters. parnames must point to a vector
  /// of length Npars, set mustinit=true if parameters must be initialized
  /// (to be used by OMAlib, where function must be implemented)
  bool declareFitFunction(std::string Name, 
                          int Npars,
                          std::vector<std::string> *parnames,
                          bool mustInit,
                          std::string &doc,
                          std::vector<float> *parInitv = NULL,
                          std::vector<std::string> *inPars =NULL ,
                          std::vector<float> *inDefv = NULL );
  std::string getFitFunction(int code);
  /// gets the algorithm list version
  int getAlgListID() const {return m_AlgListID;}
  /// sets the algorithm list version (works only for DB admin account)
  bool setAlgListID(int algListID);
  /// change reference histograms root directory (for testing)
  void setRefRoot(std::string& newroot) { *m_refRoot = newroot; }
  /// change savesets root directory (for testing)
  void setSavesetsRoot(std::string& newroot) { *m_savesetsRoot = newroot; }


  // deleting methods (handle with care!)

  /// removes Page Folder only if it doesn't have pages (useful for cleanup)
  bool removePageFolder(std::string& Folder);
  /// reload all cached objects from the DB
  void refresh();


  // query functions
  /// total number of histograms in the DB
  int nHistograms() {return m_nHistograms;}
  /// total number of pages in the DB
  int nPages() {return m_nPages;}
  ///  total number of page folders in the DB
  int nPageFolders() {return m_nPageFolders;}
  /// gets the full list of histograms. Returns the number of histograms found. Vectors with pointers
  /// to OnlineHistogram objects, histogram identifiers, histogram types can optionally created  by the user
  /// and filled if not null
  int getAllHistograms(std::vector<OnlineHistogram*>* list = NULL,
		       std::vector<std::string>* ids = NULL,
		       std::vector<std::string>* types = NULL);
  /// gets the list of histograms on which some check analysis has to be performed 
  int getHistogramsWithAnalysis(std::vector<OnlineHistogram*>* list = NULL,
				std::vector<std::string>* ids = NULL,
				std::vector<std::string>* types = NULL);
  /// gets the list of histograms that have to be produced by analysis task
  int getAnalysisHistograms(std::vector<OnlineHistogram*>* list = NULL,
			    std::vector<std::string>* ids = NULL,
			    std::vector<std::string>* types = NULL);
  /// gets the list of histograms related to subsystem SubSys
  int getHistogramsBySubsystem(std::string SubSys,
			       std::vector<OnlineHistogram*>* list = NULL,
			       std::vector<std::string>* ids = NULL,
			       std::vector<std::string>* types = NULL);
  /// gets the list of histograms related to task Task
  int getHistogramsByTask(std::string Task,
			  std::vector<OnlineHistogram*>* list = NULL,
			  std::vector<std::string>* ids = NULL,
			  std::vector<std::string>* types = NULL);
  /// gets the list of histograms displayed on page Page
  int getHistogramsByPage(std::string Page,
			  std::vector<OnlineHistogram*>* list = NULL,
			  std::vector<std::string>* ids = NULL,
			  std::vector<std::string>* types = NULL);
  /// gets the list of histograms in a Set
  int getHistogramsBySet(std::string SetName,
			 std::vector<OnlineHistogram*>* list = NULL,
			 std::vector<std::string>* ids = NULL,
			 std::vector<std::string>* types = NULL);
  int getHistogramsBySet(const OnlineHistogram& Set,
			 std::vector<OnlineHistogram*>* list,
			 std::vector<std::string>* ids = NULL,
			 std::vector<std::string>* types = NULL);
  /// gets the list of page folders, Parent can be "/", a page folder name or "_ALL_" for all folders  
  int getPageFolderNames(std::vector<std::string>& list, std::string Parent="_ALL_");
  /// gets the list of pages in a folder
  int getPageNamesByFolder(std::string Folder,
			   std::vector<std::string>& list);
  int getPageNames(std::vector<std::string>& list);
  /// gets the list of known subsystems
  int getSubsystems(std::vector<std::string>& list);
  /// gets the list of tasks
  int getTasks(std::vector<std::string>& list);
  /// gets the list of algorithms, type can be "_ALL_", "CHECK", "HCREATOR"
  int getAlgorithms(std::vector<std::string>& list,std::string type="_ALL_");
  // gets list of stored messages from analysis by their unique identifier
  int getMessages(std::vector<int>& list, std::string AnalysisTask="any");
  // delete all messages from analysis
  bool deleteAllMessages();
  // delete messages older than expTime seconds
  bool deleteOldMessages(int expTime, std::string &anaTask);

  /// gets the list of known detector status conditions that can be checked by automatic analysis, returning their number
  int getConditions(std::vector<int>& bits, 
                     std::vector<std::string>& conditions,
                     std::vector<std::string>& dimservices);

 private:
  // private dummy copy constructor and assignment operator
   OnlineHistDB(const OnlineHistDB&) : OnlineHistDBEnv(),  
     OnlineTaskStorage(), OnlineHistogramStorage(), OnlinePageStorage() {} 
     OnlineHistDB& operator= (const OnlineHistDB&)  {return *this;} 
  int m_canwrite;
  int m_AlgListID;
  int m_nHistograms;
  int m_nPages;
  int m_nPageFolders;
  int genericStringQuery(std::string command,std::vector<std::string>& list);
  int getHistograms(std::string query,
		    std::vector<OnlineHistogram*>* list=NULL,
		    std::vector<std::string>* ids = NULL,
		    std::vector<std::string>* types = NULL); 
};

#endif // ONLINEHISTDB_Hg
