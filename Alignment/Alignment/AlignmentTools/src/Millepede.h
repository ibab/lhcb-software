#ifndef ALIGNMENTTOOLS_MILLEPEDE_H 
#define ALIGNMENTTOOLS_MILLEPEDE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/MsgStream.h"

#include "IMillepede.h"            // Interface
#include "MilleConfig.h"          

#include "Event/AlignTrack.h"

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

  virtual StatusCode InitMille(MilleConfig* my_config);
  virtual StatusCode PutTrack(AlignTrack* atrack, MilleConfig* my_config, int after);
  virtual StatusCode PutPVTrack(AlignTracks* aPV, MilleConfig* my_config, int nPV);
  virtual StatusCode MakeGlobalFit(double par[], double error[], double pull[]);
  virtual StatusCode ParGlo(int index, double param);

private:

// Max. dimensions

  static const int mglobl		= 400; // Max. number of global parameters
  static const int mlocal		= 20;  // Max. number of local parameters
  static const int mcs			= 10;  // Max. number of constraint equations
  static const int mgl			= 410; // mglobl+mlocal


// Private methods 

  inline double GetResidualCut()                    {return m_residual_cut;}
  inline void   SetResidualCut(double res_cut)      {m_residual_cut = res_cut;}
  inline double GetResidualCutInit()                {return m_residual_cut_init;}
  inline void   SetResidualCutInit(double res_cut)  {m_residual_cut_init = res_cut;}
  inline int    GetTrackNumber()                      {return m_track_number;}
  inline void   SetTrackNumber(int value)             {m_track_number = value;}

  StatusCode InitUn (double cutfac);
  StatusCode ParSig(int index, double sigma);
  StatusCode ConstF(double dercs[], double rhs);
  StatusCode PrtGlo();
  StatusCode EquLoc(double dergb[], double derlc[], double rmeas, double sigma);
  StatusCode ZerLoc(double dergb[], double derlc[]);
  StatusCode FitLoc(int n, int single_fit);

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

  double   *ftx, *fty, *ftz,*frotx, *froty, *frotz; 
  double   *fscaz, *shearx, *sheary;

  double   derLC[20], *derGB;

  double corrv[mglobl], psigm[mglobl], pparm[mglobl], dparm[mglobl];
  double scdiag[mglobl], blvec[mlocal], arhs[mcs], diag[mgl], bgvec[mgl];

  int indgb[mglobl], nlnpa[mglobl], indnz[mglobl], indbk[mglobl], indlc[mlocal];
  
  bool scflag[mglobl];

  std::vector<int>     indst;
  std::vector<double>  arest;

  std::vector<int>     storeind;
  std::vector<int>     storeplace;
  std::vector<double>  storeare;

  int store_row_size;

  int Nstations;    // Number of stations
  int First_Plan;   // The first station to align 
  int Last_Plan;    // The last station to align

  int m_track_number;
  double m_residual_cut_init;
  double m_residual_cut;
  bool m_iteration;

  double cfactr, cfactref;

  int itert, nst, nfl, ncs, nstdev;
  int loctot, locrej, nagb, nalc, nrank; 

  std::vector<double>  track_params;

};
#endif // ALIGNMENTTOOLS_MILLEPEDE_H

