// $Id: RichG4InputMon.h,v 1.1 2005-08-25 12:20:51 seaso Exp $
#ifndef RICHANALYSIS_RICHG4INPUTMON_H 
#define RICHANALYSIS_RICHG4INPUTMON_H 1

// Include files

/** @class RichG4InputMon RichG4InputMon.h RichAnalysis/RichG4InputMon.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2005-08-19
 */
class RichG4InputMon {
public: 
  /// Standard constructor
  RichG4InputMon( ); 

  virtual ~RichG4InputMon( ); ///< Destructor

  bool FirstMonInstance() 
  {    return m_FirstMonInstance;}
  void setFirstMonInstance(bool ava) 
  {    m_FirstMonInstance= ava;}
  void MonitorRich1GasRadiatorRefIndex();
  
protected:

private:
  bool m_FirstMonInstance;
  
};
#endif // RICHANALYSIS_RICHG4INPUTMON_H
