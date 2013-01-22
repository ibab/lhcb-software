// $Id: TupleToolTrackPosition.h,v 1.2 2010-01-26 15:48:57 rlambert Exp $
#ifndef TUPLETOOLTRACKPOSITION_H 
#define TUPLETOOLTRACKPOSITION_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
//#include "Kernel/DVAlgorithm.h"
#include "Kernel/IParticleTupleTool.h" 
#include "DecayTreeTupleBase/TupleToolBase.h"
//#include "TrackInterfaces/ITrackExtrapolator.h"

/** @class TupleToolTrackPosition TupleToolTrackPosition.h
 *  
 * \brief Plot the X/Y position at a given Z
 * 
 * Steer with:
 * - Z : the Z-position at which to plot the X/Y, double, defaults to 2500 (TT-stations)
 * - Extrapolator : which track extrapolator to use, defaults to the TrackStateProvider
 * - ExtraName : the universal TupleTool option, in case you want to use multiples of this tool
 * 
 * fills:
 * - head_X
 * - head_Y
 * 
 * \sa DecayTreeTuple
 * 
 *  @author Rob Lambert
 *  @date   2011-01-20
 */

class ITrackExtrapolator;
class ITrackStateProvider;


class TupleToolTrackPosition : public TupleToolBase, virtual public IParticleTupleTool {
public: 
  /// Standard constructor
  TupleToolTrackPosition( const std::string& type, 
              const std::string& name,
              const IInterface* parent );
  /// Loop over differnt conesizes and fill the variables into the tuple
  virtual StatusCode fill( const LHCb::Particle*
                           , const LHCb::Particle*
                           , const std::string&
                           , Tuples::Tuple& );
  
  

  virtual ~TupleToolTrackPosition( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  

protected:

private:

  double m_Z;   ///<Where to propagate the track
  const ITrackStateProvider* m_extrapolator; ///<pointer tot the track extrapolator
  std::string m_extrapolatorName;///<which extrapolator to use
  std::string m_morePrefix;///< More prefix
  
  

};



#endif // TUPLETOOLTRACKPOSITION_H

