#ifndef ALIGNMENTINTERFACES_ICENTIPEDE_H
#define ALIGNMENTINTERFACES_ICENTIPEDE_H 1

// Include files
// from Gaudi:

#include "AlignmentInterfaces/IMillepede.h"

/** @class ICentipede ICentipede.h AlignmentInterfaces/ICentipede.h
 *
 * @author Johan Blouw, Physikalisches Institut, Uni Heidelberg
 * @date 2007-08-17
 *
 * This is a helper class which enherits from the Millepede class.
 * It is used to provide the Heidelberg alignment algorithm with
 * a unified interface to Millepede.
 */

static const InterfaceID IID_ICentipede ( "ICentipede", 1, 0 );

class ICentipede : virtual public IMillepede {
 public:
  // Retrieve Interface ID
  static const InterfaceID& interfaceID() { return IID_ICentipede;};

  /// Virtual destructor
  virtual ~ICentipede() {}

  //  virtual StatusCode initialize() = 0;
  //virtual StatusCode finalize() = 0;
  
  using IMillepede::InitMille;
  using IMillepede::EquLoc;
  using IMillepede::ZerLoc;
  using IMillepede::FitLoc;
  using IMillepede::MakeGlobalFit;
  using IMillepede::ConstF;

  virtual StatusCode InitMille (
                                int nglo,
                                int nloc,
                                double startfact,
                                int nstd,
                                double res_cut,
                                double res_cut_init ) = 0;
  
  virtual StatusCode EquLoc ( std::vector<double> &dergb,
			      std::vector<double> &derlc,
			      const double &rmeas,
			      double sigma ) = 0;
  virtual StatusCode ZerLoc( std::vector<double> &dergb,
			     std::vector<double> &derlc ) = 0;

  virtual StatusCode FitLoc ( int n,
                              int locrank,
                              std::vector< std::vector< double> >  locVec,
                              std::vector<double> &track_params,
                              int single_fit,
                              std::vector<double> &estimated_para,
                              double &summ,
                              double &res ) = 0;
  virtual StatusCode MakeGlobalFit( std::vector<double> &,
                                    std::vector<double> &) = 0;
  virtual StatusCode ConstF(std::vector<double> dercs, double rhs) = 0 ;
  virtual void CheckLChi2(double , int, int, double , double  , bool & ) = 0;
};
#endif // ALIGNMENTINTERFACES_ICENTIPEDE_H 
