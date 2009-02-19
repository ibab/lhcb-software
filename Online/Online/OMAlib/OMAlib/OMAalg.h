// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/OMAlib/OMAalg.h,v 1.8 2009-02-19 10:49:50 ggiacomo Exp $
#ifndef OMALIB_OMAALG_H
#define OMALIB_OMAALG_H 1
/** @class  OMAalg OMAalg.h OMAlib/OMAalg.h
 * 
 *  generic Online Monitoring Analysis algorithm
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 3/10/2007
 */
#include "OMAlib/OMAcommon.h"
#include "OnlineHistDB/OMAMessage.h"
#include <TH1.h>

class OMAalg 
{
 public:
  OMAalg(std::string Name, OMAcommon* OMAenv);
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
  inline bool needRef() { return m_needRef; }

 protected:
  inline void setType(AlgType type) {m_type = type;}
  inline void setNpars(int N) {m_npars = N;}
  void setNinput(int N) {m_ninput = N;}
  inline void setDoc(std::string Doc) {m_doc = Doc;}
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
  inline void setNeedRef() { m_needRef = true; }

  std::string m_name;
  AlgType m_type;
  unsigned int m_npars;
  unsigned int m_ninput;
  std::vector<std::string> m_parnames;
  std::vector<float> m_parDefValues;
  std::string m_doc;
  OMAcommon* m_omaEnv;
  bool m_needRef;
 private:
   // private dummy copy constructor and assignment operator 
  OMAalg(const OMAalg&) {}
  OMAalg & operator= (const OMAalg&) {return *this;}

};

class OMACheckAlg : public OMAalg
{
 public:
  OMACheckAlg(std::string Name, OMAcommon* OMAenv) : OMAalg(Name,OMAenv),
    m_minEntries(50) {
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

 protected:
  std::vector<std::string> m_inputNames;
  std::vector<float> m_inputDefValues;
  int m_minEntries; // minimum entries for this test
 private:
   // private dummy copy constructor and assignment operator 
  OMACheckAlg(const OMACheckAlg&) : OMAalg(m_name, NULL) {}
  OMACheckAlg & operator= (const OMACheckAlg&) {return *this;}
};


class OMAHcreatorAlg : public OMAalg
{
 public:
  OMAHcreatorAlg(std::string Name, OMAcommon* OMAenv) : OMAalg(Name,OMAenv) {
    setType(OMAalg::HCREATOR);}
  virtual ~OMAHcreatorAlg() {}
  
  virtual TH1*  exec( const std::vector<TH1*> *sources,
                      const std::vector<float> *params,
                      std::string &outName,
                      std::string &outTitle,
                      TH1* existingHisto=0) =0;
  inline OnlineHistDBEnv::HistType outHType() const {return m_outHType;}
  inline bool histSetFlag() { return m_histSetFlag; }
  void setHistType(OnlineHistDBEnv::HistType htype) {m_outHType = htype;}
  void setHistSetFlag(bool Flag=true) { m_histSetFlag= Flag; }

 protected:
  OnlineHistDBEnv::HistType m_outHType;
  bool m_histSetFlag;

 private:
   // private dummy copy constructor and assignment operator 
  OMAHcreatorAlg(const OMAHcreatorAlg&) : OMAalg(m_name, NULL) {}
  OMAHcreatorAlg & operator= (const OMAHcreatorAlg&) {return *this;}
};
#endif // OMALIB_OMAALG_H
