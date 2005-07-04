// $Id: TaggerElectronTool.h,v 1.2 2005-07-04 15:40:09 pkoppenb Exp $
#ifndef USER_TAGGERELECTRONTOOL_H 
#define USER_TAGGERELECTRONTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "RecoTools/ITrVeloCharge.h"
#include "Kernel/ITagger.h"

/** @class TaggerElectronTool TaggerElectronTool.h 
 *
 *  Tool to tag the B flavour with a Electron Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerElectronTool : public GaudiTool, 
	virtual public ITagger {

public: 
  /// Standard constructor
  TaggerElectronTool( const std::string& type,
		  const std::string& name,
		  const IInterface* parent );
  virtual ~TaggerElectronTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  ParticleVector taggers( const Particle*, const Vertex*, 
				  const ParticleVector& );
  //-------------------------------------------------------------

private:
  std::string m_veloChargeName;
  ITrVeloCharge* m_veloCharge;

  //properties 
  double m_Pt_cut_ele;
  double m_P_cut_ele;
  long   m_VeloChMin;
  long   m_VeloChMax;
  double m_EoverP;
};

//===============================================================//
#endif // USER_TAGGERELECTRONTOOL_H
