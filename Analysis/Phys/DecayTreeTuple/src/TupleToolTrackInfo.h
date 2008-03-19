// $Id: TupleToolTrackInfo.h,v 1.1 2008-03-19 20:26:28 sposs Exp $
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
 * - head_TRACK_CHI2 : track chi2
 * - head_TRACK_NDOF : track ndof
 *
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
