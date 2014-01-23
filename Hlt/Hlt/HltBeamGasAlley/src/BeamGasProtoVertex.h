#ifndef INCLUDE_BEAMGASPROTOVERTEX_H
#define INCLUDE_BEAMGASPROTOVERTEX_H 1

#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"

class BeamGasProtoVertex : public HltAlgorithm
{

  public:
    // Standard methods and con/destructor
    BeamGasProtoVertex( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~BeamGasProtoVertex();
    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution

  private:
    // Member functions
    double sigmaBad( double z ) const;
    //### function to apply the zMin, zMax and lumi-region exclusion cuts
    bool passZCuts( double z ) const
    {
        return ( ( z > m_zTrMin && z < m_zTrMax ) &&
                 ( z < m_zTrExclLRLow || z > m_zTrExclLRUp ) );
    }
    template <typename ITER>
    void findProtoVertex( ITER begin, ITER end );
    template <typename ITER>
    void printVector( ITER begin, ITER end, const std::string& aText ) const;
    template <typename ITER>
    void getMeanAndSigma( ITER begin, ITER end, double& sMean,
                          double& sSigma ) const;

    unsigned stepSize1() const
    {
        return ( m_minNumTracks + 1 ) / 2;
    } // size of the main steps
    unsigned stepSize2() const
    {
        return ( m_minNumTracks - stepSize1() + 1 ) / 2;
    } // size of the extension step
    unsigned minTracksToAccept() const
    {
        return stepSize1() + stepSize2();
    } // in case of good variance

    Hlt::SelectionContainer2<LHCb::Track, LHCb::Track> m_trackSelection; // tracks
                                                                         // we'll be
                                                                         // looking
                                                                         // at
    double m_zTrMin;         // minimal possible z-value (z-list will be trimmed)
    double m_zTrMax;         // maximal possible z-value (z-list will be trimmed)
    double m_zTrExclLRLow;   // lower limit of the lumi-region exclusion zone
    double m_zTrExclLRUp;    // upper limit of the lumi-region exclusion zone
    unsigned m_minNumTracks; // number a bit higher than the number of tracks needed
                             // for a trigger
    unsigned m_maxNumOfVeloTracks; // process only events with lowish number of
                                   // tracks
};
#endif // INCLUDE_BEAMGASTRIGPROTOVERTEX_H
