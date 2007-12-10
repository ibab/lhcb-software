// $Id: RecVertexClonerWithTracks.h,v 1.1.1.1 2007-12-10 09:32:24 jpalac Exp $
#ifndef RECVERTEXCLONERWITHTRACKS_H 
#define RECVERTEXCLONERWITHTRACKS_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneRecVertex.h>            // Interface

class ICloneTrack;

/** @class RecVertexClonerWithTracks RecVertexClonerWithTracks.h
 *  
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

};
#endif // RECVERTEXCLONERWITHTRACKS_H
