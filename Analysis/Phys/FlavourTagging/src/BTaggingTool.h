// $Id: BTaggingTool.h,v 1.22 2010-02-02 19:29:08 musy Exp $
#ifndef USER_BTAGGINGTOOL_H 
#define USER_BTAGGINGTOOL_H 1

// from STL
#include <fstream>
#include <string>
#include <math.h>
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event 
#include "Event/FlavourTag.h" 
#include "Event/RecHeader.h"

#include "ICombineTaggersTool.h" 
#include "Kernel/ITagger.h" 
#include "Kernel/IBTaggingTool.h" 
#include "Kernel/IParticleDescendants.h"

#include "ITaggingUtils.h"

/** @class BTaggingTool BTaggingTool.h 
 *
 *  Tool to tag the B flavour
 *
 *  @author Marco Musy
 *  @date   05/06/2005
 */

class BTaggingTool : public GaudiTool,
                     virtual public IBTaggingTool {

public: 

  /// Standard constructor
  BTaggingTool( const std::string& type,
                const std::string& name,
                const IInterface* parent );
  virtual ~BTaggingTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  StatusCode tag( LHCb::FlavourTag& theTag, const LHCb::Particle* );

  StatusCode tag( LHCb::FlavourTag& theTag, 
                  const LHCb::Particle*, const LHCb::RecVertex* );

  StatusCode tag( LHCb::FlavourTag& theTag, 
                  const LHCb::Particle*, const LHCb::RecVertex*, 
                  LHCb::Particle::ConstVector& ); 
  const LHCb::RecVertex* choosePrimary(const LHCb::RecVertex::Container* , 
                                       const LHCb::Particle* );
  

private:

  ITaggingUtils* m_util;
  IParticleDescendants* m_descend;

  ITagger *m_taggerMu, *m_taggerEle, *m_taggerKaon, *m_taggerVtxCh;
  ITagger *m_taggerKaonS, *m_taggerPionS, *m_taggerVtx, *m_taggerJetS;
  ICombineTaggersTool *m_combine;

  //properties ----------------
  double m_thetaMin, m_distphi_cut;
  double m_IPPU_cut;

  std::string m_taggerLocation, m_CombineTaggersName, m_ChoosePV;

  bool m_EnableMuon,m_EnableElectron,m_EnableKaonOS;
  bool m_EnableKaonSS,m_EnablePionSS,m_EnableVertexCharge;
  bool m_EnableJetSame;

  bool m_UseVtxOnlyWithoutOS;

};

//=======================================================================//
#endif // USER_BTAGGINGTOOL_H
