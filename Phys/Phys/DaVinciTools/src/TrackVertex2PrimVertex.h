// $Id: TrackVertex2PrimVertex.h,v 1.2 2006-07-20 13:19:21 jpalac Exp $
#ifndef V13R1_TRACKVERTEX2PRIMVERTEX_H 
#define V13R1_TRACKVERTEX2PRIMVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/TrackVertex.h"
#include "Event/RecVertex.h"
/** @class TrackVertex2PrimVertex TrackVertex2PrimVertex.h v13r1/TrackVertex2PrimVertex.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2006-07-06
 *
 * A simple algorithm which takes LHCb::TrackVertices from a TES locaton
 * and produces and stores LHCb::PrimVertices in the TES.
 * Properties: 
 *             InputLocation (string) : 
 *                Place in TES where TrackVertces live.
 *                Default is LHCb::TrackVertexLocation::Primary.
 *             OutputLocation (string) : 
 *                Place in TES to store PrimVertces.
 *                Default is LHCb::VertexLocation::Primary
 *
 */
class TrackVertex2PrimVertex : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TrackVertex2PrimVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackVertex2PrimVertex( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  LHCb::RecVertex* makePrimVertex(const LHCb::TrackVertex*);

private:
  std::string m_inputLocation;
  std::string m_outputLocation;
};
#endif // V13R1_TRACKVERTEX2PRIMVERTEX_H
