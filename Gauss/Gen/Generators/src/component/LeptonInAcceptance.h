// $Id: LeptonInAcceptance.h,v 1.1 2005-11-29 15:43:17 robbep Exp $
#ifndef GENERATORS_LEPTONINACCEPTANCE_H 
#define GENERATORS_LEPTONINACCEPTANCE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Generators
#include "Generators/IFullGenEventCutTool.h"


/** @class LeptonInAcceptance LeptonInAcceptance.h 
 *  component/LeptonInAcceptance.h
 *  
 *  Cut on events with one lepton in LHCb acceptance + minimum pT
 *
 *  @author Patrick Robbe
 *  @date   2005-11-21
 */
class LeptonInAcceptance : public GaudiTool , 
                           virtual public IFullGenEventCutTool {
 public:
  /// Standard constructor
  LeptonInAcceptance( const std::string& type, const std::string& name,
                      const IInterface* parent ) ;
  
  virtual ~LeptonInAcceptance( ); ///< Destructor
  
  virtual bool studyFullEvent( EventVector & theEventVector ,
                               HardVector & theHardVector ) const ;
  
 protected:
  
 private:
  double m_thetaMax ;
  double m_ptMin ;
};
#endif // GENERATORS_LEPTONINACCEPTANCE_H
