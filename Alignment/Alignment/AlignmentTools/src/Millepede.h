#ifndef ALIGNMENTTOOLS_MILLEPEDE_H 
#define ALIGNMENTTOOLS_MILLEPEDE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/MsgStream.h"

// from Alignment
#include "AlignmentTools/IMillepede.h"    

/** @class Millepede Millepede.h AlignmentTools/Millepede.h
 *  
 *
 *  @author Sebastien Viret
 *  @date   2005-07-29
 */
class Millepede : public GaudiTool, virtual public IMillepede {
public: 
  /// Standard constructor
  Millepede( const std::string& type, 
             const std::string& name,
             const IInterface* parent);

  virtual ~Millepede( ); ///< Destructor

  /// Initialization
  virtual StatusCode initialize();

  virtual StatusCode InitMille(bool DOF[], double Sigm[], int nglo
			       , int nloc, double startfact, int nstd 
			       , double res_cut, double res_cut_init, int n_fits);
  virtual StatusCode MakeGlobalFit(double par[], double error[], double pull[]);
  virtual StatusCode GetAlignmentConstants(double par[]);
  virtual StatusCode ParGlo(int index, double param);
  virtual StatusCode ParSig(int index, double sigma);
  virtual StatusCode ConstF(double dercs[], double rhs);
  virtual StatusCode EquLoc(double dergb[], double derlc[], double dernl[], double dernl_i[],
			    double rmeas, double sigma);
  virtual StatusCode ZerLoc(double dergb[], double derlc[], double dernl[], double dernl_i[]);
  virtual StatusCode FitLoc(int n, double track_params[], int single_fit);
  virtual StatusCode FitLoc(int n, double track_params[], int single_fit, 
			    double estimated_para[], double &chi2, double &res);

  virtual int  GetTrackNumber();
  virtual void SetTrackNumber(int value);

private:

// Max. dimensions

  static const int mglobl		= 400; // Max. number of global parameters
  static const int mlocal		= 20;  // Max. number of local parameters
  static const int mcs			= 10;  // Max. number of constraint equations
  static const int mgl			= 410; // mglobl+mlocal

  static const int nonlin_param		= 1000000; // For non-linear terms treatment
                                                   // See how it works in EquLoc() and MakeGlobalFit() 
                                                   // , Should be much larger than any of the derivatives


  static const int mobjects		= 100000; // Max. number of objects to align

// Private methods 

  StatusCode InitUn (double cutfac);
  StatusCode PrtGlo();

  double ErrPar(int i);
  double CorPar(int i, int j);
  int  SpmInv(double v[][mgl], double b[], int n, double diag[], bool flag[]);
  int  SpmInv(double v[][mlocal], double b[], int n, double diag[], bool flag[]);
  StatusCode SpAVAt(double v[][mlocal], double a[][mlocal], double w[][mglobl], int n, int m);
  StatusCode SpAX(double a[][mlocal], double x[], double y[], int n, int m);
  double chindl(int n, int nd);

// Matrices

  double cgmat[mgl][mgl];
  double clmat[mlocal][mlocal];
  double clcmat[mglobl][mlocal];
  double corrm[mglobl][mglobl];
  double adercs[mcs][mglobl];


// Vectors and useful variables

  double corrv[mglobl], psigm[mglobl], pparm[mglobl], dparm[mglobl];
  double scdiag[mglobl], blvec[mlocal], arhs[mcs], diag[mgl], bgvec[mgl];

  int indgb[mglobl], nlnpa[mglobl], indnz[mglobl], indbk[mglobl], indlc[mlocal];
  
  bool scflag[mglobl];

  std::vector<int>     indst;
  std::vector<double>  arest;
  std::vector<double>  arenl;

  std::vector<int>     storeind;
  std::vector<int>     storeplace;
  std::vector<double>  storeare;
  std::vector<double>  storenl;

  std::vector<double>  m_par;

  int store_row_size;

  int m_track_number;
  double m_residual_cut_init;
  double m_residual_cut;
  bool m_iteration;
  int m_fixed;

  double cfactr, cfactref;

  int itert, nst, nfl, ncs, nstdev;
  int loctot, locrej, nagb, nalc, nrank; 

};
#endif // ALIGNMENTTOOLS_MILLEPEDE_H

