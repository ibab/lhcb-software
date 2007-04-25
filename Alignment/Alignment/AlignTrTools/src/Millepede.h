#ifndef ALIGNTRTOOLS_MILLEPEDE_H 
#define ALIGNTRTOOLS_MILLEPEDE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/MsgStream.h"

// from AlignmentInterfaces
#include "AlignmentInterfaces/IMillepede.h"    

//local
#include "debugoptions.h"

/** @class Millepede Millepede.h AlignmentTools/Millepede.h
 *  
 *
 *  @author Marc Deissenroth
 *  @date   2007-02-09
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

  //MD InitMille changed !
   StatusCode InitMille(bool DOF[], int nglo
		 , int nloc, double startfact, int nstd 
		 , double res_cut, double res_cut_init);
   virtual StatusCode MakeGlobalFit(double par[], double error[], double pull[]);
   virtual StatusCode ParGlo(int index, double param);
   virtual StatusCode ParSig(int index, double sigma);
   virtual StatusCode ConstF(double dercs[], double rhs);
   virtual StatusCode EquLoc(double dergb[], double derlc[], double rmeas, double sigma);
   virtual StatusCode ZerLoc(double dergb[], double derlc[]);
   virtual StatusCode FitLoc(int n, double track_params[], int single_fit, 
			     double estimated_para[], double &chi2, double &res);
   virtual int  GetTrackNumber();
   virtual void SetTrackNumber(int value);
   virtual const std::vector<double> GetLocalResiduals() const;
   virtual const std::vector<double> GetLocalResidualsIndex() const;
   virtual const std::vector<double> GetTrackMeasGuess() const;
   virtual const void ClearTrackMeasGuess();
   virtual const double GetGlobalMatrix(int row, int column);
   virtual const double GetGlobalMatrixInv(int row, int column);


private:


// Max. dimensions

  static const int mglobl		= 400; // Max. number of global parameters
  static const int mlocal		= 20;  // Max. number of local parameters
  static const int mcs			= 10;  // Max. number of constraint equations
  static const int mgl			= 410; // mglobl+mlocal


// Private methods 

  bool InitUn (double cutfac);
  bool PrtGlo();

  double ErrPar(int i);
  double CorPar(int i, int j);
  //  int  SpmInvGlobal(double v[][mgl], double b[], const int n, double diag[], bool flag[]); //MD
  int  SpmInvGlobal(double v[][mgl], double b[], const int n); //MD
  //  int  SpmInv(double v[][mlocal], double b[], int n, double diag[], bool flag[]); //MD 15-11
  int  SpmInv(double v[][mlocal], double b[], int n);
  bool SpAVAt(double v[][mlocal], double a[][mlocal], double w[][mglobl], int n, int m);
  bool SpAX(double a[][mlocal], double x[], double y[], int n, int m);
  double chindl(int n, int nd);

// Matrices

  double cgmat[mgl][mgl];
  double clmat[mlocal][mlocal];
  double clcmat[mglobl][mlocal];
  double corrm[mglobl][mglobl];
  double adercs[mcs][mglobl];


// Vectors and useful variables

  double corrv[mglobl], psigm[mglobl], pparm[mglobl], dparm[mglobl];
  double blvec[mlocal], arhs[mcs], m_diag[mgl], bgvec[mgl];// scdiag[mglobl],

  int indgb[mglobl], indnz[mglobl], indbk[mglobl], indlc[mlocal];// nlnpa[mglobl],
  double m_glmat[mgl][mgl];  
  double m_glmatinv[mgl][mgl];  
  //  bool scflag[mglobl];

  std::vector<int>     indst;
  std::vector<double>  arest;
  std::vector<double>  arenl;

  std::vector<int>     storeind;
  std::vector<int>     storeplace;
  std::vector<double>  storeare;
  std::vector<double>  storenl;
  std::vector<double>  m_storeslopes; //MD


  int store_row_size;

  int m_track_number;
  double m_residual_cut_init;
  double m_residual_cut;

  double cfactr, cfactref;

  int itert, nst, nfl, ncs, nstdev;
  int loctot, locrej, nagb, nalc;//MD , nrank; 

  //MD
  int m_NofPlanes;
  int m_NofDOFs;
  double m_blvec[mlocal];
  
  double m_Xiter[100],m_Yiter[100],m_Ziter[100];
  double m_Xiter_err[100],m_Yiter_err[100],m_Ziter_err[100];
  
  std::vector< std::vector<double> > m_iteration_vector;
  std::vector< std::vector<double> > m_iterationError_vector;

  std::vector< double > m_res_value; // local fit residuals
  std::vector< double > m_res_index; // local fit residuals
  std::vector< double > m_rmeas_guess; // MD 3/1/07
  int m_res_counter;  // local fit residuals counter
  int m_res_trackcounter;

};
#endif // ALIGNMENTTOOLS_MILLEPEDE_H

