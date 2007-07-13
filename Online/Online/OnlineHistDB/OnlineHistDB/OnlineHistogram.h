// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistogram.h,v 1.6 2007-07-13 15:55:25 ggiacomo Exp $
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
		  std::string Page="_NONE_",
		  int Instance=1);
  virtual ~OnlineHistogram();
  typedef enum { NONE, SET, HIST, HISTPAGE } DisplayOptionMode;
  void checkServiceName();
  bool setPage(std::string Page,
	       int Instance=1);
  void unsetPage();
  bool setDimServiceName(std::string DimServiceName);
  // DISPLAY OPTIONS
  bool initHistDisplayOptionsFromSet(); 
  bool initHistoPageDisplayOptionsFromSet(std::string PageName = "_DEFAULT_",
					  int Instance=-1);
  bool initHistoPageDisplayOptionsFromHist(std::string PageName = "_DEFAULT_",
					   int Instance=-1); 
  virtual bool setDisplayOption(std::string ParameterName, 
				void* value);
  virtual bool setHistoSetDisplayOption(std::string ParameterName, 
					void* value);
  virtual bool setHistDisplayOption(std::string ParameterName, 
				void* value);
  virtual bool setHistoPageDisplayOption(std::string ParameterName, 
					 void* value,
					 std::string PageName = "_DEFAULT_",
					 int Instance=-1);
  virtual bool unsetHistoSetDisplayOption(std::string ParameterName); 
  virtual bool unsetHistDisplayOption(std::string ParameterName); 
  virtual bool unsetHistoPageDisplayOption(std::string ParameterName);
  virtual bool unsetDisplayOption(std::string ParameterName);

  bool getDisplayOption(std::string ParameterName,
			void* option);
  /// number of instances of this histogram on any page
  int nPageInstances();
  // ANALYSIS OPTIONS
  int declareAnalysis(std::string Algorithm, 
		      std::vector<float>* warningThr=NULL, 
		      std::vector<float>* alarmThr=NULL, 
		      int instance=1); 
  bool setAnalysis(int AnaID, 
		   std::vector<float>* warningThr=NULL, 
		   std::vector<float>* alarmThr=NULL);
  bool getAnaSettings(int AnaID,
		      std::vector<float>* warn, 
		      std::vector<float>* alarm) const; 
  bool maskAnalysis(int AnaID,
		    bool Mask=true); 

  // access methods 
  bool isAbort() const {return m_isAbort;}
  std::string identifier() const {return m_identifier;}
  std::string page() const {return m_page;}
  int instance() const {return m_instance;}
  std::string hid() const {return m_hid;}
  std::string dimServiceName() const  {return m_dimServiceName;}
  int hsid() const {return m_hsid;}
  int ihs() const {return m_ihs;}
  int nhs() const {return m_nhs;}
  std::string hstype() const {return m_hstype;}
  std::string hsname() const {return m_hsname;}
  std::string subname() const {return m_subname;}
  std::string task() const {return m_task;}
  std::string algorithm() const {return m_algorithm;}
  int nanalysis() const {return m_nanalysis;}
  std::string descr() const {return m_Descr;}
  std::string doc() const {return m_Doc;}
  const std::vector<int>& anaId() const {return m_anaId;}
  const std::vector<std::string>& anaName() const {return m_anaName;}
  boolean isAnaHist() const {return m_isAnaHist;}
  int creation() const {return m_creation;}
  int obsoleteness() const {return m_obsoleteness;}
  DisplayOptionMode domode() const {return m_domode;}
  bool remove(bool RemoveWholeSet);
  void dump();
  void dumpDisplayOptions();

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
  bool verifyPage();
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
};


class OnlineHistogramStorage
{
 public:
  OnlineHistogramStorage(OnlineHistDBEnv* Env);
  virtual ~OnlineHistogramStorage();
  void updateHistograms();
  /// get an OnlineHistogram object, holding informations of an existing histogram, that can be used to view/edit an histogram record
  OnlineHistogram* getHistogram(std::string Identifier,
				std::string Page="_NONE_",
				int Instance = 1);
  /// remove an histogram (TEMPORARY METHOD TO BE REMOVED AT PRODUCTION STAGE)
  virtual bool removeHistogram(OnlineHistogram* h,
		       bool RemoveWholeSet = false);		 
private: 
  OnlineHistDBEnv* m_Histenv;
  std::vector<OnlineHistogram*> m_myHist;
};

#endif // ONLINEHISTOGRAM_H
