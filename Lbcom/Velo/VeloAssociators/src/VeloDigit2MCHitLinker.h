// $Id: VeloDigit2MCHitLinker.h,v 1.1 2006-03-01 14:11:14 szumlat Exp $
#ifndef VELODIGIT2MCHITLINKER_H 
#define VELODIGIT2MCHITLINKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class VeloDigits2MCHitsLinker VeloDigits2MCHitsLinker.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-02-18
 */
class VeloDigit2MCHitLinker : public GaudiAlgorithm {

public: 
  /// Standard constructor
  VeloDigit2MCHitLinker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloDigit2MCHitLinker( ); ///< Destructor
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

 std::string asctName();

private:

  std::string m_asctName;
  std::string m_inputDigits;
  std::string m_inputFEs;

};
//
inline std::string VeloDigit2MCHitLinker::asctName()
{
  return ( m_asctName );
}
#endif // VELODIGIT2MCHITLINKER_H
