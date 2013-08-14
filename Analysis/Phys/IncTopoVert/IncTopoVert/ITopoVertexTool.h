// $Id: $
#ifndef ITOPOVERTEXTOOL_H 
#define ITOPOVERTEXTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"
#include "Event/Particle.h"
#include "Event/RecVertex.h"

//
#include "IVertexFunctionTool.h"


//static const InterfaceID IID_ITopoVertexTool ( "ITopoVertexTool", 1, 0 );

/** @class ITopoVertexTool ITopoVertexTool.h ITopoVertexTool.h
 *   This class is an interface for the Topological Vertex Reconstruction.
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-09-21
 */
class GAUDI_API ITopoVertexTool : virtual public IAlgTool {

public: 

  typedef std::vector<LHCb::RecVertex*> RecVertexVector;
  
  /// Return the interface ID
  //  static const InterfaceID& interfaceID() { return IID_ITopoVertexTool; }
  DeclareInterfaceID(ITopoVertexTool, 2, 0);
  


  ///find vertices with the given list of tracks The optional n_sig_track_step vector allows to keep track of the number of signal tracks in the event at each step of the reconstruction algorithm (mainly for optimisation/debug purpose).
  virtual StatusCode findVertices(std::vector<const LHCb::Track*> & tracks, int* n_sig_track_step= NULL)=0;

  ///find vertices with the given list of tracks. The reconstruction is performed with a vertex function defined with a different track list, vTr. The optional n_sig_track_step vector allows to keep track of the number of signal tracks in the event at each step of the reconstruction algorithm (mainly for optimisation/debug purpose).
  virtual StatusCode findVertices(std::vector<const LHCb::Track*> & tracks,
                                  std::vector<const LHCb::Track*> & vTr, int* n_sig_track_step= NULL)=0;

  ///find vertices with the given list of particles The optional n_sig_track_step vector allows to keep track of the number of signal tracks in the event at each step of the reconstruction algorithm (mainly for optimisation/debug purpose).
  virtual StatusCode findVertices(std::vector<const LHCb::Particle*> & particles, int* n_sig_track_step= NULL)=0;


  ///find vertices with the given list of particles. The reconstruction is performed with a vertex function defined with a different track list, vTr. The optional n_sig_track_step vector allows to keep track of the number of signal tracks in the event at each step of the reconstruction algorithm (mainly for optimisation/debug purpose).
  virtual StatusCode findVertices(std::vector<const LHCb::Particle*> & particles,
                                  std::vector<const LHCb::Track*> & vTr, int* n_sig_track_step= NULL)=0;
  

  ///return the list of vertices found
  RecVertexVector & vertices(){return m_vertices;}

  ///set the track list used to defined the vertex function
  void setVertexFunctionTracks(std::vector<const LHCb::Track*> & vTr){ m_vertexFunction->setTracks(vTr); }

  ///get the track list used to defined the vertex function
  void getVertexFunctionTracks(std::vector<const LHCb::Track*> & vTr){ m_vertexFunction->getTracks(vTr); }

  /// get the vertex function used to perform the reconstruction
  IVertexFunctionTool* getVertexFunction(){return m_vertexFunction; }


  ///write the vertices founds on the TES
  virtual StatusCode writeOnTES(std::string location)=0;

protected:
  IVertexFunctionTool * m_vertexFunction;
  RecVertexVector m_vertices;
  
private:
};

#endif // ITOPOVERTEXTOOL_H
