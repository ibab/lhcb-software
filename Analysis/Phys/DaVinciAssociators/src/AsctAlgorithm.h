// $Id: AsctAlgorithm.h,v 1.3 2004-06-11 15:26:15 phicharp Exp $
#ifndef ASCTALGORITHM_H 
#define ASCTALGORITHM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class AsctAlgorithm AsctAlgorithm.h
 *  
 *  Base class for algorithms creating Association tables
 *  @author Philippe Charpentier
 *  @date   2002-07-16
 */

class AsctAlgorithm : public GaudiAlgorithm {
public:
  /// Standard constructor
  AsctAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~AsctAlgorithm( ); ///< Destructor

  /// Location of table in TES
  std::string outputTable() { return m_outputTable; };

protected:

  std::vector<std::string> m_inputData;  ///< location of input data in the TES
  std::string m_outputTable; ///< location of relations table
};

//=============================================================================
#endif // ASCTALGORITHM_H
