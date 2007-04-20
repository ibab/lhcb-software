#ifndef ALIGNMENTINTERFACES_IMILLEPEDE_H 
#define ALIGNMENTINTERFACES_IMILLEPEDE_H 1

// Include files
#include <vector>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/** @class IMillepede IMillepede.h AlignmentTools/IMillepede.h
 * 
 *
 *  @author Sebastien Viret
 *  @date   2005-07-29
 */

//--------------------------------------------------------------
// This is the interface to the Millepede tool 
//--------------------------------------------------------------

static const InterfaceID IID_IMillepede ( "IMillepede", 1, 0 );

class IMillepede : virtual public IAlgTool {
public: 
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {return IID_IMillepede;}

  virtual StatusCode InitMille( bool DOF[], int nglo
				    , int nloc, double startfact, int nstd 
				    , double res_cut, double res_cut_init) = 0;
  virtual StatusCode MakeGlobalFit(double par[], double error[], double pull[]) = 0;
  virtual StatusCode ParGlo(int index, double param) = 0;
  virtual StatusCode ParSig(int index, double sigma) = 0;
  virtual StatusCode ConstF(double dercs[], double rhs) = 0;
  virtual StatusCode EquLoc(double dergb[], double derlc[], double rmeas, double sigma) = 0;
  virtual StatusCode ZerLoc(double dergb[], double derlc[]) = 0;
  virtual StatusCode FitLoc(int n, double track_params[], int single_fit, 
			    double estimated_para[], double &, double &) = 0;
  virtual int  GetTrackNumber() = 0;
  virtual void SetTrackNumber(int value) = 0;
  virtual const std::vector<double> GetLocalResiduals() const = 0;
  virtual const std::vector<double> GetLocalResidualsIndex() const = 0;
  virtual const std::vector<double> GetTrackMeasGuess() const = 0;
  virtual const void ClearTrackMeasGuess()  = 0;
  virtual const double GetGlobalMatrix(int row, int column) = 0;
  virtual const double GetGlobalMatrixInv(int row, int column) = 0;


};
#endif // ALIGNMENTTOOLS_IMILLEPEDE_H
