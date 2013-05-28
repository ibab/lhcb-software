#ifndef PrVeloUTChecker_H
#define PrVeloUTChecker_H

// Include files
#include "Event/Track.h"
#include "Event/MCParticle.h"

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// interfaces
#include "MCInterfaces/IMCReconstructible.h"

// linkers
#include "Linker/LinkerTool.h"

/** @class PrVeloUTChecker PrVeloUTChecker.h "PrMCTools/PrVeloUTChecker"
 *
 * Class for VeloTT studies
 *  @author A. Di Canto
 *  @date   2012-05-18
 */

class PrVeloUTChecker : public GaudiHistoAlg
{
public:
	/** Standard construtor */
	PrVeloUTChecker( const std::string& name, ISvcLocator* pSvcLocator );

	/** Destructor */
	virtual ~PrVeloUTChecker();

	/** Algorithm execute */
	virtual StatusCode execute();

	/** Algorithm initialize */
	virtual StatusCode initialize();

	/** Algorithm finalize */
	virtual StatusCode finalize();

protected:
	void computeEfficiency();
	void computeGhostRate();
	void printStatistics();
	void getTTtruth(const LHCb::MCParticle*, int &hits, int &layers);
	bool bAncestor(const LHCb::MCParticle* ) const;
	void fillEffHistos( double var, std::string title, double min, double max, bool found, std::vector<bool> flags );
	void fillGhostHistos( double var, std::string title, double min, double max, bool found, std::vector<bool> flags );

private:
	/** Data memebers */
	std::string m_ForwardContainer;     ///< Input Forward tracks container location
	std::string m_VeloContainer;        ///< Input Velo tracks container location
	std::string m_VeloTTContainer;      ///< Input VeloTT tracks container location
	IMCReconstructible* m_selector;     ///< Pointer to selector

	typedef LinkerTool<LHCb::Track, LHCb::MCParticle> AsctTool;
	AsctTool m_associator;
	AsctTool::DirectType* m_directTable;
	AsctTool::InverseType* m_inverseTable;
	AsctTool m_associator_velo;
	AsctTool::DirectType* m_directTable_velo;
	AsctTool::InverseType* m_inverseTable_velo;
	AsctTool m_associator_forward;
	AsctTool::DirectType* m_directTable_forward;
	AsctTool::InverseType* m_inverseTable_forward;

	class MyCounter {
	public:
		std::vector<std::string> name;
		std::vector<int>         num;
		std::vector<int>         den;

		void addCategory(std::string );
		void count(bool , std::vector<bool> );
	};

	MyCounter m_eff_counter;
	MyCounter m_ghosts_counter;
};

#endif// PrVeloUTChecker_H
