// $Id: $
#ifndef TESTRECVERTEXHOLDER_H 
#define TESTRECVERTEXHOLDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/RecVertexHolder.h"

namespace LHCb {
  class RecVertex;
}


/** @class TestRecVertexHolder TestRecVertexHolder.h
 *  
 *  Tests for LHCb::RecVertexHolder class.
 *  Creates some dummy PVs, puts them on the TES,
 *  makes LHCb::RecVertexHolders with them, tests them.
 * 
 *  @author Juan Palacios
 *  @date   2010-12-06
 */
class TestRecVertexHolder : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TestRecVertexHolder( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestRecVertexHolder( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  void createDummyPVs();

  const LHCb::RecVertexHolder clonePV(const LHCb::RecVertex* pv) const;
  
  const LHCb::RecVertexHolder returnPV(const LHCb::RecVertex* pv) const;

  const LHCb::RecVertexHolder createPV() const;

  void testRecVertexHolder(const LHCb::RecVertexHolder pv) const;

  bool inTES(const LHCb::RecVertex* PV) const;

private:

  std::string m_dummyPVLocation;
  std::string m_clonePVLocation;
  
  

};
#endif // TESTRECVERTEXHOLDER_H
