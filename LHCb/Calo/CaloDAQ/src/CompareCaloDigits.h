// $Id: CompareCaloDigits.h,v 1.1.1.1 2005-01-11 07:51:47 ocallot Exp $
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
  void compareContainers ( CaloDigits* dig1,
                           CaloDigits* dig2,
                           double tol);
  
private:
  std::string m_extension;
};
#endif // COMPONENT_COMPARECALODIGITS_H
