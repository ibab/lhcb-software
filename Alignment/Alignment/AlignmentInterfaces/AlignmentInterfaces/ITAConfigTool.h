#ifndef ITACONFIGTOOL_H
#define ITACONFIGTOOL_H
//-----------------------------------------------------------------------------
/** @file TAConfigTool.h
 *
 *  Header file for Tstation alignment : ITAConfigTool
 *
 *  CVS Log :-
 *  $Id: ITAConfigTool.h,v 1.2 2007-05-11 14:41:37 jblouw Exp $
 *
 *  @author J. Blouw johan.blouw@cern.ch
 *  @date   12/04/2007
 */
//-----------------------------------------------------------------------------


#include "GaudiKernel/IAlgTool.h"

#include <string>

static const InterfaceID 
IID_ITAConfigTool("ITAConfig",0,0);


namespace LHCb {
	class Track;
	class LHCbID;
};

class ITAConfigTool : virtual public IAlgTool {
  
 public:
  // Retrieve interface ID
  static const InterfaceID & interfaceID() {
    return IID_ITAConfigTool; 
  }

  virtual StatusCode Initialize( std::vector<std::string> & ) = 0;
  virtual StatusCode FillMatrix( LHCb::Track &, 
				 LHCb::LHCbID &, 
				 const int &,
				 const double & ) = 0;
  virtual StatusCode CacheDetElements() = 0;
  virtual StatusCode ResetGlVars() = 0;
  virtual bool AlignDetector( std::string & ) = 0;
  virtual StatusCode LocalTrackFit( int &,
				    std::vector<double> &,
				    const int &,
				    std::vector<double> &,
				    double &,
				    double & ) = 0;
  virtual int NumTrPars() = 0;
  virtual Gaudi::XYZVector Residual( LHCb::Track &, 
				     LHCb::LHCbID & ) = 0;
  virtual StatusCode GlobalFit( std::vector<double> &, 
				std::vector<double> &, 
				std::vector<double> & ) = 0;
  virtual StatusCode Rank( LHCb::LHCbID &, int & ) = 0;
  virtual int NumAlignPars() = 0;
};

#endif // ITACONFIGTOOL_H
