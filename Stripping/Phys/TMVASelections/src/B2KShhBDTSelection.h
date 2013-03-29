// $Id: $
#ifndef B2KSHHBDTSELECTION_H 
#define B2KSHHBDTSELECTION_H 1

#include <vector>
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"

#include "Kernel/IParticleFilter.h"

#include "TMVA/Reader.h"
#include "TString.h"

#include "LoKi/LoKiPhys.h"
#include "LoKi/Child.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/ParticleContextCuts.h"

class IDistanceCalculator;
class DVAlgorithm;

/** @class B2KShhBDTSelection B2KSBDTSelection.h
 *  
 *
 *  @author Rafael Coutinho, Thomas Latham and David Dossett
 *  @date   2013-03-27
 */
class B2KShhBDTSelection : public extends1<GaudiTool,IParticleFilter>{
  
	friend class ToolFactory<B2KShhBDTSelection>;

public: 
	/** initialize tool */
	virtual StatusCode initialize();
	
	
	/** finalize tool */
	virtual StatusCode finalize();
	
	
	/** 
	 *  @see IParticleFilter 
	 */
	virtual bool operator()(const LHCb::Particle* p) const ;
  
protected:
	/// Standard constructor
	B2KShhBDTSelection( const std::string& type,
						const std::string& name,
	                    const IInterface* parent );
	
	virtual ~B2KShhBDTSelection( ); ///< Destructor

private:
	/// default constructor is disabled 
	B2KShhBDTSelection();
	
	/// copy constructor is disabled 
	B2KShhBDTSelection(const B2KShhBDTSelection&);
	
	/// assignemet operator is disabled 
	B2KShhBDTSelection& operator=(const B2KShhBDTSelection&);
	
	bool set(const LHCb::Particle* p) const;
	
	const IDistanceCalculator* m_dist;
	IDVAlgorithm *m_dva;
	TMVA::Reader *m_BDTReader;
	
	unsigned int m_nVars;  
	float *m_values;
	
	double m_cut; // BDT cut value
	std::string m_weightsFile; // weights file
 	std::string m_typeKS;
	
	const LoKi::Cuts::CHI2IP m_BPVIPCHI2;
	const LoKi::Cuts::VDCHI2 m_BPVVDCHI2;
};
#endif // B2KSHHBDTSELECTION_H
