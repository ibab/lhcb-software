// $Id: TupleToolTrackInfo.h,v 1.2 2009-03-27 17:51:36 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLTRACKINFO_H
#define JBOREL_TUPLETOOLTRACKINFO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

/** @class TupleToolTrackInfo TupleToolTrackInfo.h jborel/TupleToolTrackInfo.h
 *
 * \brief Fill track information for DecayTreeTuple
 *
 * - X_TRACK_TYPE : track type
 * - X_TRACK_CHI2 : track chi2
 * - X_TRACK_NDOF : track ndof
 * - X_TRACK_PCHI2 : track Chi2 probability
 * - X_TRACK_VeloCHI2NDOF : Track fit velo chi2/nDoF
 * - X_TRACK_TCHI2NDOF : Track fit T chi2/nDoF
 * - X_TRACK_GhostProb : Ghost probability (run NeuralNetTmva to fill it)
 * - X_TRACK_CloneDist : Only available for 2009 data
 *
 * \sa DecayTreeTuple
 *
 *  @author Stephane Poss
 *  @date   2008-03-17
 */
class TupleToolTrackInfo : public GaudiTool, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolTrackInfo( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual ~TupleToolTrackInfo(){}; ///< Destructor

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );
};

#endif // JBOREL_TUPLETOOLTRACKINFO_H
