// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/OMAlib/OMAalg.h,v 1.15 2010-06-11 13:00:10 ggiacomo Exp $
#ifndef OMALIB_OMAALG_H
#define OMALIB_OMAALG_H 1
/** @class  OMAalg OMAalg.h OMAlib/OMAalg.h
 * 
 *  generic Online Monitoring Analysis algorithm
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 3/10/2007
 */

#include "OnlineHistDB/OMAMessage.h"
#include <TH1.h>

class OMAlib;
class OnlineHistogram;

class OMAalg 
{
 public:
  OMAalg(std::string Name, OMAlib* OMAenv);
  virtual ~OMAalg();
  inline std::string& name() { return m_name;}
  typedef enum { HCREATOR, CHECK} AlgType;
  inline AlgType type() {return m_type;}
  std::string typestr();
  inline int npars() const {return m_npars;}
  inline int ninput() const {return m_ninput;}
  inline std::string& doc() {return m_doc;}
  inline std::string& parName(int i)  { return m_parnames[i];}
  inline float parDefValue(int i)  { return m_parDefValues[i];}
  void setOnlineHistogram(OnlineHistogram *h) {m_oh=h;}
  inline bool needRef() { return m_needRef; }
  double content2counts(TH1 &Histo);
 protected:
  inline void setType(AlgType type) {m_type = type;}
  inline void setNpars(int N) {m_npars = N;}
  void setNinput(int N) {m_ninput = N;}
  inline void setDoc(std::string Doc) {m_doc = Doc;}
  inline int intParam(const float &param) 
    {return (int)(param+ (param>0 ? 0.1 : -0.1));}
  void setParNames(std::vector<std::string> &ParNames);
  void raiseMessage(unsigned int Id,
                    OMAMessage::OMAMsgLevel level,
                    std::string message,
                    std::string& histogramName);
  void raiseMessage(unsigned int Id,
                    bool warnCondition,
                    bool alarmCondition,
                    std::string message,
                    std::string& histogramName);
  virtual bool refMissing(TH1* ref,
                          std::vector<float> &) {
    return ( m_needRef && !ref); // default implementation
  }
  bool getBinLabels(TH1 &Histo);

  std::string m_name;
  AlgType m_type;
  unsigned int m_npars;
  unsigned int m_ninput;
  std::vector<std::string> m_parnames;
  std::vector<float> m_parDefValues;
  std::string m_doc;
  OMAlib* m_omaEnv;
  OnlineHistogram* m_oh;
  bool m_needRef;
 private:
   // private dummy copy constructor and assignment operator 
  OMAalg(const OMAalg&) {}
  OMAalg & operator= (const OMAalg&) {return *this;}
};

class OMACheckAlg : public OMAalg
{
 public:
  OMACheckAlg(std::string Name, OMAlib* OMAenv) : OMAalg(Name,OMAenv),
                                                  m_minEntriesPerBin(5) {
    setType(OMAalg::CHECK); 
    m_inputNames.clear(); 
    m_inputDefValues.clear(); 
  }
  virtual ~OMACheckAlg() {}
  inline std::string inputName(int i) const { return m_inputNames[i];}
  inline float inputDefValue(int i) const { return m_inputDefValues[i];}

  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref) =0;
  virtual bool checkStats(TH1* h,
                          unsigned int anaID,
                          TH1* ref,
                          std::vector<float> & input_pars,
                          float minstatperbin,
                          float minstatfrac);

 protected:
  virtual bool notEnoughStats(TH1* h,
                              float minstatperbin,
                              float minstatfrac);
  std::vector<std::string> m_inputNames;
  std::vector<float> m_inputDefValues;
  int m_minEntriesPerBin; // default minimum entries per bin for this test, can be overriden by single analysis
};


class OMAHcreatorAlg : public OMAalg
{
 public:
  OMAHcreatorAlg(std::string Name, OMAlib* OMAenv) : OMAalg(Name,OMAenv) {
    setType(OMAalg::HCREATOR);}
  virtual ~OMAHcreatorAlg() {}
  
  virtual TH1*  exec( const std::vector<TH1*> *sources,
                      const std::vector<float> *params,
                      std::string &outName,
                      std::string &outTitle,
                      TH1* existingHisto=0,
                      TH1* Ref=0) =0;
  inline OnlineHistDBEnv::HistType outHType() const {return m_outHType;}
  /// input can be an histogram set ?
  inline bool histSetFlag() { return m_histSetFlag; }
  void setHistType(OnlineHistDBEnv::HistType htype) {m_outHType = htype;}
  void setHistSetFlag(bool Flag=true) { m_histSetFlag= Flag; }

 protected:
  bool sourceVerified(const std::vector<TH1*> *sources) {
    bool out=true;
    if (sources) {
      std::vector<TH1*>::const_iterator is;
      for (is=sources->begin(); is!= sources->end(); is++) {
        if ( NULL == (*is)) out=false;
      }
    }
    else {
      out=false;
    }
    return out;
  }
  OnlineHistDBEnv::HistType m_outHType;
  bool m_histSetFlag;

};
#endif // OMALIB_OMAALG_H
