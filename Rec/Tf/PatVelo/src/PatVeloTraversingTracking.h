#ifndef TF_PATVELOTRAVERSINGTRACKING_H 
#define TF_PATVELOTRAVERSINGTRACKING_H 1

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
//velo
#include "VeloDet/DeVelo.h"
//track
#include "Event/Track.h"
#include "AIDA/IHistogram1D.h"
// ============================================================================

namespace Tf
{
  /** @class PatVeloTraversingTracking PatVeloTraversingTracking.h
   *
   * See talk in VELO Commissionig meeting 15/06/2009 for more info.
   *
   * This algorithm tries to find tracks from particles that traverse both
   * detector halves. As the pattern recognition programs split these tracks
   * into two tracks (one per half) this algorithm merges the candidates and
   * saves them in TES for later processing, for example by the VELO alignment.
   * The track matching works with three criteria:
   *
   * 1. The tracks have to have similar theta
   * 2. The tracks have to be in different VELO halves
   * 3. The tracks have to be parallel. This is done by extrapolating
   *    the tracks to large distances and checking if the distance betwen
   *    them remains constant.
   *    Essentially it's the same as a phi-criterion.
   *
   * Execute this algorithm AFTER pattern recognition but BEFORE the alignment.
   *
   *
   * N.B! The pattern recognition has to provide tracks in the local frame
   * for this algorithm to work properly.
   * For PatVeloGeneralTracking this is done by setting the option
   * PatVeloGeneralTracking.PatVeloTrackTool.TracksInHalfBoxFrame = True
   * same for PatVeloSpaceTool
   *
   *  @author Aras Papadelis. aras.papadelis@cern.ch
   *  @date   2009-06-22
   */

  class PatVeloTraversingTracking : public GaudiHistoAlg {
  public:
    /// Standard constructor
    PatVeloTraversingTracking( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~PatVeloTraversingTracking( ) {}; ///< Empty Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  private:
    AIDA::IHistogram1D* m_halfDistanceX;
    AIDA::IHistogram1D* m_halfDistanceY;
    AIDA::IHistogram1D* m_halfSigmaX;
    AIDA::IHistogram1D* m_halfSigmaY;

    /** @class PatVeloTraversingTracking::TraversingTrackSet PatVeloTraversingTracking.h
     *
     * Container class. Holds two traversing track candidates,
     * their merged track and their x-distance.
     * Local to PatVeloTraversingTracking
     *
     *  @author Aras Papadelis. aras.papadelis@cern.ch
     *  @date   2009-06-22
     */
    class TraversingTrackSet{
    public:
      TraversingTrackSet() : m_t1(NULL), m_t2(NULL) {}; ///< Empty constructor

      /// set parameters from two tracks
      void Init(double distx, double sigmax,
                double disty, double sigmay,
                LHCb::Track* t1,LHCb::Track* t2){
        m_distancex = distx;
        m_sigmax = sigmax;
        m_distancey = disty;
        m_sigmay = sigmay;
        m_t1 = t1->clone();
        m_t2 = t2->clone();
      }

      // accessors
      inline double DistanceX(){return m_distancex;}
      inline double SigmaX(){return m_sigmax;}
      inline double DistanceY(){return m_distancey;}
      inline double SigmaY(){return m_sigmay;}
      inline LHCb::Track* Track1(){return m_t1;}
      inline LHCb::Track* Track2(){return m_t2;}

      /// return the track constructed from the two stored tracks
      LHCb::Track* mergedTrack(){
        LHCb::Track* merged(m_t1->clone());
        const std::vector<LHCb::LHCbID>& m_idContainer = m_t2->lhcbIDs();
        std::vector<LHCb::LHCbID>::const_iterator itID;
        for(itID=m_idContainer.begin();itID!=m_idContainer.end();++itID)
          merged->addToLhcbIDs(*itID);
        return merged;
      }

    private:
      double m_distancex;  ///< X distance between the two tracks
      double m_sigmax;     ///< X sigma of the dist between tracks
      double m_distancey;  ///< Y distance between the two tracks
      double m_sigmay;     ///< Y sigma of the dist between tracks
      LHCb::Track* m_t1;  ///< pointer to first track
      LHCb::Track* m_t2;  ///< pointer to second track
    };

  public:
    /// Main method. Finds traversing tracks based on the 3 criteria
    /// listed in the class description
    void findTracks(std::vector<TraversingTrackSet>& chosen_sets,
                    const LHCb::Tracks * trackContainer,
                    std::vector< std::vector<LHCb::Track*> > & traversingTrackContainer);

    /// put this track into the TES
    void saveTrackInTES(LHCb::Track*);

    /// Method that calculates the signed impact parameter between
    /// a track and the z-axis
    double impactParameter(LHCb::Track*);

  private:
    /// Initial guess of opening. Needed to exclude luminous region
    double m_distanceGuess;

    /// Sigma criterion (measure of deviation from parallel lines)
    double m_sigmaCut;

    /// Theta criterion
    double m_thetaCut;

    /// Impact parameter cut in mm
    double m_IPCut;

    /// Include tracks from luminous region?
    bool m_includeLR;

    /// store a histogram of the esitmated openning of the detector
    bool m_makeHistogram;

    DeVelo* m_veloDet; ///< Velo detector element
    std::string m_inputTracksLocation; ///< Input track location
    std::string m_outputTracksLocation; ///< Output track location

    bool m_debugLevel;
    bool m_verboseLevel;
  };

}

#endif // TF_PATVELOTRAVERSINGTRACKING_H
