// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/OMAlib/OMAAlgorithms.h,v 1.17 2010-07-04 13:29:55 ggiacomo Exp $
#ifndef OMALIB_OMAALGORITHMS_H
#define OMALIB_OMAALGORITHMS_H 1

#include <sstream>
#include "OMAlib/OMAalg.h"
#include "OMAlib/OMAEnv.h"
class TH1;
class TH2D;
class TF1;

//----- Check Algorithms -----------------------------------//


class OMACheckXRange : public OMACheckAlg
{
  public:
  OMACheckXRange(OMAlib* Env);
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref);
  void exec(TH1 &Histo,
            float warn_min,
            float warn_max,
            float alarm_min,
            float alarm_max,
            unsigned int anaID);
 private:
  bool findBadEntries(TH1 &Histo,
                       float min,
                       float max,
                       std::stringstream &mess);
};

class OMACheckMeanAndSigma : public OMACheckAlg
{
  public:
  OMACheckMeanAndSigma(OMAlib* Env);
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref);
 private:
  bool checkMean(TH1 &Histo,
                 int axis,
                 float min,
                 float max,
                 float sig,
                 std::stringstream &mess);
  bool checkSigma(TH1 &Histo,
                  int axis,
                  float min,
                  float max,
                  float sig,
                  std::stringstream &mess);
};

class OMAGaussFit : public OMACheckAlg
{
 public:
  OMAGaussFit(OMAlib* Env);
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref);
 private:
  bool checkParam( TF1* fit,
                   int ipar,
                   float min,
                   float max,
                   float sig,
                   std::stringstream &mess);
};

class OMACheckHolesAndSpikes  : public OMACheckAlg
{
 public:
  OMACheckHolesAndSpikes(OMAlib* Env);
  enum DeltaMode {MinMode=0, Ratio =1, AbsDiff =2, SigmaDiff=3, MaxMode=4};
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref);
  virtual bool refMissing(TH1* ref,
                          std::vector<float> & input_pars);
};

class OMACheckEmptyBins  : public OMACheckAlg
{
 public:
  OMACheckEmptyBins(OMAlib* Env);
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref);
};

class OMACompareToReference : public OMACheckAlg
{
 public:
  OMACompareToReference(OMAlib* Env);
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref);
};

class OMACheckEntriesInRange : public OMACheckAlg
{
 public:
  OMACheckEntriesInRange(OMAlib* Env);
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref);
};

class OMAFit : public OMACheckAlg 
// special one: parameters taken dinamically according to requested fit function
{
public:
  OMAFit(OMAlib* Env);
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref);
};

class OMAIfbMonitor  : public OMACheckAlg
{
public:
  OMAIfbMonitor(OMAlib* Env);
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref);
  void setOutputDir(std::string &dir) {m_OutputDir=dir;}
private:
  std::string m_OutputDir;
  int ID2Xnum1(int ID);
  int ID2Ynum1(int ID);
  int ID2Xnum2(int ID);
  int ID2Ynum2(int ID);
};
class OMACheckDeadBins  : public OMACheckAlg
{
public:
  OMACheckDeadBins(OMAlib* Env);
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref);
  virtual bool refMissing(TH1* ref,
                          std::vector<float> & input_pars);
};
class OMACheckErrorBins : public OMACheckAlg
{
public:
  OMACheckErrorBins(OMAlib* Env);
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref);
};
class OMACheckMinMax : public OMACheckAlg
{
public:
  OMACheckMinMax(OMAlib* Env);
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars,
                    unsigned int anaID,
                    TH1* Ref);
private:
  bool checkContent(TH1 &Histo, int ix, int iy, 
                    float min, float max,
                    float conf);
  double m_k2c;
  std::vector<int> m_badbins;
};

//------ Histogram Creator Algorithms ------------------//

class OMAEfficiency : public OMAHcreatorAlg
{
public:
  OMAEfficiency(OMAlib* Env);
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string &outName,
                     std::string &outTitle,
                     TH1* existingHisto=0,
                     TH1* Ref=0);  
  TH1* exec( TH1* okH,
             TH1* allH,
             std::string &outName,
             std::string &outTitle,
             TH1* existingHisto=0);
};

class OMADivide : public OMAHcreatorAlg
{
public:
  OMADivide(OMAlib* Env);
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string &outName,
                     std::string &outTitle,
                     TH1* existingHisto=0,
                     TH1* Ref=0);  
};
class OMAMultiply : public OMAHcreatorAlg
{
public:
  OMAMultiply(OMAlib* Env);
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string &outName,
                     std::string &outTitle,
                     TH1* existingHisto=0,
                     TH1* Ref=0);  
};
class OMAAdd : public OMAHcreatorAlg
{
public:
  OMAAdd(OMAlib* Env);
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string &outName,
                     std::string &outTitle,
                     TH1* existingHisto=0,
                     TH1* Ref=0);  
};
class OMAAsymmetry : public OMAHcreatorAlg
{
public:
  OMAAsymmetry(OMAlib* Env);
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string &outName,
                     std::string &outTitle,
                     TH1* existingHisto=0,
                     TH1* Ref=0);  
};

class OMAHMerge : public OMAHcreatorAlg
{
public:
  OMAHMerge(OMAlib* Env);
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string &outName,
                     std::string &outTitle,
                     TH1* existingHisto=0,
                     TH1* Ref=0);  
private:
  bool approxeq(double x, 
                double y ) ;
  TH1* hMerge(const char* newname, 
              const char* newtitle, 
              const std::vector<TH1*> *in);
  void fillMerged(TH1* newH, 
                  const std::vector<TH1*> *in);

};

class OMAScale : public OMAHcreatorAlg
{
public:
  OMAScale(OMAlib* Env);
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string &outName,
                     std::string &outTitle,
                     TH1* existingHisto=0,
                     TH1* Ref=0);  
};
class OMAProject : public OMAHcreatorAlg
{
public:
  OMAProject(OMAlib* Env);
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string &outName,
                     std::string &outTitle,
                     TH1* existingHisto=0,
                     TH1* Ref=0);  
};
class OMARebin : public OMAHcreatorAlg
{
public:
  OMARebin(OMAlib* Env);
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string &outName,
                     std::string &outTitle,
                     TH1* existingHisto=0,
                     TH1* Ref=0);  
};
class OMADivideByReference: public OMAHcreatorAlg
{
public:
  OMADivideByReference(OMAlib* Env);
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string &outName,
                     std::string &outTitle,
                     TH1* existingHisto=0,
                     TH1* Ref=0);  
};
class OMASubtractReference: public OMAHcreatorAlg
{
public:
  OMASubtractReference(OMAlib* Env);
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string &outName,
                     std::string &outTitle,
                     TH1* existingHisto=0,
                     TH1* Ref=0);  
};

#endif // OMALIB_OMAALGORITHMS_H
