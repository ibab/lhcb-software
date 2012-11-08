// $Id: NeutralPP2MC.h,v 1.9 2008-09-25 12:00:54 odescham Exp $
#ifndef NEUTRALPP2MC_H 
#define NEUTRALPP2MC_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "AsctAlgorithm.h"


/** @class NeutralPP2MC NeutralPP2MC.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-09-25
 */
class NeutralPP2MC  : public AsctAlgorithm
{
  // friend factory for instantiation 
  friend class AlgFactory<NeutralPP2MC>;
public:
  /** standard execution of the algorithm
   *  @return StatusCode
   */
  virtual StatusCode execute    () ;
protected:  
  /** Standard constructor
   *  @param name name of the algorithm
   *  @param svc  service locator
   */
  NeutralPP2MC( const std::string& name , ISvcLocator* svc );
  virtual ~NeutralPP2MC(){};
  virtual StatusCode initialize();
private:
  NeutralPP2MC();
  NeutralPP2MC( const NeutralPP2MC& );
  NeutralPP2MC& operator=( const NeutralPP2MC& );
private:
  std::string m_mcTable ;
  std::string m_test;

};
#endif // NEUTRALPP2MC_H
