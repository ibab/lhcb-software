// $Id: CompareCaloDigits.h,v 1.3 2005-12-20 13:35:28 ocallot Exp $
#ifndef COMPONENT_COMPARECALODIGITS_H 
#define COMPONENT_COMPARECALODIGITS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/CaloDigit.h"

/** @class CompareCaloDigits CompareCaloDigits.h component/CompareCaloDigits.h
 *  Compare the containers of CaloDigits
 *
 *  @author Olivier Callot
 *  @date   2003-11-18
 */
class CompareCaloDigits : public GaudiAlgorithm {
public:
  /// Standard constructor
  CompareCaloDigits( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CompareCaloDigits( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:
  void compareContainers ( LHCb::CaloDigits* dig1,
                           LHCb::CaloDigits* dig2,
                           double tol);
  
private:
  std::string m_extension;
  bool m_packedRawBuffer;
};
#endif // COMPONENT_COMPARECALODIGITS_H
