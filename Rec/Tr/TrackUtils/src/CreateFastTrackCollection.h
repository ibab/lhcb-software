#ifndef INCLUDE_CREATEFASTTRACKCOLLECTION_H
#define INCLUDE_CREATEFASTTRACKCOLLECTION_H 1

#include <vector>
#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"

/** @class CreateFastTrackCollection CreateFastTrackCollection.h
 * given a list of input track containers, this algorithm creates a fast
 * GaudiSharedObjectsContainer containing pointers to the tracks in the
 * input containers given
 *
 * @author Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
 * @date   2009-02-25
 */
class CreateFastTrackCollection : public GaudiAlgorithm
{
    public:
	/// Standard Constructor
	CreateFastTrackCollection(const std::string& name, ISvcLocator* pSvcLocator);

	virtual ~CreateFastTrackCollection(); ///< Destructor

	virtual StatusCode initialize(); ///< Algorithm initialization
	virtual StatusCode    execute(); ///< Algorithm event execution

    private:
	std::vector<std::string> m_inputLocations; ///< input locations
	std::string m_outputLocation; ///< output location
	bool m_slowContainer; ///< optionally deep-copy tracks into keyed cont.
};
#endif // INCLUDE_CREATEFASTTRACKCOLLECTION_H
