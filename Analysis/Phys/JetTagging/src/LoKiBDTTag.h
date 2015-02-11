#ifndef LOKIBDTTAG_H   // Begin LoKi:BDTTag.
#define LOKIBDTTAG_H 1

// Gaudi.
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"

// Kernel.
#include "Kernel/IJetTagTool.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/IDistanceCalculator.h"

// Event.
#include "Event/ODIN.h"
#include "Event/Particle.h"
#include "Event/RecVertex.h"

// ROOT.
#include "TVector3.h"
#include "TMath.h"

// TMVA.
#include "TMVA/Reader.h"

using namespace std;
using namespace LHCb;
namespace LoKi {
  /**
   * Jet tagging via the secondary vertex (SVR) method of LHCb-ANA-2014-074.
   *
   * The jet tagging algorithm of LHCb-ANA-2014-074 builds two-body
   * SVRs. When a jet is supplied, the two-body SVRs within the cone
   * of the jet, set by m_dr, are linked if they share a common track
   * to produce n-body vertices. Two BDTs are calculated for each
   * n-body SVR. BDT0 separates light jets (low response) from heavy
   * jets (high response). BDT1 separates c-jets (low response) from
   * b-jets (high response).
   *
   * @class   BDTTag
   * @file    LoKiBDTTag.h
   * @author  Philip Ilten
   * @author  Mike Williams
   * @date    2015-01-30
   * @version 1.0
   */
  class BDTTag : public virtual IJetTagTool, public GaudiTool {

    // Allow a templated ToolFactory of this class.
    friend class ToolFactory<LoKi::BDTTag>;
    friend class Svr;

  public:

    StatusCode initialize(); ///< Initialize the tagger.
    StatusCode finalize();   ///< Finalize the tagger.
    /**
     * Calculate the tagger properties for a jet.
     *
     * The map of jet tag properties returned contains "Tag" which is
     * the number of n-body SVRs associated with the jet. For each
     * n-body vertex the relevant variables are supplied via the form
     * "TagX_Y" where X is the index of the SVR, beginning from zero,
     * and Y is one of the variable names of the structure Vars,
     * without the prefix "m_". Additionally, the BDT0 and BDT1
     * responses are returned via "TagX_bdt0" and "TagX_bdt1". The
     * SVRs are sorted from high to low via the variable set with
     * m_nbvSort, which by default is SVR pT.
     * 
     * An additional variable "TagX_pass" is also provided, which
     * flags whether the n-body SVR has passed the pre-selection of
     * Table 5 from LHCb-ANA-2014-074. If m_nbvSelect is set to true
     * only the first tag is considered, and is written only if it
     * passes the pre-selection. Consequently, if m_nbvSelect is true
     * "Tag0_pass" will always be set to 1 if it exists. Additional
     * variables, besides the ten used in the BDT, are required to
     * apply the pre-selection. These are provided by "TagX_tau" (the
     * lifetime-proxy), "TagX_z" (the vertex z-position), and "pt"
     * (the pT of the vertex). The variable "TagX_backwards" indicates
     * whether the tag is a backwards tag.
     */
    bool calculateJetProperty(const Particle *jet, map<string, double> &props);

  protected:

    /// Standard constructor.
    BDTTag(const std::string& type, const std::string& name, 
	   const IInterface* parent);

    /// Standard destructor.
    virtual ~BDTTag();

    /// Structure for the SVR variables needed by the TMVA reader for the BDTs.
    struct Vars {
      float m_fdrMin;    ///< Minimum two-body SVR radial flight distance.
      float m_ptSvrJet;  ///< Ratio of pT, pT(SVR) / pT(jet).
      float m_nTrk;      ///< Number of tracks.
      float m_nTrkJet;   ///< Number of tracks also contained in the jet.
      float m_drSvrJet;  ///< dR(SVR flight direction, jet momentum direction).
      float m_absQSum;   ///< Absolute value of summed charges.
      float m_m;         ///< Mass.
      float m_mCor;      ///< Corrected mass.
      float m_fdChi2;    ///< Chi-squared of the flight distance.
      float m_ipChi2Sum; ///< Summed minimum IP chi-squared of the tracks.
    };

