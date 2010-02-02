#ifndef NNETTOOL_NNETTOOL_DC06_H 
#define NNETTOOL_NNETTOOL_DC06_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "INNetTool.h"            // Interface

/** @class NNetTool_DC06 NNetTool_DC06.h NNetTool_DC06.h
 *  
 * Neural Net returns wrong tag fraction for a given tagger.
 * Inputs are typically: event-multiplicity,
 * tagger p/GeV, pt/GeV, IP-significance,
 * Delta-eta between B and tagger,
 * Delta-phi, Delta-Q (= mass of B+tagger system - mass of B).
 *  v1.3
 *  @author Marco Musy
 *  @date   2004-12-14
 */
class NNetTool_DC06 : public GaudiTool, 
                     virtual public INNetTool {
public:
  /// Standard constructor
  NNetTool_DC06( const std::string& type, 
                const std::string& name,
                const IInterface* parent );

  virtual ~NNetTool_DC06( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  /// Finalize
  StatusCode finalize();
  
  virtual double MLPm(std::vector<double>&);
  virtual double MLPe(std::vector<double>&);
  virtual double MLPk(std::vector<double>&);
  virtual double MLPkS(std::vector<double>&);
  virtual double MLPpS(std::vector<double>&);
  virtual double MLPvtx(std::vector<double>&);

private:
  double SIGMOID(double);
  void normaliseOS(std::vector<double>& );
  void normaliseSS(std::vector<double>& );
  double pol2(double x, double a0, double a1);
  double pol3(double x, double a0, double a1, double a2);
  double m_P0mu, m_P1mu, m_P0e, m_P1e, m_P0k, m_P1k; 
  double m_P0ks, m_P1ks, m_P0ps, m_P1ps, m_P0vtx, m_P1vtx;  

};
#endif // NNETTOOL_NNETTOOL_DC06_H
