#ifndef MYREADER_H 
#define MYREADER_H 1

// Include files
// from STL
//#include <string>
#include "TMVA/Reader.h"

/** @class MyReader MyReader.h
 *  
 *
 *  @author Stefania Vecchi
 *  @date   2013-02-01
 */
class MyReader {
public:
  typedef std::list<std::pair<std::string, Float_t> > ParListType;
  MyReader(const TString& method, const TString& weightfile, const ParListType& par):
    m_method(method),
    m_reader(new TMVA::Reader("!Color:Silent")),
    m_vars(new Float_t[par.size()])
  {
    Float_t *var = m_vars;
    for(ParListType::const_iterator p = par.begin(); p != par.end(); p++) {
       m_reader->AddVariable(p->first, var);
       ++var;
    }
    m_reader->BookMVA(method, weightfile);
  }
  ~MyReader() {
    delete[] m_vars;
  }

  
  inline void set(const ParListType& par) {
    std::transform(par.begin(), par.end(), m_vars,
                   [](const std::pair<std::string,Float_t>& p)
                   { return p.second; } );
  }

  inline Double_t eval() {
    return m_reader->EvaluateMVA(m_method);
  }

  inline Double_t eval(const ParListType& par) {
    set(par);
    return eval();
  }

private:

  TString m_method;
  std::unique_ptr<TMVA::Reader> m_reader;
  Float_t *m_vars;

};



#endif // MYREADER_H
