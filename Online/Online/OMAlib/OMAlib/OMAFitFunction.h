// $Id: OMAFitFunction.h,v 1.9 2010-10-19 13:30:53 ggiacomo Exp $
#ifndef OMALIB_OMAFITFUNCTION_H 
#define OMALIB_OMAFITFUNCTION_H 1
#include <string>
#include <vector>
#include <TF1.h>
class TH1;


/** @class OMAFitFunction OMAFitFunction.h OMAlib/OMAFitFunction.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2009-02-10
 */
class OMAFitFunction : public TF1 {
public: 
  /// constructor for derived class (implementing custom fit function)
  OMAFitFunction(std::string Name);
  /// constructor for simple fit functions defined by funcString
  OMAFitFunction(std::string Name,
                 std::string FuncString,
                 std::vector<std::string> &ParNames,
                 bool MustInit,
                 std::string Doc,
                 bool predefined); 
  virtual ~OMAFitFunction( ); ///< Destructor

  virtual OMAFitFunction* clone() {
    return (new OMAFitFunction(*this));
  }
  inline std::string& name() {return m_name;}
  inline int np(){return m_parNames.size();}
  inline int ninput(){return m_inputNames.size();}
  inline std::string& doc() {return m_doc;}
  inline bool mustInit() {return m_mustInit;}
  std::vector<std::string>& parNames() {return m_parNames;}
  std::vector<float>& parDefValues() {return m_parDefValues;}
  std::vector<std::string>& inputNames() {return m_inputNames;}
  std::vector<float>& inputDefValues() {return m_inputDefValues;}
  void checkDefValues();
  virtual void fit(TH1* histo, std::vector<float>* initValues);
  TF1* fittedfun() {return this;}
protected:
  virtual void init(std::vector<float>* initValues, TH1* histo=NULL);
  void initfun();

  std::string m_name;
  std::vector<std::string> m_parNames;
  std::vector<float> m_parDefValues;
  std::vector<std::string> m_inputNames;
  std::vector<float> m_inputDefValues;
  bool m_mustInit;
  std::string m_doc;
  std::string m_funcString;
  bool m_predef;
  bool m_useTF1;
};


class OMAFitDoubleGaus : public OMAFitFunction {
public: 
  OMAFitDoubleGaus();
  virtual ~OMAFitDoubleGaus() {}
  virtual OMAFitFunction* clone() {
    return (OMAFitFunction*) (new OMAFitDoubleGaus(*this));
  }
protected:
  virtual void init(std::vector<float>* initValues, TH1* histo=NULL);
};

class OMAFitGausPlusBkg : public OMAFitFunction {
public: 
  OMAFitGausPlusBkg(unsigned int degree);
  virtual ~OMAFitGausPlusBkg() {}
  virtual OMAFitFunction* clone() {
    return (OMAFitFunction*) (new OMAFitGausPlusBkg(*this));
  }
protected:
  virtual void init(std::vector<float>* initValues, TH1* histo=NULL);
  unsigned int m_degree;
};

class OMAFitTH2withSinCosC: public OMAFitFunction {
 public: 
  OMAFitTH2withSinCosC();
  virtual ~OMAFitTH2withSinCosC();
  virtual OMAFitFunction* clone() {
    return (OMAFitFunction*) (new OMAFitTH2withSinCosC(*this));
  }
  virtual void fit(TH1* histo, std::vector<float>* initValues);
private:
  TF1 GausP1;
};
 
class OMAFitHLTjpsi: public OMAFitFunction {
 public: 
  OMAFitHLTjpsi();
  virtual ~OMAFitHLTjpsi();
  virtual OMAFitFunction* clone() {
    return (OMAFitFunction*) (new OMAFitHLTjpsi(*this));
  }
  virtual void fit(TH1* histo, std::vector<float>* initValues);
private:
  TF1 m1;
  TF1 m2;
  TF1 m3;
};


#endif // OMALIB_OMAFITFUNCTION_H