    /**
     * Internal SVR representation for the LoKi::BDTTag jet tagger class.
     *
     * This class represents two types of SVRs, a two-body SVR or an
     * n-body SVR. The distinction is that a two-body SVR has an empty
     * m_svrs vector, while an n-body SVR has valid pointers to one or
     * more two-body SVRs. Consequently, an n-body SVR can physically
     * be a two-body vertex. However, only n-body SVRs can be
     * linked. All other methods and members are valid for both types
     * of SVRs.
     */
    class Svr {
  
    public:

      /// Two-body constructor.
      Svr(BDTTag *parent, const Particle *prt1, const Particle *prt2);

      /// N-body constructor.
      Svr(const Svr *svr);
      
      /**
       * Check if the two-body SVR passes the quality requirements.
       * 
       * The two-body vertex requirements are given in Table 4 of
       * LHCb-ANA-2014-074.
       */
      bool pass();
      /**
       * Write the n-body SVR info, if it passes the quality requirements.
       *
       * The n-body vertex requirements are given in Table 5 of
       * LHCb-ANA-2014-074. If the SVR does not pass the n-body
       * requirements, false is returned and the properties are not
       * written. If m_nbvSelect is set to false for m_parent then the
       * info is written even if the SVR does not pass the quality
       * requirements.
       */
      bool info(int idx, const Particle *jet, map<string, double> &props);
      /**
       * Calculate the stored variables. Returns true if successful.
       *
       * If no jet is passed, as by default, only the intrinsic SVR
       * properties are calculated; any values dependent upon the jet
       * are not calculated.
       */
      bool calc(const Particle *jet = 0, bool force = false); 
      /// Link an n-body SVR with another n-body SVR, failure returns false.
      bool link(Svr &nbv);
      
      // Members.
      vector<const Particle*> m_trks; ///< The stored tracks.
      vector<const Svr*>      m_tbvs; ///< The stored two-body SVRs.
      const RecVertex        *m_pvr;  ///< The associated PVR.
      Vertex                  m_vrt;  ///< The SVR vertex.
      Particle                m_prt;  ///< The SVR particle.
      
      // Stored variables.
      bool     m_stored;  ///< Flag if variables are stored.
      bool     m_fit;     ///< Flag whether the vertex has been fit.
      TVector3 m_fv;      ///< SVR flight vector.
      double m_fdrMin;    ///< Minimum two-body SVR radial flight distance.
      double m_ptSvrJet;  ///< Ratio of pT, pT(SVR) / pT(jet).
      double m_nTrkJet;   ///< Number of tracks also contained in the jet.
      double m_drSvrJet;  ///< dR(SVR flight direction, jet momentum direction).
      double m_absQSum;   ///< Absolute value of summed charges.
      double m_mCor;      ///< Corrected mass.
      double m_fdChi2;    ///< Chi-squared of the flight distance.
      double m_ipChi2Sum; ///< Summed minimum IP chi-squared of the tracks.
      double m_ipMin;     ///< Minimum impact parameter to a PVR.
      double m_ipChi2Min; ///< Minimum IP chi-squared to a PVR.
      double m_docaMin;   ///< Distance of closest approach between the tracks.
      double m_drMax;     ///< Maximum dR between the linked SVR momenta.
      double m_dsMax;     ///< Maximum distance between the linked SVR.
      double m_tau;       ///< Flight distance over momentum (in GeV).
      double m_bdt0;      ///< BDT0 response.
      double m_bdt1;      ///< BDT1 response.

    private:

      ///< The parent tagging algorithm.
      BDTTag *m_parent; 
    };

