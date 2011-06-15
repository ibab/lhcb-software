// $Id: RichG4StepAnalysis3.h,v 1.2 2004-02-10 14:24:16 jonesc Exp $
#ifndef RICHANALYSIS_RICHG4STEPANALYSIS3_H
#define RICHANALYSIS_RICHG4STEPANALYSIS3_H 1

// Include files

/** @class RichG4StepAnalysis3 RichG4StepAnalysis3.h RichAnalysis/RichG4StepAnalysis3.h
 *
 *
 *  @author Sajan EASO
 *  @date   2003-06-07
 */
/// GiGa
#include "GiGa/GiGaStepActionBase.h"

template <class TYPE> class GiGaFactory;
class G4Step;

class RichG4StepAnalysis3 : virtual public  GiGaStepActionBase {
  friend class GiGaFactory<RichG4StepAnalysis3>;

public:

  /// Standard constructor
  RichG4StepAnalysis3 ( const std::string& type   ,
                        const std::string& name   ,
                        const IInterface*  parent ) ;

  virtual ~RichG4StepAnalysis3( ); ///< Destructor

  virtual void UserSteppingAction( const G4Step* aStep );

  G4double RichG4AgelPlanePosTolerence() const
  {
    return  m_RichG4AgelPlanePosTolerence;
  }
  G4double RichG4AgelPhotonDirTolerence() const
  {
    return m_RichG4AgelPhotonDirTolerence;
  }

  G4bool RichG4AgelPhotRad(G4int aRadiatorNum);
  G4bool RichG4Rich1GasPhotRad(  G4int aRadiatorNumber);

protected:

private:

  RichG4StepAnalysis3();
  RichG4StepAnalysis3(const  RichG4StepAnalysis3& );
  RichG4StepAnalysis3& operator=(const  RichG4StepAnalysis3&);

private:
  G4double   m_RichG4AgelPlanePosTolerence;
  G4double    m_RichG4AgelPhotonDirTolerence;


};
#endif // RICHANALYSIS_RICHG4STEPANALYSIS3_H
