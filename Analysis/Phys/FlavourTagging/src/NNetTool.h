// $Id: NNetTool.h,v 1.2 2005-02-02 07:10:27 pkoppenb Exp $
#ifndef NNETTOOL_NNETTOOL_H 
#define NNETTOOL_NNETTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "FlavourTagging/INNetTool.h"            // Interface

/** @class NNetTool NNetTool.h NNetTool.h
 *  
 *  v1.3
 *  @author 
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
  
  virtual double MLPm(double x1,double x2,double x3,
		      double x4,double x5,double x6,
		      double x7,double x8,double x9);
  virtual double MLPe(double x1,double x2,double x3,
		      double x4,double x5,double x6,
		      double x7,double x8,double x9);
  virtual double MLPk(double x1,double x2,double x3,
		      double x4,double x5,double x6,
		      double x7,double x8,double x9);
  virtual double MLPkS(double x1,double x2,double x3,
		       double x4,double x5,double x6,
		       double x7,double x8,double x9,
		       double x10,double x11,double x12);
  virtual double MLPpS(double x1,double x2,double x3,
		       double x4,double x5,double x6,
		       double x7,double x8,double x9,
		       double x10,double x11,double x12);
private:
  double SIGMOID(double);

};
#endif // NNETTOOL_NNETTOOL_H
