// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistogram.h,v 1.13 2007-10-26 13:46:43 ggiacomo Exp $
#ifndef ONLINEHISTOGRAM_H
#define ONLINEHISTOGRAM_H 1
/** @class  OnlineHistogram OnlineHistogram.h OnlineHistDB/OnlineHistogram.h
 * 
 *  Histogram class header for the C++ interface to the Online Monitoring Histogram DB
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 1/12/2006
 */

#include "OnlineHistDB/OnlineHistDBEnv.h"

class  OnlineHistogram : public OnlineHistDBEnv
{
 public:
  OnlineHistogram(OnlineHistDBEnv &env,
		  std::string Identifier,
		  std::string FullPathPageName="_NONE_",
		  int Instance=1);
  virtual ~OnlineHistogram();
  void checkServiceName();
  bool isAbort() const {return m_isAbort;}
  /// full histogram unique identifier Taskname/AlgorithmName/HistogramName
  std::string identifier() const {return m_identifier;}
  /// full path name of the page to which this histogram object is attached
  std::string page() const {return m_page;}
  /// counter (starting from 1) to distinguish several instances of the same histogram on the same page
  int instance() const {return m_instance;}
  /// name of the DIM service that is currently publishing the histogram
  std::string dimServiceName() const  {return m_dimServiceName;}
  /// internal histogram set ID
  int hsid() const {return m_hsid;}
  /// position of this histogram in set (starting from 1). Can be larger than nhs() if some histogram in the set has been deleted
  int ihs() const {return m_ihs;}
  /// internal histogram ID (equivalent to hsid()/ihs())
  std::string hid() const {return m_hid;}
  /// number of histograms in set
  int nhs() const {return m_nhs;}
  /// histogram type (``H1D'', ``H2D'', ``P1D'',``P2D'' or ``CNT'' )
  std::string hstype() const {return m_hstype;}
  /// histogram name
  std::string hname() const {return (m_subname == "" && m_nhs == 1) ? 
      m_hsname : m_hsname + OnlineHistDBEnv_constants::m_SetSeparator + m_subname;}
  /// standard DB histogram title (not necessarly equal to the published ROOT title)
  std::string htitle() const {return (m_subname == "" && m_nhs == 1) ? 
      m_hsname : m_hsname + " " + m_subname;} 
  /// histogram set name
  std::string hsname() const {return m_hsname;}
  /// subname
  std::string subname() const {return m_subname;}
  /// task name
  std::string task() const {return m_task;}
  /// algorithm name
  std::string algorithm() const {return m_algorithm;}  
  /// short description of the histogram
  std::string descr() const {return m_Descr;}
  /// link to a more extensive documentation
  std::string doc() const {return m_Doc;}
  /// creation date, as a unix timestamp
  int creation() const {return m_creation;}
  /// if the histogram is no more in use, returns the end--of--validity date
  /// as a unix timestamp, otherwise returns 0.
  int obsoleteness() const {return m_obsoleteness;}
  /// sets page on which histogram is displayed (reload display options if needed). Histogram has to be already
  /// been attached to the page through OnlineHistPage::declareHistogram()
  bool setPage(std::string FullPathPageName,
	       int Instance=1);
  /// unsets page associated to histogram object
  void unsetPage();
  /// sets the DIM service name that is currently publishing the histogram. Returns true on success
  bool setDimServiceName(std::string DimServiceName);
  /// provide a short description to be optionally printed on the plot 
  bool setDoc(std::string doc);
  /// provide a  description of the histogram content 
  bool setDescr(std::string descr);
  bool remove(bool RemoveWholeSet);
  /// dumps histogram data
  void dump();
  /// dumps histogram display options
  void dumpDisplayOptions();
  /// number of instances of this histogram on any page
  int nPageInstances();
  /// number of instances of this histogram on its current page
  int nThisPageInstances();

