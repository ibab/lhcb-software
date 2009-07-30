// $Id: RecVertexClonerWithTracks.h,v 1.5 2009-07-30 10:03:01 jpalac Exp $
#ifndef RECVERTEXCLONERWITHTRACKS_H 
#define RECVERTEXCLONERWITHTRACKS_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneVertexBase.h>            // Interface

class ICloneTrack;
namespace LHCb 
{
  class RecVertex;
}

/** @class RecVertexClonerWithTracks RecVertexClonerWithTracks.h
 *  
 *  MicroDSTTool that clones an LHCb::RecVertex and its consituent
 *  LHCb::Tracks for storage on the MicroDST.
 *  The LHCb::Track cloning is performed by an implementation of the
 *  ICloneTrack interface. This is configurable via the ICloneTrack
 *  property, with default value "TrackCloner".
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class RecVertexClonerWithTracks : public extends1<MicroDSTTool, ICloneVertexBase> {
public: 
  /// Standard constructor
  RecVertexClonerWithTracks( const std::string& type, 
                             const std::string& name,
                             const IInterface* parent);

  virtual ~RecVertexClonerWithTracks( ); ///< Destructor

  virtual LHCb::VertexBase* operator() (const LHCb::VertexBase* vertex);



  virtual StatusCode initialize();
  
  virtual StatusCode finalize();

private:

  virtual LHCb::RecVertex* operator() (const LHCb::RecVertex* vertex);

  LHCb::RecVertex* clone (const LHCb::RecVertex* vertex);

private:

  ICloneTrack* m_trackCloner;
  std::string m_trackClonerType;
  
};
#endif // RECVERTEXCLONERWITHTRACKS_H
