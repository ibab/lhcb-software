#ifndef ITACONFIGTOOL_H
#define ITACONFIGTOOL_H
//-----------------------------------------------------------------------------
/** @file TAConfigTool.h
 *
 *  Header file for Tstation alignment : ITAConfigTool
 *
 *  CVS Log :-
 *  $Id: ITAConfigTool.h,v 1.6 2008-03-04 09:24:56 jblouw Exp $
 *
 *  @author J. Blouw johan.blouw@cern.ch
 *  @date   12/04/2007
 */
//-----------------------------------------------------------------------------


#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Transform3DTypes.h"

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
  virtual StatusCode CalcResidual( const LHCb::Track &, 
				 const LHCb::Measurement *, 
				 const int &,
				 const double &,
				 double &,
				 double &,
				 double &,
				 LHCb::State & ) = 0;
  virtual StatusCode FillMatrix( const int &,
				 const double [], 
				 const double &, 
				 const double &, 
				 const double & ) = 0;
  virtual int InvMatrix( double [][4],
			 double [],
			 const int & ) = 0;
  virtual StatusCode ConfMatrix( const double &,
				 const double &,
				 const double &,
				 const double &,
				 double [],
				 double [][4] ) = 0;
  virtual StatusCode ZeroMatrVec( double [][4], double [] ) = 0;
  virtual StatusCode CacheDetElements() = 0;
  virtual StatusCode ResetGlVars() = 0;
  virtual bool AlignDetector( std::string & ) = 0;
  virtual StatusCode LocalTrackFit( int &,
				    std::vector<double> &,
				    const int &,
				    std::vector<double> &,
				    double &,
				    double & ) = 0;
  virtual StatusCode PrintParameters( std::vector<double> & ) = 0;
  virtual int NumTrPars() = 0;
  virtual StatusCode GlobalFit( std::vector<double> &, 
				std::vector<double> &, 
				std::vector<double> & ) = 0;
  virtual StatusCode Rank( LHCb::LHCbID &, int & ) = 0;
  virtual int NumAlignPars() = 0;
  virtual std::vector<Gaudi::Transform3D> GetDetMap() = 0;

};

#endif // ITACONFIGTOOL_H
