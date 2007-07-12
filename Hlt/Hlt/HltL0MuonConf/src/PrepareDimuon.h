// $Id: PrepareDimuon.h,v 1.1 2007-07-12 17:52:36 asatta Exp $
#ifndef PREPAREDIMUON_H 
#define PREPAREDIMUON_H 1

// Include files
// from Gaudi

#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"


/** @class PrepareDimuon PrepareDimuon.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-06-06
 */
class PrepareDimuon : public HltAlgorithm {
public: 
  /// Standard constructor
  PrepareDimuon( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrepareDimuon( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string  m_outputL0MuonVerticesName;
  PatVertexContainer* m_patVertexBank;

};
#endif // PREPAREDIMUON_H
