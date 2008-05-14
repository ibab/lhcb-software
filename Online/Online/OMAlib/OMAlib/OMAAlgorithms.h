// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/OMAlib/OMAAlgorithms.h,v 1.5 2008-05-14 10:01:16 ggiacomo Exp $
#ifndef OMALIB_OMAALGORITHMS_H
#define OMALIB_OMAALGORITHMS_H 1

#include "OMAlib/OMAalg.h"
class TH1;
class TH2D;
class TF1;

//----- Check Algorithms -----------------------------------//


class OMACheckXRange : public OMACheckAlg
{
  public:
  OMACheckXRange();
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars);
  void exec(TH1 &Histo,
            float warn_min,
            float warn_max,
            float alarm_min,
            float alarm_max);
 private:
  bool check(TH1 &Histo,
             float min,
             float max);
};

class OMACheckMeanAndSigma : public OMACheckAlg
{
  public:
  OMACheckMeanAndSigma();
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars);
 private:
  bool checkMean(TH1 &Histo,
                 float min,
                 float max,
                 float sig);
  bool checkSigma(TH1 &Histo,
                 float min,
                 float max,
                 float sig);
};

class OMAGaussFit : public OMACheckAlg
{
 public:
  OMAGaussFit();
  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars);
 private:
  bool checkParam( TF1* fit,
                   int ipar,
                   float min,
                   float max,
                   float sig);
};


//------ Histogram Creator Algorithms ------------------//

class OMAEfficiency : public OMAHcreatorAlg
{
 public:
  OMAEfficiency();
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string outName,
                     std::string outTitle,
                     TH1* existingHisto=0);  
  TH1* exec( TH1* okH,
             TH1* allH,
             std::string outName,
             std::string outTitle,
             TH1* existingHisto=0);
};

class OMADivide : public OMAHcreatorAlg
{
 public:
  OMADivide();
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string outName,
                     std::string outTitle,
                     TH1* existingHisto=0);  
  TH1* exec( TH1* okH,
             TH1* allH,
             std::string outName,
             std::string outTitle,
             TH1* existingHisto=0);
};


class OMAHMerge : public OMAHcreatorAlg
{
 public:
  OMAHMerge();
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string outName,
                     std::string outTitle,
                     TH1* existingHisto=0);  
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
  OMAScale();
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
                     std::string outName,
                     std::string outTitle,
                     TH1* existingHisto=0);  
  TH1* exec( TH1* H,
             TH1* scalefactorH,
             std::string outName,
             std::string outTitle,
             TH1* existingHisto=0);
};

#endif // OMALIB_OMAALGORITHMS_H
