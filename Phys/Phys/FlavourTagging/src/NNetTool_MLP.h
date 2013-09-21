#ifndef NNETTOOL_NNETTOOL_MLP_H 
#define NNETTOOL_NNETTOOL_MLP_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "INNetTool.h"            // Interface

#include <list>
#include "TString.h"
#include <string>
#include <TROOT.h>

/** @class NNetTool_MLP NNetTool_MLP.h NNetTool_MLP.h
 *
 * Neural Net returns wrong tag fraction for a given tagger.
 * Inputs are typically: event-multiplicity,
 * tagger p/GeV, pt/GeV, IP-significance,
 * Delta-eta between B and tagger,
 * Delta-phi, Delta-Q (= mass of B+tagger system - mass of B).
 *  v1.4
 *  @author Marc Grabalosa
 *  @date   2010-06-17
 */
class NNetTool_MLP : public GaudiTool,
                     virtual public INNetTool {
public:
  /// Standard constructor
  NNetTool_MLP( const std::string& type,
                const std::string& name,
                const IInterface* parent );

  virtual ~NNetTool_MLP( ); ///< Destructor

  StatusCode initialize();
  StatusCode finalize();

  virtual double MLPm(std::vector<double>&);
  virtual double MLPe(std::vector<double>&);
  virtual double MLPk(std::vector<double>&);
  virtual double MLPkS(std::vector<double>&);
  virtual double MLPpS(std::vector<double>&);
  virtual double MLPvtx(std::vector<double>&);
  
  virtual double MLPmTMVA(std::vector<std::string>&, std::vector<double>&);
  virtual double MLPeTMVA(std::vector<std::string>&, std::vector<double>&);
  virtual double MLPkaonTMVA(std::vector<std::string>&, std::vector<double>&);
  virtual double MLPvtxTMVA(std::vector<std::string>&, std::vector<double>&);
  //virtual double MLPmTMVA(std::list<std::pair<std::string, Float_t> >&);
  //virtual double MLPeTMVA(std::list<std::pair<std::string, Float_t> >&);
  //virtual double MLPkaonTMVA(std::list<std::pair<std::string, Float_t> >&);
  //virtual double MLPvtxTMVA(std::list<std::pair<std::string, Float_t> >&);
  virtual double MLPmTMVA_MC(std::vector<std::string>&, std::vector<double>&);
  virtual double MLPeTMVA_MC(std::vector<std::string>&, std::vector<double>&);
  virtual double MLPkaonTMVA_MC(std::vector<std::string>&, std::vector<double>&);
  virtual double MLPvtxTMVA_MC(std::vector<std::string>&, std::vector<double>&);
  virtual double MLPpSTMVA_MC(std::vector<std::string>&, std::vector<double>&);
  virtual double MLPkSTMVA_MC(std::vector<std::string>&, std::vector<double>&);

private:

  void normaliseOS(std::vector<double>& );
  void normaliseSS(std::vector<double>& );
  void normalisepSS(std::vector<double>& );
  void normaliseVtx(std::vector<double>& );

  double pol2(double x, double a0, double a1);
  double pol3(double x, double a0, double a1, double a2);
  double pol4(double x, double a0, double a1, double a2, double a3);
  double func(double x, double a0, double a1, double a2, double a3);

  double m_P0mu, m_P1mu, m_P2mu, m_P3mu; 
  double m_P0e,  m_P1e,  m_P2e,  m_P3e; 
  double m_P0k,  m_P1k,  m_P2k,  m_P3k; 
  double m_P0vtx,m_P1vtx,m_P2vtx,m_P3vtx;  
  double m_P0ps, m_P1ps, m_P2ps, m_P3ps;  
  double m_P0ks, m_P1ks, m_P2ks, m_P3ks;
  /*
  std::string m_NNetWeights_mu, m_NNetWeights_ele, m_NNetWeights_kaon, m_NNetWeights_vtx, m_XML_dir;
  */
};

#endif // NNETTOOL_NNETTOOL_MLP_H
