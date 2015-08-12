// $Id: TaggerJetSameTool.h,v 1.3 2006-10-24 10:16:45 jpalac Exp $
#ifndef USER_TAGGERJETSAMETOOL_H
#define USER_TAGGERJETSAMETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"

/** @class TaggerJetSameTool TaggerJetSameTool.h
 *
 *  Tool to tag the B flavour with a JetSame Tagger
 *
 *  @author Julien Babel
 *  @date   30/06/2005
 */

class TaggerJetSameTool : public GaudiTool,
                          virtual public ITagger {

public:

  /// Standard constructor
  TaggerJetSameTool( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

  virtual ~TaggerJetSameTool( ); ///< Destructor

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*, 
                            const LHCb::RecVertex*,
                            const int,
                            LHCb::Particle::ConstVector& );
  //-------------------------------------------------------------

private:
  //properties
  double  m_AverageOmega;
  double  m_Pt_cut_jetS, m_dR_cut_jetS;
};
//===============================================================//
#endif // USER_TAGGERJETSAMETOOL_H
