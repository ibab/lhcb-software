#ifndef NNETTOOL_NNETTOOL_TMVA_H 
#define NNETTOOL_NNETTOOL_TMVA_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "INNetTool.h"            // Interface

#include <TMVA/Reader.h>


/** @class NNetTool_TMVA NNetTool_TMVA.h NNetTool_TMVA.h
 *  
 *  @author Marco Musy
 *  @date   2008-11-11
 */
class NNetTool_TMVA : public GaudiTool, 
                      virtual public INNetTool {
public:
  /// Standard constructor
  NNetTool_TMVA( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent );

  virtual ~NNetTool_TMVA( ); ///< Destructor

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
  void normaliseOS(std::vector<double>& );
  void normaliseSS(std::vector<double>& );
  double pol2(double x, double a0, double a1);
  double pol3(double x, double a0, double a1, double a2);

  std::string m_MuonMethod, m_ElectronMethod, m_KaonOSMethod;
  std::string  m_KaonSSMethod, m_PionSSMethod;

  TMVA::Reader* m_reader_e, *m_reader_mu, *m_reader_k, *m_reader_kS, *m_reader_pS;
  Float_t mult, ptB, partP, partPt, IPPV, nndeta, nndphi, nndq;

};
#endif // NNETTOOL_NNETTOOL_TMVA_H
