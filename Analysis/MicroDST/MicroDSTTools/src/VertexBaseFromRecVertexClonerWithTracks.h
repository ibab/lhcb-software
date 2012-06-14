
#ifndef VertexBaseFromRecVertexClonerWithTracks_H
#define VertexBaseFromRecVertexClonerWithTracks_H 1

// local
#include "VertexBaseFromRecVertexCloner.h"

class ICloneTrack;

/** @class VertexBaseFromRecVertexClonerWithTracks VertexBaseFromRecVertexClonerWithTracks.h
 *
 *  MicroDSTTool that clones an LHCb::RecVertex for storage on the MicroDST.
 *  The LHCb::RecVertex's constituent LHCb::Tracks are also cloned for storage.
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class VertexBaseFromRecVertexClonerWithTracks : public VertexBaseFromRecVertexCloner
{

public:

  /// Standard constructor
  VertexBaseFromRecVertexClonerWithTracks( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent);

  virtual ~VertexBaseFromRecVertexClonerWithTracks( ); ///< Destructor

  virtual StatusCode initialize();

protected:

  virtual LHCb::RecVertex* clone (const LHCb::RecVertex* vertex);

private:

  ICloneTrack* m_trackCloner;
  std::string m_trackClonerType;

};

#endif // VertexBaseFromRecVertexClonerWithTracks_H
