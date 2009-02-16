// $Id: OMAFitFunction.h,v 1.1 2009-02-16 10:38:21 ggiacomo Exp $
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
  void fit(TH1* histo, std::vector<float>* initValues);
protected:

private:
  std::string m_name;
  std::vector<std::string> m_parNames;
  bool m_mustInit;
  std::string m_doc;
  std::string m_funcString;
  TF1* m_fitfun;
  bool m_predef;
};
#endif // OMALIB_OMAFITFUNCTION_H