    /// Calculate dR between two particles.
    static double deltaR(const Particle *prt1, const Particle *prt2);
    /// Calculate dR between the SVR and a particle.
    static double deltaR(const Svr *svr, const Particle *prt, double sgn = 1);
    /// Compare two particles by pT, used for std::sort.
    static bool comparePrtPt(const Particle *prt1, const Particle *prt2);
    /// Compare two SVRs by pT, used for std::sort.
    static bool compareSvrPt(const Svr &svr1, const Svr &svr2);
    /// Compare two SVRs by BDT0, used for std::sort.
    static bool compareSvrBdt0(const Svr &svr1, const Svr &svr2);
    /// Compare two SVRs by BDT1, used for std::sort.
    static bool compareSvrBdt1(const Svr &svr1, const Svr &svr2);
    /**
     * Return the vector of two-body SVRs.
     *
     * Builds the two-body SVRs if not already built, i.e. the event
     * is new, and stores them in m_tbvs. A pointer to this vector is
     * returned. First a check is made for user supplied SVRs via the
     * TES. If these do not exist then two-body SVRs are produced from
     * the selected particles. If the two-body vertices cannot be
     * built, a null pointer is returned. If force is set to true the
     * SVRs are always rebuilt.
     */
    const vector<Svr> *tbvs(bool force = false);
    /**
     * Return the vector of n-body SVRs.
     *
     * The two-body SVRs are built via tbvs(), and then n-body SVRs
     * are produced via linking from the two-body SVRs that satisfy
     * dR(SVR flight direction, jet momentum) < m_dR. If requested,
     * the n-body SVRs are sorted by the variable of m_nbvSort. If the
     * n-body vertices cannot be built, a null pointer is returned.
     */
    const vector<Svr> *nbvs(const Particle *jet);

    // Members.
    IVertexFit          *m_fitTool;   ///< The vertex fitter tool.
    IDistanceCalculator *m_dstTool;   ///< The distance calculator tool.
    TMVA::Reader        *m_reader;    ///< TMVA reader for the BDTs.
    Vars                 m_vars;      ///< Variables for the TMVA reader.
    Particles           *m_prts;      ///< Particles used to build SVRs.
    RecVertices         *m_pvrs;      ///< Primary vertices.
    vector<Svr>          m_tbvs;      ///< Two-body secondary vertices.
    vector<Svr>          m_nbvs;      ///< N-body secondary vertices.
    int                  m_runNumber; ///< Stored run number.
    ulonglong            m_evtNumber; ///< Stored event number.

