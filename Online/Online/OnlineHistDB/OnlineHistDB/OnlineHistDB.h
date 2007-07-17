// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistDB.h,v 1.10 2007-07-17 15:54:12 ggiacomo Exp $
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
#include "OnlineHistDB/OnlineRootHist.h"
#include "OnlineHistDB/OnlineHistPage.h"
#include "OnlineHistDB/OnlineHistTask.h"


class  OnlineHistDB : public OnlineHistDBEnv, 
    public OnlineTaskStorage, public OnlineHistogramStorage, 
    public OnlinePageStorage, public OnlineRootHistStorage
{
 public:
  /// constructor
  OnlineHistDB (std::string passwd, 
		std::string user=OnlineHistDBEnv_constants::ACCOUNT, 
		std::string db=OnlineHistDBEnv_constants::DB);
  virtual ~OnlineHistDB();
  // DB session methods
  /// commits all changes to the DB. Returns true if there are no errors.
  bool commit();
  /// when creating histograms with the {\it declareHistByServiceName}
  /// method, the histogram list is actually send to the DB server every N
  /// histograms (or at commit) in order to optimize performance. The
  /// default buffer depth (recommended value) is 1000. 
  void setHistogramBufferDepth(int N);
  
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
  /// forces sending histogram declaration buffer to the DB. Returns true if there are no errors.
  bool sendHistBuffer();
  /// declares an histogram to be produced at analysis
  /// level using algorithm Algorithm. Name is the histogram name. Sources
  /// must contain the pointers to the input histograms. Parameters is a
  /// pointer to a parameter vector, optionally needed by the algorithm. If
  /// the algorithm requires an histogram set as input, use any histogram of the
  /// set. Returns the pointer to the new histogram object.
  OnlineHistogram* declareAnalysisHistogram(std::string Algorithm,
					    std::string Name,
					    std::vector<OnlineHistogram*> &Sources,
					    std::vector<float>* Parameters = NULL);
  /// declares to the DB an Analysis algorithm implemented in the Analysis
  /// library. Npars is the number of algorithm's parameters, pars should
  /// point to an array containing the parameter names, doc is a short
  /// description of the algorithm.
  bool declareCheckAlgorithm(std::string Name, 
			     int Npars, 
			     std::string* pars=NULL, 
			     std::string doc="NONE");
  /// declares to the DB an available algorithm to produce histograms at
  /// analysis time. Ninput is the number of input histograms, Npars the
  /// number of optional parameters (pars containing their names), doc is a short
  /// description of the algorithm.
  bool declareCreatorAlgorithm(std::string Name, 
			       int Ninput=0, 
			       HistType OutputType = H1D,
			       int Npars=0, 
			       std::string* pars=NULL,
			       std::string doc="NONE");

  // deleting methods (handle with care!)

  /// removes an histogram, and optionally its full set. 
  /// ({\bf TEMPORARY METHOD TO BE REMOVED AT PRODUCTION STAGE})
  virtual bool removeHistogram(OnlineHistogram* h,
			       bool RemoveWholeSet = false);	
  /// removes Page Folder only if it doesn't have pages (useful for cleanup)
  bool removePageFolder(std::string Folder);

  // access methods
  /// gets the number of parameters, and optionally the number of input histograms, needed by algorithm AlgName.
  int getAlgorithmNpar(std::string AlgName,
		       int* Ninput = NULL) const;
  /// gets the name of parameter Ipar (starting from 1) of algorithm AlgName
  std::string getAlgParName(std::string AlgName,
		       int Ipar) const;


  // query functions
  /// gets the full list of histograms. Returns the number of histograms found. Vectors with pointers
  /// to OnlineHistogram objects, histogram identifiers, histogram types can optionally created  by the user
  /// and filled if not null. Set useRootHist = true to get OnlineRootHist objects.
  int getAllHistograms(std::vector<OnlineHistogram*>* list = NULL,
		       std::vector<string>* ids = NULL,
		       std::vector<string>* types = NULL,
		       bool useRootHist = false);
  /// gets the list of histograms on which some check analysis has to be performed 
  int getHistogramsWithAnalysis(std::vector<OnlineHistogram*>* list = NULL,
				std::vector<string>* ids = NULL,
				std::vector<string>* types = NULL,
				bool useRootHist = false);
  /// gets the list of histograms that have to be produced by analysis task
  int getAnalysisHistograms(std::vector<OnlineHistogram*>* list = NULL,
			    std::vector<string>* ids = NULL,
			    std::vector<string>* types = NULL,
			    bool useRootHist = false);
  /// gets the list of histograms related to subsystem SubSys
  int getHistogramsBySubsystem(std::string SubSys,
			       std::vector<OnlineHistogram*>* list = NULL,
			       std::vector<string>* ids = NULL,
			       std::vector<string>* types = NULL,
			       bool useRootHist = false);
  /// gets the list of histograms related to task Task
  int getHistogramsByTask(std::string Task,
			  std::vector<OnlineHistogram*>* list = NULL,
			  std::vector<string>* ids = NULL,
			  std::vector<string>* types = NULL,
			  bool useRootHist = false);
  /// gets the list of histograms displayed on page Page
  int getHistogramsByPage(std::string Page,
			  std::vector<OnlineHistogram*>* list = NULL,
			  std::vector<string>* ids = NULL,
			  std::vector<string>* types = NULL,
			  bool useRootHist = false);
  /// gets the list of histograms in a Set
  int getHistogramsBySet(std::string SetName,
			 std::vector<OnlineHistogram*>* list = NULL,
			 std::vector<string>* ids = NULL,
			 std::vector<string>* types = NULL,
			 bool useRootHist = false);
  int getHistogramsBySet(const OnlineHistogram& Set,
			 std::vector<OnlineHistogram*>* list,
			 std::vector<string>* ids = NULL,
			 std::vector<string>* types = NULL,
			 bool useRootHist = false);
  /// gets the list of page folders, Parent can be "ROOT", a page folder name or "_ALL_" for all folders  
  int getPageFolderNames(std::vector<string>& list, std::string Parent="_ALL_");
  /// gets the list of pages in a folder
  int getPageNamesByFolder(std::string Folder,
			   std::vector<string>& list);
  int getPageNames(std::vector<string>& list);
  /// gets the list of known subsystems
  int getSubsystems(std::vector<string>& list);
  /// gets the list of tasks
  int getTasks(std::vector<string>& list);
  /// gets the list of algorithms, type can be "_ALL_", "CHECK", "HCREATOR"
  int getAlgorithms(std::vector<string>& list,std::string type="_ALL_");

 private:
  // private dummy copy constructor and assignment operator
  OnlineHistDB(const OnlineHistDB&) : OnlineHistDBEnv("dummy"), 
    OnlineTaskStorage(this), OnlineHistogramStorage(this), OnlinePageStorage(this,this),
    OnlineRootHistStorage(this) {}
  OnlineHistDB& operator= (const OnlineHistDB&)  {return *this;}
  int m_DBschema;
  Statement *m_stmt;
  int m_nit;
  int m_maxNit;
  int genericStringQuery(std::string command,std::vector<string>& list);
  int getHistograms(std::string query,
		    std::vector<OnlineHistogram*>* list=NULL,
		    std::vector<string>* ids = NULL,
		    std::vector<string>* types = NULL,
		    bool useRootHist = false);
};

#endif // ONLINEHISTDB_H
