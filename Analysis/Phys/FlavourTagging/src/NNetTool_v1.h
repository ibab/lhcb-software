// $Id: NNetTool_v1.h,v 1.1 2006-05-16 10:23:20 musy Exp $
#ifndef NNETTOOL_NNETTOOL_V1_H 
#define NNETTOOL_NNETTOOL_V1_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "INNetTool.h"            // Interface

/** @class NNetTool_v1 NNetTool_v1.h NNetTool_v1.h
 *  
 * Neural Net returns wrong tag fraction for a given tagger.
 * Inputs are typically: event-multiplicity,
 * tampering flag, tagger p/GeV, pt/GeV, IP-significance,
 * IP significance wrt secondary Vtx, Delta-eta between B and tagger,
 * Delta-phi, Delta-Q (= mass of B+tagger system - mass of B).
 *  v1.3
 *  @author Marco Musy
 *  @date   2004-12-14
 */
class NNetTool_v1 : public GaudiTool, 
	virtual public INNetTool {
public:
  /// Standard constructor
  NNetTool_v1( const std::string& type, 
	    const std::string& name,
	    const IInterface* parent );

  virtual ~NNetTool_v1( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  /// Finalize
  StatusCode finalize();
  
  virtual double MLPm(std::vector<double>&);
  virtual double MLPe(std::vector<double>&);
  virtual double MLPk(std::vector<double>&);
  virtual double MLPkS(std::vector<double>&);
  virtual double MLPpS(std::vector<double>&);

private:
  double SIGMOID(double);
  void normaliseOS(std::vector<double>& );
  void normaliseSS(std::vector<double>& );
  double pol2(double x, double a0, double a1);
  double pol3(double x, double a0, double a1, double a2);

};
#endif // NNETTOOL_NNETTOOL_V1_H
