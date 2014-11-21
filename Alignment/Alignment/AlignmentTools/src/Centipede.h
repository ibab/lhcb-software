#ifndef ALIGNMENTTOOLS_CENTIPEDE_H
#define ALIGNMENTTOOLS_CENTIPEDE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/GaudiTupleTool.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/MsgStream.h"

// from Alignment
#include "AlignmentInterfaces/ICentipede.h"    
#include "AlignmentInterfaces/IMillepede.h"

// local includes
#include "Millepede.h"
/** @class Centipede Centipede.h AlignmentTools/Centipede.h
 *  
 *
 *  @author Johan Blouw
 *  @date   2007-08-14
 * 
 * This is a helper class which enherits from the Millepede class.
 * It is used to provide the Heidelberg alignment algorithm with
 * a unified interface to Millepede.
 * 
 */

//class Singlepede;

class Centipede : public Millepede, virtual public ICentipede {
 public:

  // Standard constructor
  Centipede ( const std::string& type, 
              const std::string& name,
              const IInterface* parent);


  virtual ~Centipede(); 

/*   virtual StatusCode initialize(); */
//  virtual StatusCode finalize(); 
  
  int  SpmInvGlobal(double v[][mgl], double b[], const int n); //MD
  int  SpmInv(double v[][mlocal], double b[], int n);//MD

  using Millepede::InitMille;
  using Millepede::EquLoc;
  using Millepede::ZerLoc;
  using Millepede::FitLoc;
  using Millepede::MakeGlobalFit;
  using Millepede::ConstF;
  
  virtual StatusCode InitMille( int nglo, 
                                int nloc, 
                                double startfac, 
                                int nstd, 
                                double res_cut, 
                                double res_cut_init );
  virtual StatusCode EquLoc ( std::vector<double> &dergb,
                              std::vector<double> &derlc,
                              const double &rmeas, 
                              double sigma);
  virtual StatusCode ZerLoc( std::vector<double> &dergb,
                             std::vector<double> &derlc );
	virtual StatusCode FitLoc(int n, 
                            int locrank,
                            std::vector< std::vector< double> >  locVec,
                            std::vector<double> &track_params, 
                            int single_fit, 
                            std::vector<double> &estimated_para, 
                            double &chi2, 
                            double &res );
  virtual StatusCode MakeGlobalFit( std::vector<double> &, 
                                    std::vector<double> &);
  virtual StatusCode ConstF(std::vector<double> dercs, double rhs);
  virtual void VectortoArray( const std::vector<double> &, double * );
  virtual void ArraytoVector( const double *, std::vector<double> & );

  virtual void CheckLChi2(double , int, int, double , double  , bool & );
  

 private:

  bool DEBUGGLOBAL;
  bool DEBUGLOCAL;

  double *m_derGB, *m_derLC;
  double *m_dernl, *m_dernl_i;

  double test;

  
};
#endif // ALIGNMENTTOOLS_CENTIPEDE_H

