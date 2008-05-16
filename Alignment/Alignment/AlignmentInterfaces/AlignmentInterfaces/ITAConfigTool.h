#ifndef ITACONFIGTOOL_H
#define ITACONFIGTOOL_H
//-----------------------------------------------------------------------------
/** @file TAConfigTool.h
 *
 *  Header file for Tstation alignment : ITAConfigTool
 *
 *  CVS Log :-
 *  $Id: ITAConfigTool.h,v 1.7 2008-05-16 11:03:26 jblouw Exp $
 *
 *  @author J. Blouw johan.blouw@cern.ch
 *  @date   12/04/2007
 */
//-----------------------------------------------------------------------------


#include "GaudiKernel/IAlgTool.h"
#include "Math/SMatrix.h"

#include <string>

static const InterfaceID 
IID_ITAConfigTool("ITAConfig",0,0);


namespace LHCb {
	class Track;
	class LHCbID;
	class Measurement;
        class State;
};

class ITAConfigTool : virtual public IAlgTool {
  
 public:
  // Retrieve interface ID
  static const InterfaceID & interfaceID() {
    return IID_ITAConfigTool; 
  }

  virtual StatusCode Initialize( std::vector<std::string> & ) = 0;
  virtual StatusCode ConfMatrix( unsigned int,
			   double,
			   std::vector<double> & ) = 0;

  virtual StatusCode CalcResidual( unsigned int,
			     const LHCb::Track &, 
			     const LHCb::Measurement *, 
			     const LHCb::LHCbID &,
			     bool,
			     double & ) = 0;

  virtual bool CalcResidualOT( unsigned int,
			     const LHCb::Track &, 
			     const LHCb::Measurement *, 
			     const LHCb::LHCbID &,
			     bool,
			     double & ) = 0;
  virtual void ZeroMatrix() = 0;

  virtual StatusCode FillMatrix( unsigned int,
				 double ) = 0; 
//				 , double ) = 0;
/*  virtual int InvMatrix( double [][4], 
			 std::vector<double> &, 
			 int ) = 0; */

  virtual int InvMatrix( std::vector<double> &,
			 int ) = 0;


  virtual StatusCode StoreParameters( std::vector<double> & ) = 0;
  virtual StatusCode CacheDetElements() = 0;
  virtual void ResetLVars() = 0;
  virtual void ResetGlVars() = 0;
  virtual void SetTrackPar( const std::vector<double> &, unsigned int ) = 0;
  virtual void MakeTrackParVec() = 0;
  virtual double GetLChi2() = 0;
  virtual void CheckLChi2( int, int, bool, bool & ) = 0;
  virtual void SetAlignmentPar( const std::vector<double> & ) = 0;
  virtual void SetFlag( bool, unsigned int ) = 0;

  virtual bool AlignDetector( const std::string & ) = 0;
  virtual StatusCode LocalTrackFit( unsigned int,
				    std::vector<double> &,
				    int,
				    std::vector<double> &,
				    double &,
				    double & ) = 0;
  virtual int NumTrPars() = 0;
  virtual StatusCode GlobalFit( std::vector<double> &, 
				std::vector<double> &, 
				std::vector<double> & ) = 0;
  virtual StatusCode Rank( const LHCb::LHCbID & ) = 0;
  virtual int NumAlignPars() = 0;
};

#endif // ITACONFIGTOOL_H
