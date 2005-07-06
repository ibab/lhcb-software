// $Id: TaggerElectronTool.h,v 1.3 2005-07-06 00:33:19 musy Exp $
#ifndef USER_TAGGERELECTRONTOOL_H 
#define USER_TAGGERELECTRONTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "DaVinciTools/IGeomDispCalculator.h"
#include "RecoTools/ITrVeloCharge.h"
#include "Kernel/ITagger.h"
#include "INNetTool.h"

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
  virtual Tagger tag( const Particle*, 
		      std::vector<const Vertex*>&, ParticleVector&);
  //-------------------------------------------------------------

private:
  std::string m_veloChargeName;
  ITrVeloCharge* m_veloCharge;
  IGeomDispCalculator *m_Geom;
  INNetTool* m_nnet;
  std::string m_CombinationTechnique;

  double pol4(double x, double a0, double a1, double a2, double a3);
  StatusCode calcIP( Particle* , const Vertex* , double&, double&);

  //properties 
  double m_Pt_cut_ele;
  double m_P_cut_ele;
  long   m_VeloChMin;
  long   m_VeloChMax;
  double m_EoverP;
};

//===============================================================//
#endif // USER_TAGGERELECTRONTOOL_H
