// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistogram.h,v 1.36 2010-08-03 15:19:48 ggiacomo Exp $
#ifndef ONLINEHISTOGRAM_H
#define ONLINEHISTOGRAM_H 1
/** @class  OnlineHistogram OnlineHistogram.h OnlineHistDB/OnlineHistogram.h
 * 
 *  Histogram class for the C++ interface to the Online Monitoring Histogram DB
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 1/12/2006
 */
#include <map>

#include "OnlineHistDB/OnlineHistDBEnv.h"
#include "OnlineHistDB/OnlineHistTask.h"
#include "OnlineHistDB/OHDB_ocitypes.h"
class OnlineHistoOnPage;
class OnlineHistPage;

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
  inline std::string& identifier() {return m_identifier;}
  /// full path name of the page to which this histogram object is attached
  inline std::string& page() {return m_page;}
  /// associated OnlineHistPage object
  OnlineHistPage* pageObject();
  /// link to OnlineHistoOnPage object (if available)
  inline OnlineHistoOnPage* onpage() {return m_onpage;}
  /// counter (starting from 1) to distinguish several instances of the same histogram on the same page
  inline int instance() const {return m_instance;}
  /// name of the DIM service that is currently publishing the histogram
  inline std::string& dimServiceName() {return m_dimServiceName;}
  /// internal histogram set ID
  inline int hsid() const {return m_hsid;}
  /// position of this histogram in set (starting from 1). Can be larger than nhs() if some histogram in the set has been deleted
  inline int ihs() const {return m_ihs;}
  /// internal histogram ID (equivalent to hsid()/ihs())
  inline std::string& hid() {return m_hid;}
  /// number of histograms in set
  inline int nhs() const {return m_nhs;}
  /// histogram type (``H1D'', ``H2D'', ``P1D'',``P2D'', ``CNT'' or ``TRE'' )
  inline std::string& hstype() {return m_hstype;}
  /// histogram type (``H1D'', ``H2D'', ``P1D'',``P2D'', ``CNT'' or ``TRE'' )
  inline OnlineHistDBEnv::HistType type() {return m_type;}
  /// histogram dimension (1 or 2 for histograms, 0 for counters)
  inline int dimension() { 
    int out=-1;
    switch (m_type) {
    case OnlineHistDBEnv::SAM :
    case OnlineHistDBEnv::CNT ://should be 1?      
      out=0; break;
    case OnlineHistDBEnv::H1D :
    case OnlineHistDBEnv::P1D :
    case OnlineHistDBEnv::TRE :
    default:
      out=1; break;
    case OnlineHistDBEnv::H2D :
    case OnlineHistDBEnv::P2D :
      out=2; break;
    }
    return out;
  }
  /// histogram name
  inline std::string hname() const {return (m_subname == "") ? 
      m_hsname : (m_hsname + OnlineHistDBEnv_constants::m_SetSeparator + m_subname);}
  /// name of ROOT object in savesets
  inline std::string rootName() const {
    return m_algorithm +"/"+hname();
  }
  /// standard DB histogram title (not necessarly equal to the published ROOT title)
  inline std::string htitle() const {return (m_subname == "") ? 
      m_hsname : (m_hsname + " " + m_subname);} 
  /// histogram set name
  inline std::string& hsname() {return m_hsname;}
  /// subname
  inline std::string& subname() {return m_subname;}
  /// task name
  inline std::string& task() {return m_task;}
  /// algorithm name
  inline std::string& algorithm() {return m_algorithm;}  
  /// short description of the histogram
  std::string& doc() {return m_Doc;}
  /// link to a more extensive documentation
  std::string& descr() {return m_Descr;}
  /// creation date, as a unix timestamp
  int creation() const {return m_creation;}
  /// if the histogram is no more in use, returns the end--of--validity date
  /// as a unix timestamp, otherwise returns 0.
  int obsoleteness() const {return m_obsoleteness;}
  /// sets page on which histogram is displayed (reload display options if needed). Histogram has to be already
  /// been attached to the page through OnlineHistPage::declareHistogram()
  bool setPage(std::string FullPathPageName,
	       int Instance=1);
  inline void setOnpage(OnlineHistoOnPage* OnPage) {m_onpage=OnPage;}
  /// unsets page associated to histogram object
  void unsetPage();
  /// sets the DIM service name that is currently publishing the histogram. Returns true on success
  bool setDimServiceName(std::string& DimServiceName);
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
  /// gets associated task object
  OnlineHistTask* getTask();

  // DISPLAY OPTIONS
  /// specifies if the display options in this object are: not defined,
  /// associated to the histogram set, associated to the histogram,
  /// associated to the histogram on  page {\it page()}
  typedef enum { NONE, SET, HIST, HISTPAGE } DisplayOptionMode;
  DisplayOptionMode domode() const {return m_domode;}
  /// gets the current value of display option ParameterName, returning false if
  /// the option is unset.
  /// option must be a pointer to a variable allocated by the user, of type 
  /// std::string, int (signed) or float depending on the option type.
  /// The available option names and the corresponding types are listed 
  /// in section \ref{dispopts}.
  bool getDisplayOption(std::string ParameterName,
			void* option);
  /// checks is display option is set
  bool isSetDisplayOption(std::string ParameterName);
  /// get fit directions: function name and initial parameter values
  /// returns false if a fit function is not specified
  bool getFitFunction(std::string &Name, std::vector<float> *initValues=NULL, std::vector<float> *fitRange=NULL);
  /// see if a fit function is associated for display
  inline bool hasFitFunction() { return (!m_fitfun_null); }
  /// sets a display option (in the current display mode). Change is sent to the DB only 
  /// after a call to saveDisplayOptions()
  virtual bool setDisplayOption(std::string ParameterName, void* option);
  /// specify a fit to be performed at display time: function name and initial parameter values
  void setFitFunction(std::string Name, std::vector<float> *initValues=NULL, std::vector<float> *fitRange=NULL);
  /// unsets a display option (in the current display mode)
  virtual bool unsetDisplayOption(std::string ParameterName);
  /// unsets all display options (in the current display mode)
  void unsetAllDisplayOptions();
  /// returns true if the current option value is different from *option
  virtual bool changedDisplayOption(std::string ParameterName, 
				    void* option);
  /// saves current display options.  This is the recommended saving method, that
  /// chooses the most appropriate display option mode 
  /// (Histogram set, Histogram, or Histogram on Page) calling one of the next methods.
  virtual bool saveDisplayOptions();
  /// saves current display options for the whole histogram set
  virtual bool saveHistoSetDisplayOptions();
  /// saves current display options for the present histogram.
  virtual bool saveHistDisplayOptions();
  /// attaches current display options to instance Instance of the present histogram on page 
  /// Page (default values for Page, Instance are {\it page()},{\it instance()}  )
  virtual bool saveHistoPageDisplayOptions(std::string Page = "_DEFAULT_",
					   int Instance=-1);

  void getIntDisplayOptions( std::map<std::string, int>& buf );
  void getFloatDisplayOptions( std::map<std::string, float>& buf );
  void getStringDisplayOptions( std::map<std::string, std::string>& buf );

  
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
  /// copies display options from another histogram record
  bool copyDisplayOptions(OnlineHistogram* h);
  /// copies display options from another histogram on a given page
  bool copyDisplayOptions(OnlineHistoOnPage* h);


  /// set custom bin labels for X and/or Y axis. Provide null pointers for not changing labels
  bool setBinLabels(std::vector<std::string> *Xlabels, 
                    std::vector<std::string> *Ylabels);
  /// number of custom bin labels
  inline unsigned int nbinlabels() {return m_binlabels.size();}
  /// number of custom bin labels for X axis
  inline unsigned int nXbinlabels() {return m_xbinlab;}
  /// number of custom bin labels for Y axis
  inline unsigned int nYbinlabels() {return m_ybinlab;}
  inline std::string binlabel(unsigned int i, int axis=0) { 
    unsigned int j = (0 == axis) ? i : i+m_xbinlab;
    if(j<m_binlabels.size()) return m_binlabels[j];
    else return "";
  }

  /// get the name of page to be displayed when histogram is clicked on Presenter
  std::string page2display() {return m_refPage;} 
  /// set the name of page to be displayed when histogram is clicked on Presenter
  bool setPage2display(std::string& pageName);

  // ANALYSIS OPTIONS
  /// for analysis histogram, get the directions for creating histogram
  bool getCreationDirections(std::string& Algorithm,
                             std::vector<std::string> &source_list,
                             std::vector<float> &parameters,
                             int* optionalSourceSet=NULL);
  /// number of analysis to be performed on the histogram set
  int nanalysis() const {return m_nanalysis;}
  /// get analysy description as vectors of length {\it  nanalysis}() containing 
  /// the analysis internal IDs and the analysis algorithm names
  void getAnalyses(std::vector<int>& anaIDs,
		   std::vector<std::string>& anaAlgs) {
    if(!m_anadirLoaded) loadAnalysisDirections();
    anaIDs.clear();    
    anaAlgs.clear();    
    anaIDs.insert(anaIDs.begin(),m_anaId.begin(),m_anaId.end());
    anaAlgs.insert(anaAlgs.begin(),m_anaName.begin(),m_anaName.end());
  }
  /// true if the histogram is a virtual histogram (produced at display/analysis level)
  bool isAnaHist() const {return m_isAnaHist;}
  /// true if the histogram is a virtual histogram (produced at display/analysis level)
  bool isVirtual() {return  m_isAnaHist;}

  /// declare an analysis to be performed on the histogram set. If the algorithm
  /// requires some parameters, the input parameters and the warning and alarm 
  /// thresholds for output parameters must be
  /// specified as vectors of floats and will be set for all histograms in
  /// set (then, you can specify values for single histograms with the {\it
  /// setAnalysis} method. Doc should document the analysis, Message is a short
  /// optional sentence that will appear in the warnings/alarms messages.
  /// You can create more than one analysis
  /// with the same algorithm by using instance > 1. If the analysis
  /// identified by Algorithm and instance already exists, parameters are
  /// updated. Returns the internal analysis ID.
  int declareAnalysis(std::string Algorithm, 
                      std::vector<float>* warningThr=NULL, 
                      std::vector<float>* alarmThr=NULL, 
                      std::vector<float>* inputs=NULL, 
                      int instance=1,
                      std::string *Doc=NULL,
                      std::string *Message=NULL); 
  /// updates parameters for analysis with ID AnaID (for this histogram only). Returns true on success
  bool setAnalysis(int AnaID, 
                   std::vector<float>* warningThr=NULL, 
                   std::vector<float>* alarmThr=NULL, 
                   std::vector<float>* inputs=NULL);
  /// gets parameters for analysis with ID AnaID. Returns true on success
  bool getAnaSettings(int AnaID,
                      std::vector<float>* warn, 
                      std::vector<float>* alarm,
                      std::vector<float>* inputs,
                      bool &mask,
                      long &statusbits,
                      float &minstat,
                      float &minstatfrac); 
  /// masks analysis with ID AnaID. Use Mask=false to unmask. Returns true on success
  bool maskAnalysis(int AnaID,
                    bool Mask=true); 

  /// renames an histogram (including its full set if needed) 
  /// ({\bf TEMPORARY METHOD TO BE REMOVED AT PRODUCTION STAGE})
  bool rename(std::string &newTask,
              std::string &newAlgo,
              std::string &newName);

  // methods to force reloading of all or part of histogram variables
  void load();
  void reload();
  bool loadAnalysisDirections();
  bool loadCreationDirections();

 protected:
  std::string CreationAlgorithm;
  std::vector<std::string> Sources;
  int SourceSet;
  sb2 SourceSet_null;
  std::vector<float> SourcePar;

 private:
  // private dummy copy constructor and assignment operator 
  OnlineHistogram(const OnlineHistogram&) : OnlineHistDBEnv() {}
  OnlineHistogram & operator= (const OnlineHistogram&) {return *this;}
  // private OnlineDisplayOption class
  class OnlineDisplayOption {
  public:
    typedef enum {STRING, FLOAT, INT} DisplayOptionType;
    OnlineDisplayOption(std::string Name, 
			DisplayOptionType Type,
			void* OCIvar,
			OCIInd* OCIvar_null,
			OnlineHistDBEnv * Env);
    virtual ~OnlineDisplayOption();
    std::string& name() { return m_name; }
    inline DisplayOptionType type() const { return m_type; }    
    void set(void* option);
    bool get(void* option);
    void unset();
    inline bool isSet() { return (0 == *m_value_null);
    }     
    bool differentfrom(void* option);
  private:
    std::string m_name;
    DisplayOptionType m_type;
    void* m_value;
    OCIInd* m_value_null;
    bool m_locString;
    OnlineHistDBEnv* m_env;
    std::string getStringValue();
    int getIntValue();
    float getFloatValue();
  };

  bool m_isAbort;
  std::string m_identifier;
  std::string m_page;
  OnlineHistoOnPage* m_onpage;
  int m_instance;
  bool m_page_verified;
  std::string m_hid;
  std::string m_dimServiceName;
  sb2 m_dimServiceName_null;
  int m_hsid;
  int m_ihs;
  int m_nhs;
  std::string m_hstype;
  OnlineHistDBEnv::HistType m_type;
  std::string m_hsname;
  std::string m_subname;
  sb2 m_subname_null;
  std::string m_task;
  std::string m_algorithm;
  int m_nanalysis;
  std::string m_Descr;
  sb2 m_Descr_null;
  std::string m_Doc;
  sb2 m_Doc_null;
  std::vector<int> m_anaId;
  std::vector<std::string> m_anaName;
  
  bool m_isAnaHist;
  
  int m_creation;
  int m_obsoleteness;
  sb2 m_obsoleteness_null;
  //display options
  bool m_DOinit;
  DisplayOptionMode m_InitDOmode;
  DisplayOptionMode m_domode;
  int m_hsdisp;
  sb2 m_hsdisp_null;
  int m_hdisp;
  sb2 m_hdisp_null;
  int m_shdisp;
  sb2 m_shdisp_null;
  dispopt *m_dispopt;
  dispoptInd *m_dispopt_null;
  std::vector<OnlineDisplayOption*> m_do;
  std::string m_fitfun;
  sb2 m_fitfun_null;
  std::vector<float> m_fitPars;
  std::vector<float> m_fitRange;

  std::vector<std::string> m_binlabels;
  int m_xbinlab;
  sb2 m_xbinlab_null;
  int m_ybinlab;
  sb2 m_ybinlab_null;
  std::string m_refPage;
  sb2 m_refPage_null;
  

  bool verifyPage(std::string &Page, int Instance);
  bool checkHSDisplayFromDB();
  bool checkHDisplayFromDB();
  bool useSetForHistDisplay();
  bool useHistForPageDisplay();
  OnlineDisplayOption* getDO(std::string ParameterName); 
  void getDisplayOptions(int doid); 
  bool copyDO(std::string hname, std::string page="", unsigned int instance=1);
  std::string& algFromID(int anaid);
  int setDisplayOptions(std::string function);
  void createDisplayOptions();
  int findAnalysis(std::string Algorithm,
		   int instance=1);
  bool m_anadirLoaded;
  bool m_credirLoaded;
};


class OnlineHistogramStorage
{
 public:
  OnlineHistogramStorage();
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
  void reloadAnalysisForSet(int Hsid,
			    int ihs);
 protected:
  void setHistEnv(OnlineHistDBEnv* Env) {m_Histenv = Env;}
  void reloadHistograms();
 private: 
  OnlineHistDBEnv* m_Histenv;
  std::vector<OnlineHistogram*> m_myHist;
  bool m_avoid_hdup;
};

#endif // ONLINEHISTOGRAM_H
