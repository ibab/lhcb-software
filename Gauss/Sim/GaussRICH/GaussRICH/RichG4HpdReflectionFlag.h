// $Id: RichG4HpdReflectionFlag.h,v 1.1 2008-01-21 16:59:01 seaso Exp $
#ifndef GaussRICH_RICHG4HPDREFLECTIONFLAG_H 
#define GaussRICH_RICHG4HPDREFLECTIONFLAG_H 1

// Include files
#include "globals.hh"


/** @class RichG4HpdReflectionFlag RichG4HpdReflectionFlag.h Misc/RichG4HpdReflectionFlag.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-10-30
 */
class RichG4HpdReflectionFlag {
public: 

  static RichG4HpdReflectionFlag* RichG4HpdReflectionFlagInstance();
  

  virtual ~RichG4HpdReflectionFlag( ); ///< Destructor

  G4int HpdQwPCRefl()  {  return m_HpdQwPCRefl;}
  G4int HpdPCQwRefl()  {  return m_HpdPCQwRefl;}
  
  G4int HpdChromiumRefl(){  return m_HpdChromiumRefl;}
  
  G4int HpdAirQwRefl()  {  return m_HpdAirQwRefl; }
  
  G4int HpdAirPCRefl()  {  return m_HpdAirPCRefl;}
  
  G4int HpdSiliconRefl()  {  return m_HpdSiliconRefl;}
  
  G4int HpdKovarRefl()  {  return m_HpdKovarRefl;}
  
  G4int HpdKaptonRefl()  {  return m_HpdKaptonRefl;}
  
  G4int MaxNumHpdReflFlag()  {return m_MaxNumHpdReflFlag;}
  
    
  
protected:

private:
  RichG4HpdReflectionFlag( ); 

  static RichG4HpdReflectionFlag* m_RichG4HpdReflectionFlagInstance;
  G4int m_HpdQwPCRefl;
  G4int m_HpdPCQwRefl;
  G4int m_HpdChromiumRefl;
  G4int m_HpdAirQwRefl;
  G4int m_HpdAirPCRefl;
  G4int m_HpdSiliconRefl;
  G4int m_HpdKovarRefl;
  G4int m_HpdKaptonRefl;
  G4int m_MaxNumHpdReflFlag;
  
  
};
#endif // GaussRICH_RICHG4HPDREFLECTIONFLAG_H
