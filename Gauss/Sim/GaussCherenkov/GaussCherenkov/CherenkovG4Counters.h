// $Id: $
#ifndef CHERENKOVANALYSIS_CHERENKOVG4COUNTERS_H 
#define CHERENKOVANALYSIS_CHERENKOVG4COUNTERS_H 1

// Include files
#include <vector>
#include "G4ThreeVector.hh"


/** @class CherenkovG4Counters CherenkovG4Counters.h CherenkovAnalysis/CherenkovG4Counters.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2011-04-17
 */
class CherenkovG4Counters {
public: 


  virtual ~CherenkovG4Counters( ); ///< Destructor
  static CherenkovG4Counters* getInstance();
  void InitCherenkovG4Counters();
  void InitCherenkovG4CountersSR();
  
protected:

private:
  /// Standard constructor
  CherenkovG4Counters( ); 
  static CherenkovG4Counters* CherenkovG4CountersInstance;
  std::vector<int>m_RichPmtHitArray;
  int  m_NumPmtsRich1;
  int  m_NumPmtsRich2;
  int  m_NumPmtInModuleRich;
  int  m_NumPmtsSuperRich;
  
public:
  #include "GaussCherenkov/CherenkovG4Counters.icpp"
    
};
#endif // CHERENKOVANALYSIS_CHERENKOVG4COUNTERS_H