  // DISPLAY OPTIONS
  /// specifies if the display options in this object are: not defined,
  /// associated to the histogram set, associated to the histogram,
  /// associated to the histogram on  page {\it page()}
  typedef enum { NONE, SET, HIST, HISTPAGE } DisplayOptionMode;
  DisplayOptionMode domode() const {return m_domode;}
  /// sets a display option.  The available parameter names and the corresponding types are listed 
  /// in section \ref{dispopts}. Returns true on success.
  /// This method chooses the most appropriate display option mode calling one of the next methods  
  virtual bool setDisplayOption(std::string ParameterName, 
				void* value);
  /// sets a display option for the whole histogram set
  virtual bool setHistoSetDisplayOption(std::string ParameterName, 
					void* value);
  /// sets a display option for the present histogram.
  virtual bool setHistDisplayOption(std::string ParameterName, 
				void* value);
  /// sets a display option for the present histogram on page FullPathPageName (default is {\it page()})
  virtual bool setHistoPageDisplayOption(std::string ParameterName, 
					 void* value,
					 std::string FullPathPageName = "_DEFAULT_",
					 int Instance=-1);
  /// unsets a display option in the current display mode
  virtual bool unsetDisplayOption(std::string ParameterName);
  /// unsets a display option for the whole histogram set
  virtual bool unsetHistoSetDisplayOption(std::string ParameterName); 
  /// unsets a display option for the present histogram.
  virtual bool unsetHistDisplayOption(std::string ParameterName); 
  /// unsets a display option for the present histogram on its page (if defined)
  virtual bool unsetHistoPageDisplayOption(std::string ParameterName);


  /// if display option  ParameterName has been defined, puts its value into
  /// option and returns true. In case different sets of display options
  /// exists for histogram set, histogram and histogram in page, the most
  /// specific one is used.
  bool getDisplayOption(std::string ParameterName,
			void* option);

  /// initializes display options associated to this histogram with the
  /// options defined for the histogram set (if available). Returns true on
  /// success.
  bool initHistDisplayOptionsFromSet(); 
  /// initializes display options associated to this histogram on page
  /// FullPathPageName (default is {\it page()}) with the
  /// options defined for the histogram set (if available). Returns true on
  /// success.
  bool initHistoPageDisplayOptionsFromSet(std::string FullPathPageName = "_DEFAULT_",
					  int Instance=-1);
  /// initializes display options associated to this histogram on page
  /// FullPathPageName (default is {\it page()}) with the
  /// options defined for the histogram (if available). Returns true on
  /// success.
  bool initHistoPageDisplayOptionsFromHist(std::string FullPathPageName = "_DEFAULT_",
					   int Instance=-1); 

  // ANALYSIS OPTIONS
  /// for analysis histogram, get the directions for creating histogram
  bool getCreationDirections(std::string &Algorithm,
			     std::vector<std::string> &source_list,
			     std::vector<float> &parameters);
  /// number of analysis to be performed on the histogram set
  int nanalysis() const {return m_nanalysis;}
  /// get analysy description as vectors of length {\it  nanalysis}() containing 
  /// the analysis internal IDs and the analysis algorithm names
  void getAnalyses(std::vector<int>& anaIDs,
		   std::vector<std::string>& anaAlgs) const {
    anaIDs.clear();    
    anaAlgs.clear();    
    anaIDs.insert(anaIDs.begin(),m_anaId.begin(),m_anaId.end());
    anaAlgs.insert(anaAlgs.begin(),m_anaName.begin(),m_anaName.end());
  }
  /// true if the histogram is produced at analysis level
  boolean isAnaHist() const {return m_isAnaHist;}
  /// declare an analysis to be performed on the histogram set. If the algorithm
  /// requires some parameters, the warning and alarm values must be
  /// specified as vectors of floats and will be set for all histograms in
  /// set (then, you can specify values for single histograms with the {\it
  /// setAnalysis} method. 
  /// You can create more than one analysis
  /// with the same algorithm by using instance > 1. If the analysis
  /// identified by Algorithm and instance already exists, parameters are
  /// updated. Returns the internal analysis ID.
  int declareAnalysis(std::string Algorithm, 
		      std::vector<float>* warningThr=NULL, 
		      std::vector<float>* alarmThr=NULL, 
		      int instance=1); 
  /// updates parameters for analysis with ID AnaID (for this histogram only). Returns true on success
  bool setAnalysis(int AnaID, 
		   std::vector<float>* warningThr=NULL, 
		   std::vector<float>* alarmThr=NULL);
  /// gets parameters for analysis with ID AnaID. Returns true on success
  bool getAnaSettings(int AnaID,
		      std::vector<float>* warn, 
		      std::vector<float>* alarm) const; 
  /// masks analysis with ID AnaID. Use Mask=false to unmask. Returns true on success
  bool maskAnalysis(int AnaID,
		    bool Mask=true); 


