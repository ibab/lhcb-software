// $Id: OMAFitFunction.h,v 1.3 2009-02-26 12:37:49 ggiacomo Exp $
#ifndef OMALIB_OMAFITFUNCTION_H 
#define OMALIB_OMAFITFUNCTION_H 1
#include <string>
#include <vector>
class TF1;
class TH1;


/** @class OMAFitFunction OMAFitFunction.h OMAlib/OMAFitFunction.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2009-02-10
 */
class OMAFitFunction {
public: 
  //constructor for derived class (implementing custom fit function)
  OMAFitFunction(std::string Name);
  // cunstructor for simple fit functions defined by funcString
  OMAFitFunction(std::string Name,
                 std::string FuncString,
                 std::vector<std::string> &ParNames,
                 bool MustInit,
                 std::string Doc,
                 bool predefined); 

  virtual ~OMAFitFunction( ); ///< Destructor

  inline std::string& name() {return m_name;}
  inline int np(){return m_parNames.size();}
  inline std::string& doc() {return m_doc;}
  inline bool mustInit() {return m_mustInit;}
  std::vector<std::string>& parNames() {return m_parNames;}
  virtual void fit(TH1* histo, std::vector<float>* initValues);
protected:
  virtual void init(std::vector<float>* initValues, TH1* histo=NULL);
  void initfun();

  std::string m_name;
  std::vector<std::string> m_parNames;
  bool m_mustInit;
  std::string m_doc;
  std::string m_funcString;
  TF1* m_fitfun;
  bool m_predef;
};


class OMAFitDoubleGaus : public OMAFitFunction {
public: 
  OMAFitDoubleGaus();
  virtual ~OMAFitDoubleGaus() {}
protected:
  virtual void init(std::vector<float>* initValues, TH1* histo=NULL);
};

class OMAFitGausPlusBkg : public OMAFitFunction {
public: 
  OMAFitGausPlusBkg(unsigned int degree);
  virtual ~OMAFitGausPlusBkg() {}
protected:
  virtual void init(std::vector<float>* initValues, TH1* histo=NULL);
  unsigned int m_degree;
};

class OMAFitTH2withSinCosC: public OMAFitFunction {
 public: 
  OMAFitTH2withSinCosC();
  virtual ~OMAFitTH2withSinCosC();
  virtual void fit(TH1* histo, std::vector<float>* initValues);
private:
  TF1* GausP1;
};
 

#endif // OMALIB_OMAFITFUNCTION_H
