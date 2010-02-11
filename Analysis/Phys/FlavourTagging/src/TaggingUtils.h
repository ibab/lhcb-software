#ifndef USER_TAGGINGUTILS_H 
#define USER_TAGGINGUTILS_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
//#include "Kernel/IGeomDispCalculator.h"
#include <Kernel/DVAlgorithm.h>
#include <Kernel/GetDVAlgorithm.h>
#include <Kernel/IDistanceCalculator.h>
#include "Kernel/IPVReFitter.h"

#include "ITaggingUtils.h"

/** @class TaggingUtils TaggingUtils.h 
 *
 *  Tool to tag the B flavour with a Electron Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggingUtils : public GaudiTool, 
	virtual public ITaggingUtils {

public: 
  /// Standard constructor
  TaggingUtils( const std::string& type,
		const std::string& name,
		const IInterface* parent );
  virtual ~TaggingUtils( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  StatusCode calcIP( const LHCb::Particle* axp, 
                     const LHCb::VertexBase* v, 
                     double& ip, double& iperr) ;
  
  StatusCode calcIP( const LHCb::Particle*,
                     const LHCb::RecVertex::ConstVector& ,
                     double& , double& );

  int countTracks( LHCb::Particle::ConstVector& );
  bool isinTree( const LHCb::Particle*, 
                 std::vector<const LHCb::Particle*>& , double& );
  //-------------------------------------------------------------

private:

  std::string m_ChoosePV ;
  IDistanceCalculator *m_Dist;
  DVAlgorithm* m_dva;
  IPVReFitter* m_pvReFitter;

};

//===============================================================//
#endif // USER_TAGGINGUTILS_H
