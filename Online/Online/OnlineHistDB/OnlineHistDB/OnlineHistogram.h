// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistogram.h,v 1.2 2007-01-22 17:07:32 ggiacomo Exp $
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
  typedef enum { NONE, SET, HIST, HISTPAGE } DisplayOptionMode;
  OnlineHistogram(std::string Name,
		  Connection* conn,
		  std::string Page="_NONE_",
		  int Instance=1);
  virtual ~OnlineHistogram();
  void update();
  void setPage(std::string Page,
	       int Instance=1);
  // DISPLAY OPTIONS
  bool initDisplayOptionsFromSet(); 
  bool initHistoPageDisplayOptionsFromSet(std::string PageName = "_DEFAULT_",
					  int Instance=-1);
  bool initHistoPageDisplayOptionsFromHist(std::string PageName = "_DEFAULT_",
					   int Instance=-1); 
  bool setHistoSetDisplayOption(std::string ParameterName, 
				void* value);
  bool setDisplayOption(std::string ParameterName, 
			void* value);
  bool setHistoPageDisplayOption(std::string ParameterName, 
				 void* value,
				 std::string PageName = "_DEFAULT_",
				 int Instance=-1);
  bool getDisplayOption(std::string ParameterName,
					 void* option);

  // ANALYSIS OPTIONS
  int declareAnalysis(std::string Algorithm, 
		      std::vector<float>* warningThr=NULL, 
		      std::vector<float>* alarmThr=NULL, 
		      int instance=1); 
  void setAnalysis(int AnaID, 
		   std::vector<float>* warningThr=NULL, 
		   std::vector<float>* alarmThr=NULL);
  void getAnaSettings(int AnaID,
		      std::vector<float>* warn, 
		      std::vector<float>* alarm) const; 
  void maskAnalysis(int AnaID,
		    bool Mask=true); 

  // access methods 
  bool isAbort() const {return m_isAbort;}
  std::string name() const {return m_name;}
  std::string page() const {return m_page;}
  int instance() const {return m_instance;}
  std::string hid() const {return m_hid;}
  std::string dimServiceName() const  {return m_dimServiceName;}
  int hsid() const {return m_hsid;}
  int ihs() const {return m_ihs;}
  int nhs() const {return m_nhs;}
  std::string hstype() const {return m_hstype;}
  std::string hstitle() const {return m_hstitle;}
  std::string subtitle() const {return m_subtitle;}
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
  
  
 private:
  // private dummy copy constructor and assignment operator 
  OnlineHistogram(const OnlineHistogram&) : OnlineHistDBEnv() {}
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
    void set(std::string value); 
    void set(int value); 
    void set(float value);
    bool isSet() { return (m_value != NULL);}     
  private:
    std::string m_name;
    DisplayOptionType m_type;
    void* m_value;
  };

  bool m_isAbort;
  std::string m_name;
  std::string m_page;
  int m_instance;
  std::string m_hid;
  std::string m_dimServiceName;
  int m_hsid;
  int m_ihs;
  int m_nhs;
  std::string m_hstype;
  std::string m_hstitle;
  std::string m_subtitle;
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
  DisplayOptionMode m_domode;
  int m_hsdisp;
  int m_hdisp;
  int m_shdisp;
  std::vector<OnlineDisplayOption*> m_do;
  void load();
  OnlineDisplayOption* getDO(std::string ParameterName); 
  void getDisplayOptions(int doid); 
  bool linkDisplayParameter(std::string& ParameterName,
			    Statement *st, 
			    int pos,
			    void* value);
  int algNinput(std::string* Algorithm,
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


#endif // ONLINEHISTOGRAM_H
