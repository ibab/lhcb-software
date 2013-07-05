#ifndef COMBINETAGGERSPID_H 
#define COMBINETAGGERSPID_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"

#include "ICombineTaggersTool.h"            // Interface


/** @class CombineTaggersPID CombineTaggersPID.h CombineTaggersPID.h
 *
 *  Tool to do the combined tagging (OS and SS)
 *  @author Marc Grabalosa
 *  @date   2008-10-31
 */
class CombineTaggersPID : public GaudiTool,
                          virtual public ICombineTaggersTool {
public:
  /// Standard constructor
  CombineTaggersPID( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

  virtual ~CombineTaggersPID( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  int combineTaggers(LHCb::FlavourTag& theTag, std::vector<LHCb::Tagger*>& , int, bool );

private:

  std::vector<int> m_pid_cats_bs;
  std::vector<int> m_pid_cats_bu;
  std::vector<int> m_index;

};
#endif // COMBINETAGGERSPID_H
