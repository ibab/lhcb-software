#ifndef RELINFOCDFISOVARIABLES_H 
#define RELINFOCDFISOVARIABLES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/GetIDVAlgorithm.h"
#include "Kernel/IRelatedInfoTool.h"
#include "Event/RelatedInfoMap.h"
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IDistanceCalculator.h"
//needed for TMVA
#include "MVADictTools/TMVATransform.h"
#include "Kernel/IParticleDictTool.h"
#include "Event/Particle.h"


/** @class RelInfoBstautauCDFIso RelInfoBstautauCDFIso.h
 *
 * \brief Calculate track isolation.
 *    Compare daughter
 *
 *    m_bdt1 : bdt value
 *
 * Options:
 *
 *   None
 *
 *  Converted from ConeVariables by A. Shires 18/06/2014
 *
 *  @author Giampiero Mancinelli
 *  @date   2014-07-25
 *
 */

class IDVAlgorithm;
class IDistanceCalculator;
class IParticleDictTool ;

class RelInfoBstautauCDFIso : public GaudiTool, virtual public IRelatedInfoTool {
    public:
        /// Standard constructor
        RelInfoBstautauCDFIso( const std::string& type,
                const std::string& name,
                const IInterface* parent );

        virtual StatusCode initialize();

        virtual StatusCode calculateRelatedInfo( const LHCb::Particle*,
                                         const LHCb::Particle*);

        virtual LHCb::RelatedInfoMap* getInfo(void);
  
        virtual std::string infoPath(void);

        virtual ~RelInfoBstautauCDFIso( ); ///< Destructor

    private:

        std::vector<std::string> m_variables;
        LHCb::RelatedInfoMap m_map;
        std::vector<short int> m_keys; 

        double m_bdt1;

        std::string m_weightsName;
        /// TMVA transform
        //Reader
        //TMVA::Reader *m_Reader;
        std::string m_transformName ;
        //variables

  std::string m_TracksPath;
  std::string m_ParticlePath;
  std::string m_PVInputLocation;		\
        IParticleDictTool::DICT m_varmap ; 
        IParticleDictTool::DICT m_out ; 


        //save the vertice
        LHCb::RecVertex::Container * m_vertices ;

        std::vector<const LHCb::Particle*> m_decayParticles;

        /// Save all particles in your decay descriptor in a vector
        void saveDecayParticles( const LHCb::Particle *top);

        /// Check if your track belongs to your decay or not
        bool isTrackInDecay(const LHCb::Track* track);


        ///============================================================================
        /// Track isolation method
        ///============================================================================

        bool calcValue( const LHCb::Particle * part
                , const LHCb::Particles * tracks
                , const LHCb::VertexBase * PV
                , const LHCb::VertexBase * SV
                ) ;



};

#endif // CONEVARIABLES_H