 protected:
  std::string CreationAlgorithm;
  std::vector<std::string> Sources;
  int SourceSet;
  std::vector<float> SourcePar;

 private:
  // private dummy copy constructor and assignment operator 
  OnlineHistogram(const OnlineHistogram&) : OnlineHistDBEnv("dummy") {}
  OnlineHistogram & operator= (const OnlineHistogram&) {return *this;}
  // private OnlineDisplayOption class
  class OnlineDisplayOption {
  public:
    typedef enum {STRING, FLOAT, INT} DisplayOptionType;
    OnlineDisplayOption(std::string Name, 
			DisplayOptionType Type) 
      : m_name(Name), m_type(Type), m_value(0) {};
    virtual ~OnlineDisplayOption(); 
    std::string name() const { return m_name; }
    DisplayOptionType type() const { return m_type; }
    const void* value() const { return m_value; }
    void set(void* value); 
    void set(std::string value); 
    void set(int value); 
    void set(float value);
    void unset();
    bool isSet() { return (m_value != NULL);}     

  private:
    std::string m_name;
    DisplayOptionType m_type;
    void* m_value;
  };

  bool m_isAbort;
  std::string m_identifier;
  std::string m_page;
  int m_instance;
  std::string m_hid;
  std::string m_dimServiceName;
  int m_hsid;
  int m_ihs;
  int m_nhs;
  std::string m_hstype;
  std::string m_hsname;
  std::string m_subname;
  std::string m_task;
  std::string m_algorithm;
  int m_nanalysis;
  std::string m_Descr;
  std::string m_Doc;
  std::vector<int> m_anaId;
  std::vector<std::string> m_anaName;
  boolean m_isAnaHist;
  
  int m_creation;
  int m_obsoleteness;
  //display options
  bool m_DOinit;
  DisplayOptionMode m_InitDOmode;
  DisplayOptionMode m_domode;
  int m_hsdisp;
  int m_hdisp;
  int m_shdisp;
  std::vector<OnlineDisplayOption*> m_do;
  bool verifyPage(std::string &Page, int Instance);
  void load();
  bool checkHSDisplayFromDB();
  bool checkHDisplayFromDB();
  bool useSetForHistDisplay();
  bool useHistForPageDisplay();
  OnlineDisplayOption* getDO(std::string ParameterName); 
  void getDisplayOptions(int doid); 
  bool linkDisplayParameter(std::string& ParameterName,
			    Statement *st, 
			    int pos,
			    void* value);
  int algNpars(std::string* Algorithm,
	       int* anaid) const;
  int formatThresholds(std::string* Algorithm,
		       int* anaid,
		       std::vector<float>* warningThr, 
		       std::vector<float>* alarmThr,
		       std::string * warnString, 
		       std::string* alarmString) const;
  int setDisplayOptions(std::string function);
  void createDisplayOptions();
  int findAnalysis(std::string Algorithm,
		   int instance=1);
  bool loadCreationDirections();
};


class OnlineHistogramStorage
{
 public:
  OnlineHistogramStorage(OnlineHistDBEnv* Env);
  virtual ~OnlineHistogramStorage();
  void updateHistograms();
  /// gets a pointer to an OnlineHistogram object that can be used to view/edit an histogram record. If FullPathPageName
  /// is specified, the default display options for the histogram are those associated to the page (if available).
  /// Uses cached histogram objects if available
  OnlineHistogram* getHistogram(std::string Identifier,
				std::string FullPathPageName="_NONE_",
				int Instance = 1);
  /// same as getHistogram, but a new object is always created (no caching)
  OnlineHistogram* getNewHistogram(std::string Identifier,
				   std::string FullPathPageName="_NONE_",
				   int Instance = 1);
  /// removes an histogram, and optionally its full set. 
  /// ({\bf TEMPORARY METHOD TO BE REMOVED AT PRODUCTION STAGE})
  virtual bool removeHistogram(OnlineHistogram* h,
			       bool RemoveWholeSet = false);		 
private: 
  OnlineHistDBEnv* m_Histenv;
  std::vector<OnlineHistogram*> m_myHist;
  bool m_avoid_hdup;
};

#endif // ONLINEHISTOGRAM_H
