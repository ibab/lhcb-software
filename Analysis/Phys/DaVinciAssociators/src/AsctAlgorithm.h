// $Id: AsctAlgorithm.h,v 1.6 2006-06-23 14:54:38 phicharp Exp $
#ifndef ASCTALGORITHM_H 
#define ASCTALGORITHM_H 1

// Include files
// from STL
#include <string>
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/KeyedObject.h"

/** @class AsctAlgorithm AsctAlgorithm.h
 *  
 *  Base class for algorithms creating Association tables
 *  @author Philippe Charpentier
 *  @date   2002-07-16
 */

class AsctAlgorithm : public GaudiAlgorithm {
public:
  /// Standard constructor
  AsctAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
    : GaudiAlgorithm ( name , pSvcLocator ) {
  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputTable", m_outputTable = "" );
};

  virtual ~AsctAlgorithm( ) {}; ///< Destructor

  /// Location of table in TES
  std::string outputTable() { return m_outputTable; };

protected:

  std::vector<std::string> m_inputData;  ///< location of input data in the TES
  std::string m_outputTable; ///< location of relations table
  inline std::string 
  objectName( const KeyedObject<int>* obj) const
  {
    if( !obj->hasKey() || NULL == obj->parent() || 
        NULL == obj->parent()->registry()) return "noKey";
    return 
      obj->parent()->registry()->identifier()+
      "/"+boost::lexical_cast<std::string>(obj->key());
  }
};

//=============================================================================
#endif // ASCTALGORITHM_H
