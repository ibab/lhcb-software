// $Id: RichG4StepAnalysis4.h,v 1.2 2004-02-10 14:24:17 jonesc Exp $
#ifndef RICHANALYSIS_RICHG4STEPANALYSIS4_H
#define RICHANALYSIS_RICHG4STEPANALYSIS4_H 1

// Include files
/// GiGa
#include "GiGa/GiGaStepActionBase.h"
template <class TYPE> class GiGaFactory;
class G4Step;

/** @class RichG4StepAnalysis4 RichG4StepAnalysis4.h RichAnalysis/RichG4StepAnalysis4.h
 *
 *
 *  @author Sajan EASO
 *  @date   2003-10-29
 */

class RichG4StepAnalysis4:virtual public  GiGaStepActionBase {
  friend class GiGaFactory<RichG4StepAnalysis4>;

public:
  /// Standard constructor
  RichG4StepAnalysis4
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;

  virtual ~RichG4StepAnalysis4( ); ///< Destructor
  virtual void UserSteppingAction( const G4Step* aStep );

protected:

private:
  RichG4StepAnalysis4();
  RichG4StepAnalysis4(const  RichG4StepAnalysis4& );
  RichG4StepAnalysis4& operator=(const  RichG4StepAnalysis4& );


};
#endif // RICHANALYSIS_RICHG4STEPANALYSIS4_H
