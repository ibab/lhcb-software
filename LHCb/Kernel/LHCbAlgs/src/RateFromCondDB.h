#ifndef RATEFROMTCK_H 
#define RATEFROMTCK_H 1

// CondDB
#include "DetDesc/Condition.h"
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

//#include "Kernel/IPropertyConfigSvc.h"
#include "Kernel/TCK.h"
#include "Kernel/IRateFromTCK.h"
static const InterfaceID IID_RateFromCondDB ( "RateFromCondDB", 1, 0 ); // needed as there's no interface
/** @class RateFromCondDB RateFromCondDB.h
 *  
 *  Get rate of a given rate limiter for the Condition Database
 *
 *  @sa OfflineRateLimiter
 *
 * @todo This could implement the same interface as @sa RateFromTCK
 *
 *  @author Patrick Koppenburg
 *  @date   2012-05-09
 */
class RateFromCondDB : public GaudiTool
                       //, virtual public IRateFromCondDB 
{

public: 
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_RateFromCondDB; }; // needed as there's no interface
  /// Standard constructor
  RateFromCondDB( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~RateFromCondDB( ){}; ///< Destructor

  bool initializeCondDB(); 

  /// Get rate from Conditions Database
  double getRate() const ;

protected:

private:
  StatusCode i_updateConditions();              ///< Function extracting data from Condition

  //  mutable IPropertyConfigSvc * m_propertyConfigSvc;
  //  std::string m_propertyConfigSvcName;      ///< Name of PropertyConfigSvc
  Condition *m_condTrigger;                     ///< Condition for sampling coefficients
  double m_rate;                                ///< rate in condDB
  std::string m_condition ;                     ///< ConditionName
  std::string m_lineRate ;                      ///< Line Rate name in CondDB
  double m_unit ;                               ///< Unit for rate
};
#endif // RATEFROMTCK_H
