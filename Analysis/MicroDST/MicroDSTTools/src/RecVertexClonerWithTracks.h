// $Id: RecVertexClonerWithTracks.h,v 1.2 2008-08-21 14:45:55 jpalac Exp $
#ifndef RECVERTEXCLONERWITHTRACKS_H 
#define RECVERTEXCLONERWITHTRACKS_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneRecVertex.h>            // Interface

class ICloneTrack;

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
class RecVertexClonerWithTracks : public MicroDSTTool, 
                                  virtual public ICloneRecVertex {
public: 
  /// Standard constructor
  RecVertexClonerWithTracks( const std::string& type, 
                             const std::string& name,
                             const IInterface* parent);

  virtual ~RecVertexClonerWithTracks( ); ///< Destructor

  virtual LHCb::RecVertex* operator() (const LHCb::RecVertex* vertex);

  virtual StatusCode initialize();
  
  virtual StatusCode finalize();

private:

  LHCb::RecVertex* clone (const LHCb::RecVertex* vertex);

private:

  ICloneTrack* m_trackCloner;
  std::string m_trackClonerType;
  
};
#endif // RECVERTEXCLONERWITHTRACKS_H