    // Members used for tool configuration.
    /**
     * BDT0 TMVA weights filename and path from $JETTAGGINGROOT.
     *
     * Configured via "Bdt0Weights" with a default of
     * "data/bdt_configs/LHCb_ANA_2014_076_BDT0.weights.xml". Changing
     * this property takes effect only after initialization. Warning,
     * changing this invalidates the results of LHCb-ANA-2014-074.
     */
    string m_bdt0Weights;
    /**
     * BDT1 TMVA weights filename and path from $JETTAGGINGROOT.
     *
     * Configured via "Bdt1Weights" with a default of
     * "data/bdt_configs/LHCb_ANA_2014_076_BDT1.weights.xml". Changing
     * this property takes effect only after initialization. Warning,
     * changing this invalidates the results of LHCb-ANA-2014-074.
     */
    string m_bdt1Weights;
    /**
     * Options to pass to the TMVA reader.
     *
     * Configured via "TmvaOptions" with a default of
     * "Silent". Changing this property takes effect only after
     * initialization. To switch to verbose TMVA output change this
     * option to "V".
     */
    string m_tmvaOptions;
    /**
     * Name of the vertex fitter tool used to create SVRs.
     *
     * Configured via "FitName" with a default of
     * "OfflineVertexFitter". Changing this property takes effect only
     * after initialization. Warning, changing this invalidates the
     * results of LHCb-ANA-2014-074.
     */
    string m_fitName;
    /**
     * Name of the distance calculator tool used to create SVRs.
     *
     * Configured via "DstName" with a default of
     * "LoKi::DistanceCalculator:PUBLIC". Changing this property takes
     * effect only after initialization. Warning, changing this
     * invalidates the results of LHCb-ANA-2014-074.
     */
    string m_dstName;
    /**
     * The maximum dR(SVR flight direction, jet momentum) for linking
     * two-body SVRs.
     *
     * Configured via "DR" with a default of 0.5. Only two-body SVRs
     * with dR(SVR flight direction, jet momentum) < m_dr are linked
     * to build SVR candidates for a jet. Changing this property takes
     * effect only after initialization. Warning, changing this
     * invalidates the results of LHCb-ANA-2014-074.
     */
    double m_dr;
    /**
     * If true, build backwards n-body SVRs by reversing the SVR
     * flight directions.
     *
     * Configured via "Backwards" with a default of false. Backwards
     * SVRs are used in LHCb-ANA-2014-074 to cross-check light jet
     * backgrounds. If this flag is set to true the n-body SVRs are
     * built with two-body SVRs satisfying dR(-SVR flight direction,
     * jet momentum) < m_dr. Additionally, the flight direction of the
     * n-body vertex is reversed when calculating dR with the
     * jet. This property can be set at any time and will take
     * immediate effect when calculateJetProperty is called.
     */
    bool m_backwards;
    /**
     * If true, apply the default selection to the particles.
     *
     * Configured via "PrtSelect" with a default of true. The particle
     * selection applied is given by Table 3 in LHCb-ANA-2014-074. One
     * can apply a different selection by filtering particles,
     * providing the filtered location via m_prtLocation, and setting
     * m_prtSelect to false. Changing this property takes effect only
     * after the next event is read if calculateJetProperty has
     * already been called. Warning, changing the particle selection
     * invalidates the results of LHCb-ANA-2014-074.
     */
    bool m_prtSelect; 
    /**
     * If true, apply the default selection to the n-body SVRs.
     *
     * Configured via "NbvSelect" with a default of true. The n-body
     * vertex selection is given by Table 5 in LHCb-ANA-2014-074. This
     * property can be set at any time and will take immediate effect
     * when calculateJetProperty is called. Warning, changing the
     * n-body selection invalidates the results of LHCb-ANA-2014-074.
     */
    bool m_nbvSelect; 
    /**
     * Sort the n-body SVRs by "pt", "bdt0", or "bdt1".
     *
     * Configured via "NbvSort" with a default of "pt". If the value
     * of m_nbvSort is not set to one of the three recognized options,
     * the n-body SVRs will not be sorted. This sorting only affects
     * the order in which the SVR information is written out via
     * calculateJetProperty. This property can be set at any time and
     * will take immediate effect when calculateJetProperty is called.
     */
    string m_nbvSort;
    /**
     * TES location of particles used to build the SVRs.
     *
     * Configured via "PrtLocation" with a default of
     * "Phys/StdAllNoPIDsPions/Particles". Changing this property
     * takes effect only after the next event is read if
     * calculateJetProperty has already been called. Warning, changing
     * the particle location invalidates the results of
     * LHCb-ANA-2014-074.
     */
    string m_prtLocation;
    /**
     * Optional TES location of two-body SVRs. If not set, the
     * two-body SVRs will be automatically built.
     *
     * Configured via "TbvLocation" with a default of "". The two-body
     * SVRs used to build the final n-body SVRs are created from the
     * particles provided by m_prtLocation. However, if a valid TES
     * location is provided by m_tbvLocation, all two-body
     * combinations from this location are used instead. Changing this
     * property takes effect only after the next event is read if
     * calculateJetProperty has already been called. Warning, changing
     * the SVR location invalidates the results of LHCb-ANA-2014-074.
     */
    string m_tbvLocation;
    /**
     * TES location of PVRs used when building the SVRs.
     *
     * Configured via "PvrLocation" with a default of
     * RecVertexLocation::Primary. These PVRs are used when
     * calculating any flight vector or impact parameter information
     * for the particles and SVRs. Changing this property takes effect
     * only after the next event is read if calculateJetProperty has
     * already been called.
     */
    string m_pvrLocation;
    /**
     * TES location of ODIN.
     *
     * Configured via "OdnLocation" with a default of
     * ODINLocation::Default. The ODIN location must be found, as this
     * is used to trigger when the two-body vertices for an event must
     * be built. Changing this property takes effect only after the
     * next event is read if calculateJetProperty has already been
     * called.
     */
    string m_odnLocation;

  private:
    
    BDTTag();                         ///< Disables the default constructor.
    BDTTag(const BDTTag&);            ///< Disables the copy constructor.
    BDTTag& operator=(const BDTTag&); ///< Disables the assignment operator.
  };
}

#endif // End LoKi:BDTTag.
