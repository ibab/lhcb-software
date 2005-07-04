// $Id: NNetTool.h,v 1.5 2005-07-04 15:40:09 pkoppenb Exp $
#ifndef NNETTOOL_NNETTOOL_H 
#define NNETTOOL_NNETTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "INNetTool.h"            // Interface

/** @class NNetTool NNetTool.h NNetTool.h
 *  
 * Neural Net returns wrong tag fraction for a given tagger.
 * Inputs are: B-signal momentum/GeV, B-theta, event-multiplicity,
 * tampering flag, tagger p/GeV, pt/GeV, IP-significance,
 * IP significance wrt secondary Vtx, Delta-eta between B and tagger,
 * Delta-phi, Delta-Q (= mass of B+tagger system - mass of B).
 *  v1.3
 *  @author Marco Musy
 *  @date   2004-12-14
 */
class NNetTool : public GaudiTool, 
	virtual public INNetTool {
public:
  /// Standard constructor
  NNetTool( const std::string& type, 
	    const std::string& name,
	    const IInterface* parent );

  virtual ~NNetTool( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  /// Finalize
  StatusCode finalize();
  
  virtual double MLPm(double ,double,double,
		      double ,double,double,double ,double);
  virtual double MLPe(double ,double,double,
		      double ,double,double,double ,double);
  virtual double MLPk(double ,double,double,
		      double ,double,double,double ,double);
  virtual double MLPkS(double,double,double,
		       double,double,double,
		       double,double,double,double ,double);
  virtual double MLPpS(double,double,double,
		       double,double,double,
		       double,double,double,double ,double);
private:
  double SIGMOID(double);
  void normaliseOS(double& , double& , double&,
		   double& , double& , double& ,
		   double& , double& );
  void normaliseSS(double& , double& , double&,
		   double& , double& , double& ,
		   double& , double& , double& , 
		   double& , double& );
};
#endif // NNETTOOL_NNETTOOL_H
