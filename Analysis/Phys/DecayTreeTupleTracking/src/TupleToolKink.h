#ifndef TUPLETOOLKINK_H
#define TUPLETOOLKINK_H 1

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackFitter.h"
/** @class TupleToolKink TupleToolKink.h 
 *  \brief TupleTool to reconstruct and save information about kinks on given tracks
 *
 *  Two methods are provided: 
 *   - KinkChi2 
 *   - KinkFitChi2
 *  The following variables are stored:
 *  - _TRACK_kinkChi2, _TRACK_kinkChi2z
 *  - _TRACK_kinkFitChi2, _TRACK_kinkFitDeltaR, _TRACK_kinkFitDeltaRE, _TRACK_kinkFitDeltaRZ
 *
 *  @author Francesco Dettori
 *  @date   2013-23-02
 */

class TupleToolKink : public TupleToolBase, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolKink( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual ~TupleToolKink(); ///< Destructor
  virtual  StatusCode initialize();
  

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );

  StatusCode kinkChi2( const LHCb::Track& track, double &chi2kink, 
                       double &zkink) const;

  StatusCode fitKink(const LHCb::Track &track,
                     double &kink_deltaR, double &kink_deltaR_err, 
                     double &kink_chi2,
                     double &kink_z);
  
  
private: 
  
  ToolHandle<ITrackFitter> m_trackfitter ;
  
};

#endif // TUPLETOOLKINK_H
