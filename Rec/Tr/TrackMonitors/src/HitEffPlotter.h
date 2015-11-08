#ifndef INCLUDE_HITEFFPLOTTER_H
#define INCLUDE_HITEFFPLOTTER_H 1

#include <vector>
#include <bitset>
#include <string>
#include <array>

#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/Vector3DTypes.h"

class IHitExpectation;
class IVeloExpectation;
class TrackExpectedHitsXYZTool;

/** @class HitEffPlotter HitEffPlotter.h
 *
 * plot effective hit efficiencies for given track container as function of (x, y)
 *
 * @author Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
 * @date   2010-06-10
 */
class HitEffPlotter : public GaudiHistoAlg
{
    public:

	/// Standard Constructor
	HitEffPlotter(const std::string& name, ISvcLocator* pSvcLocator);

	virtual ~HitEffPlotter(); ///< Destructor

	virtual StatusCode initialize(); ///< Algorithm initialization
	virtual StatusCode    execute(); ///< Algorithm event execution

    protected:


    private:
	/// name of input track collection
	std::string m_inputCollection;
  bool m_useUT;
  
	IVeloExpectation* m_veloExpectation;
	IHitExpectation* m_ttExpectation;
	IHitExpectation* m_itExpectation;
	IHitExpectation* m_otExpectation;
	TrackExpectedHitsXYZTool* m_xyzExpectation;

	/// plot hit efficiency numerator and denominator as function of (x, y)
	template<size_t N> void plot(
		std::string namepfx, std::string titlepfx,
		unsigned nxbins, const double xmin, const double xmax,
		unsigned nybins, const double ymin, const double ymax,
		const std::bitset<N>& expected, const std::bitset<N>& ontrack,
		const std::array<Gaudi::XYZVector, N>& points);


};
#endif // INCLUDE_HITEFFPLOTTER_H

