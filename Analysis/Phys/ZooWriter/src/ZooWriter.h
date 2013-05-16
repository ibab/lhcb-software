//$Header: $
/*
 * LoKi Algo + Zoo writer
 *
 * It saves all defined selections in the tree.
 *
 * Note: it is not MT safe
 *
 * AZ, SHM, MS, GK,  
 * PI Heidelberg 2008, 2009, 2010
 */

#ifndef __ZOOWRITER_H
#define __ZOOWRITER_H

#include <TFile.h>
#include <TTree.h>

#if DV_VER < 322
#include "Kernel/DVAlgorithm.h"
#else
#include "Kernel/DaVinciTupleAlgorithm.h"
#endif
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/ILHCbMagnetSvc.h"

#include "DecayTreeFitter/Fitter.h"

#include <vector>
#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool_alloc.hpp>

// forward declarations (to avoid including all the headers)
class IContextTool;
class IDistanceCalculator;
class ITriggerTisTos;
class IPVRefitter;
class ILifetimeFitter;
class IBackgroundCategory;
class ITrackGhostClassification;
class IFilterCriterion;
class ITrackExtrapolator;
class IDaVinciAssociatorsWrapper;
class IVisPrimVertTool;
class IEventTimeDecoder;
class ITrackFitter;

namespace LHCb {
  class VertexBase;
  class Particle;
  class MCParticle;
}

namespace Tf {
  template<class Hit> class TStationHitManager;
  template<class Hit> class TTStationHitManager;
  class DefaultVeloRHitManager;
  class DefaultVeloPhiHitManager;
  class VeloRHit;
  class VeloPhiHit;
}
class PatForwardHit;
class PatTTHit;
class ITrackHitCollector;

#include "ZooObjectManager.h"
#include "TrackExpectedHitsTool.h"

class ZooEv;

/** @class ZooWriter ZooWriter.h
 *  DaVinci Algorithm that is used to write ZooTuples.
 *  
 *  Note: To ensure the consistency of your Tuples, it is strongly advised that
 *  you don't change anything in the code of this algorithm. If you have a feature
 *  request, please inform the maintainers of this package.
 *
 *  @author Alexey Zhelezov
 *  @author Stephanie Hansmann-Menzemer 
 *  @author Manuel Schiller
 *  @author Georg Krocker
 *  @date   2010-06-21
 */
#if DV_VER < 322
class ZooWriter : public DVAlgorithm
#else
                  class ZooWriter : public DaVinciTupleAlgorithm
