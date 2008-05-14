#ifndef ALIGNMENTTOOLS_CENTIPEDE_H
#define ALIGNMENTTOOLS_CENTIPEDE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
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

  // Standard constructor

  virtual StatusCode InitMille( bool DOF[], 
				int nglo, 
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
			    std::vector<double> &track_params, 
			    int single_fit, 
			    std::vector<double> &estimated_para, 
			    double &chi2, 
			    double &res );

  virtual StatusCode MakeGlobalFit( std::vector<double> &, 
				    std::vector<double> &,
				    std::vector<double> & );

  virtual void CheckLChi2( const double &,
                           const int &,
                           const int &,
                           const double &,
                           const double &,
                                 bool & );



  virtual void VectortoArray( const std::vector<double> &, double * );
  virtual void ArraytoVector( const double *, std::vector<double> & );
 private:

  double *m_derGB, *m_derLC;
  double *dernl, *dernl_i;
};
#endif // ALIGNMENTTOOLS_CENTIPEDE_H

