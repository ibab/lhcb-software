// $Id: TaggerElectronTool.h,v 1.8 2007-02-28 08:27:58 sposs Exp $
#ifndef USER_TAGGERELECTRONTOOL_H 
#define USER_TAGGERELECTRONTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/ITagger.h"
#include "CaloInterfaces/ICaloElectron.h" 
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
  virtual LHCb::Tagger tag( const LHCb::Particle*, const LHCb::RecVertex*,
			    std::vector<const LHCb::Vertex*>&, 
			    LHCb::Particle::ConstVector&);
  //-------------------------------------------------------------

private:
  IGeomDispCalculator *m_Geom;
  INNetTool* m_nnet;
  ICaloElectron*  m_electron;

  std::string m_CombinationTechnique, m_NeuralNetName;

  StatusCode calcIP( const LHCb::Particle* , 
		     const LHCb::Vertex* , double&, double&);
  StatusCode calcIP( const LHCb::Particle* , 
		     const LHCb::RecVertex* , double&, double&);
  //properties 
  double m_Pt_cut_ele;
  double m_P_cut_ele;
  double m_VeloChMin;
  double m_VeloChMax;
  double m_EoverP;  
  double m_AverageOmega;

};

//===============================================================//
#endif // USER_TAGGERELECTRONTOOL_H
