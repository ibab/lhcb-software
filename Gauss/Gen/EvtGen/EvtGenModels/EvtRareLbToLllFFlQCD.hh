#ifndef EVTRARELBTOLLLFFLQCD_HH 
#define EVTRARELBTOLLLFFLQCD_HH 1

// Include files

/** @class EvtRareLbToLllFF EvtRareLbToLllFF.hh EvtGenModels/EvtRareLbToLllFF.hh
 *  
 *
 *  @author Michal Kreps
 *  @date   2014-10-23
 */

#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtIdSet.hh"
#include "EvtGenModels/EvtRareLbToLllFFBase.hh"

#include <string>
#include <map> 

class EvtRareLbToLllFFlQCD : public EvtRareLbToLllFFBase{

public: 
  
  /// Standard constructor
  EvtRareLbToLllFFlQCD( ); 

  virtual ~EvtRareLbToLllFFlQCD( ); ///< Destructor
  
  void init() ;
  
  void getFF( EvtParticle* parent, 
              EvtParticle* lambda, 
              EvtRareLbToLllFFBase::FormFactors& FF );


protected:
  
  
private:
  double formFactorParametrization(double var, double N, double X);

  double FiLQCDconsts[2][2];
  double alphaS;
  double cv;
  double cgamma;
  double csigma;
};

#endif // EVTRARELBTOLLLFF_HH