#endif
{
  public:
    ZooWriter( const std::string& name , ISvcLocator*       svc  );
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();
    virtual ~ZooWriter();

  private:
    ZooWriter();
    ZooWriter( const ZooWriter & );
    ZooWriter& operator= ( const ZooWriter & );

    // forward decl
    struct EventGuard;

    /// manages most of the pecularities of the ZooWriter in a consistent way
    /** writing to ROOT Trees breaks easily, so encapsulate everything that
     * is needed inside a struct; that way, inside the ZooWriter, you mostly
     * deal with a single method call to the struct, and the details can all
     * be done in a single place
     */
    friend class ZooWriterContext;
    class ZooWriterContext
    {
      public:
        typedef struct {
          boost::shared_ptr<ZooPArray> pref;
          std::string name;
          std::string collection;
        } KnownSelection;
        /** @brief exception to throw in case ZooWriterContext is
         * misused and gets out of sync.
         */
        class ZooWriterContextException : public std::exception
      {
        private:
          const char* m_what;
        public:
          ZooWriterContextException(const char* what) throw() :
            m_what(what) { }
          virtual ~ZooWriterContextException() throw() { }
          virtual const char* what() const throw()
          { return m_what; }
      };
      private:
        std::vector<KnownSelection>  m_sel;
        TFile* m_f;
        boost::shared_ptr<TTree> m_T;
        boost::shared_ptr<TTree> m_TperJob;
        boost::shared_ptr<ZooEv> m_pev;
        boost::shared_ptr<ZooObjectManager> m_objectManager;
        boost::shared_ptr<boost::object_pool<LHCb::Track> > m_trackPool;
        boost::shared_ptr<LHCb::VertexBase> m_pvWouter;
        // in case we need to refit tracks, we want to keep the fitted copy
        // (we're not allowed to alter objects on TES, so we refit a copy)
        typedef boost::pool_allocator<std::pair<const LHCb::Track*,
                LHCb::Track* > > FittedTrackMapAllocator;
        typedef std::map<const LHCb::Track*, LHCb::Track*,
                std::less<const LHCb::Track*>, FittedTrackMapAllocator> FittedTrackMap;
        FittedTrackMap m_fittedTracks;
        int m_objectCount;
        unsigned long m_perJobMapSz;
        unsigned long m_evts_begun;
        unsigned long m_evts_ended;

      public:
        /// constructor
        ZooWriterContext(const std::string& filename,
            const std::string& treename,
            const std::vector<std::string>& sel_collections,
            const std::vector<std::string>& sel_names, ZooWriter* const base);
        /// destructor
        ~ZooWriterContext();
        /// return ZooEv
        boost::shared_ptr<ZooEv> zooev() { return m_pev; }
        /// return selection
        std::vector<KnownSelection>& sel() { return m_sel; }
        /// return object manager
        boost::shared_ptr<ZooObjectManager> objman() { return m_objectManager; }
        /// return emergency PV
        boost::shared_ptr<LHCb::VertexBase>& pvWouter() { return m_pvWouter; }
        /// get or create mapping for fitted track
        LHCb::Track*& getOrCreateFittedTrackMapping(const LHCb::Track* tr)
        { return m_fittedTracks[tr]; }
        /// return a new track in the pool
        LHCb::Track* newTrackFromPool() { return m_trackPool->construct(); }
      private:
        /// do whatever needs to be done at the beginning of a new event
        void beginEvent();
        /// do whatever needs to be done at the end of an event
        void endEvent();

        friend class ZooWriter::EventGuard;
    };

    /// smart object that can be used to start and end the event
    class EventGuard
    {
      private:
        boost::shared_ptr<ZooWriterContext> m_ctx;
      public:
        EventGuard(boost::shared_ptr<ZooWriterContext> ctx) :
          m_ctx(ctx)
      { m_ctx->beginEvent(); }
        ~EventGuard() { m_ctx->endEvent(); }
    };

    boost::shared_ptr<ZooWriterContext> m_ctx;
    boost::shared_ptr<ZooEv> zooev() { return m_ctx->zooev(); }
    std::vector<ZooWriterContext::KnownSelection>& sel() { return m_ctx->sel(); }
    boost::shared_ptr<ZooObjectManager> objman() { return m_ctx->objman(); }
    boost::shared_ptr<LHCb::VertexBase>& pvWouter() { return m_ctx->pvWouter(); }

    StringArrayProperty      m_L0Name;
    StringArrayProperty      m_Hlt1Name;
    StringArrayProperty      m_Hlt2Name;
    StringArrayProperty      m_sel_names;
    StringArrayProperty      m_sel_collections; 
    StringArrayProperty      m_treefit_names;
    std::vector<int>         m_treefit_constraints;
    std::vector<int>         m_taggingList;
    std::vector<int>         m_triggerList;
    std::vector<int>         m_backgroundList;
    std::string              m_filename;
    std::string              m_treename;
    bool                     m_writeMC;
    bool                     m_writeMCtrees;
    std::vector<int>         m_MCList;
    bool                     m_writeDLL;
    bool                     m_writeStates;
    StringArrayProperty      m_linkToList;
    StringArrayProperty      m_linkFromList;
    std::string              m_PVReFitterName;	

    bool                     m_writeTrackInfo;
    std::vector<short>       m_extraInfoList;
    std::vector<unsigned>         m_particleinfoList;
    std::vector<std::pair<UInt_t ,LoKi::PhysTypes::Fun> >  m_lokifuns;
    StringArrayProperty      m_lokinames;
    bool                     m_writeHitPattern;
    bool                     m_writeExpectedHitPattern;
    bool                     m_writeCollectedHitPattern;
    bool                     m_writeOccupancies;
    bool                     m_intelligentPV;
    bool                     m_secondIpSig;
    bool                     m_onlyTreefitter;
    int                      m_minTracksPV;

    /// pids for which to write a ZooPackedParticle info block, negative values mean all
    std::vector<int>         m_covarianceList;
    /// pids for which to write a ZooPackedStates info block, negative values mean all
    std::vector<int>         m_packedStatesList;
    /// extrapolate the states to the given z (if true) or take closest
    bool                     m_packedStatesExtrapolate;
    /// write packed state at poca to z axis
    bool                     m_packedStateAtPocaToZAxis;
    /// write packed states closest to the given Z positions
    std::vector<double>      m_packedStatesZList;
    /// should ZooWriter attempt to make emergency PV if there is no PV
    bool                     m_makeEmergencyPV;
    /// write LHCbIDs
    bool                     m_writeLHCbIDs;
    /// write Generator level event info
    bool                     m_writeMCGenEventInfo;
    ///Write MC track multiplicity in acceptance
    std::vector<double>      m_mcAcceptanceEtaRegion;
    double                   m_mcAcceptanceMinP;
    bool                     m_mcAcceptanceCharged;
    bool                     m_mcAcceptanceNeutral;
    double                   m_mcAcceptanceMinCtau;

    const IContextTool*           m_context;
    const IDistanceCalculator*    m_dist;
    IBackgroundCategory*          m_bkg; 
    ITriggerTisTos*               m_TriggerTisTos;
#if DV_VER >= 256
    ITriggerTisTos*               m_L0TriggerTisTos;
#endif
    ITrackFitter*                 m_trackFitter;
    ILifetimeFitter*              m_lifetimeFitter;
    IPVReFitter*                  m_pvReFitter;
    ITrackExtrapolator*           m_extrapolator;
    IDaVinciAssociatorsWrapper*   m_linkerTool_Links; 
    Particle2MCLinker*            m_mcLink; 
    ITrackGhostClassification*    m_ghostClassification; 
    IVisPrimVertTool*             m_visPrimVertTool;
    IEventTimeDecoder*            m_odinDecoder;
    ITrackHitCollector*           m_hitCollector;
    TrackExpectedHitsTool*        m_expectedHitsTool;
    ILHCbMagnetSvc*               m_magFieldSvc;

    unsigned int                  m_evts;
    std::vector<unsigned int>     m_parts;



    // hit managers
    typedef Tf::TStationHitManager<PatForwardHit> HitManT;
    typedef Tf::TTStationHitManager<PatTTHit> HitManTT;
    typedef Tf::DefaultVeloPhiHitManager HitManVeloPhi;
    typedef Tf::DefaultVeloRHitManager HitManVeloR;
    HitManT* m_hitManagerT;
    HitManTT* m_hitManagerTT;
    HitManVeloR* m_hitManagerVeloR;
    HitManVeloPhi* m_hitManagerVeloPhi;

    const LHCb::RecVertex calculateSecondIP(ZooP* zp, const LHCb::Particle* p, const LHCb::RecVertex* myBestVertex) ;
    ZooP *GetSaved(const LHCb::Particle* p, const LHCb::RecVertex* recvtx = NULL); 

    void writeMC();
    ZooMCP *GetSaved(const LHCb::MCParticle* p);

    void addLoKi(ZooP*, const LHCb::Particle*);

    void writeEvent();
    void writeTrigger    ( ZooP* zp, const LHCb::Particle* p);
    void writeTagging    ( ZooP* zp, const LHCb::Particle* p);
    void writeDLL        ( ZooP* zp, const LHCb::Particle* p);
    void writeLinks      ( );
    double associateLink ( const LHCb::Particle* p_from, const LHCb::Particle* p_to);
    void writePackedStates( ZooTrackInfo* ztri, const LHCb::Particle* p);
    void writeTrackInfo(ZooP* zp, const LHCb::Particle* p); 
    void writeTrackExtraInfo(ZooTrackInfo* ztri, const LHCb::Particle* p); 
    void writeEventTriggerInfo(boost::shared_ptr<ZooEv> event) const;
    void writeEventODIN(boost::shared_ptr<ZooEv> event) const;
    void fillHitPattern(ZooTrackInfo* ztri, const LHCb::Particle* p);
    ZooPackedVertex* writePackedVertex(const LHCb::VertexBase* vtx,
        bool writeCovariance = false, bool registerMapping = false);
    void writeOccupancies(boost::shared_ptr<ZooEv> ev);

    // decay tree fitter
    void treeFit(const LHCb::Particle* p);
    void getTreefitParams(const LHCb::Particle* node, DecayTreeFitter::Fitter * treefitter);
    void setTreefitMassConstraint(const LHCb::Particle* node, DecayTreeFitter::Fitter * treefitter);

    // PV reconstruction
    void getPVWouter();

    // helper methods

    // return flags for use in ZooMCBkg info block
    unsigned getMCBkgFlags(const LHCb::Particle* p) const;

    // return refitted track pointer (if refitting is neccessary),
    // original track pointer otherwise
    const LHCb::Track* refittedTrack(const LHCb::Track* tr);

    /// extrapolate track and provide a state closest to z axis
    bool getTrackStateClosestToZAxis(
        const LHCb::Track& tr, LHCb::State& st) const;
};


#endif

// vim: sw=4:tw=78:ft=cpp
