#ifndef TRACKISOVARIABLES_H 
#define TRACKISOVARIABLES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/GetIDVAlgorithm.h"
#include "Kernel/IExtraInfoTool.h"
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IDistanceCalculator.h"
//needed for TMVA
#include "MVADictTools/TMVATransform.h"
#include "Kernel/IParticleDictTool.h"
#include "Event/Particle.h"
#include "TMVA/Reader.h"

/** @class TrackIsoVariables TrackIsoVariables.h
 *
 * \brief Calculate track isolation.
 *    Compare daughter
 *
 *    m_bdt1 : highest ranked bdt value
 *    m_bdt2 : second highest ranked bdt value
 *    m_bdt2 : third highest ranked bdt value
 *
 * Options:
 *
 *   None
 *
 *  Converted from ConeVariables by A. Shires 18/06/2014
 *
 *  @author Alex Shires
 *  @date   2014-06-19
 *
 */

class IDVAlgorithm;
class IDistanceCalculator;
class IParticleDictTool ;

class TrackIsoVariables : public GaudiTool, virtual public IExtraInfoTool {
    public:
        /// Standard constructor
        TrackIsoVariables( const std::string& type,
                const std::string& name,
                const IInterface* parent );

        virtual StatusCode initialize();

        /// Loop over differnt conesizes and fill the variables into the tuple
        virtual StatusCode calculateExtraInfo( const LHCb::Particle*
                , const LHCb::Particle*);

        //pure virtual, needs impl
        virtual int getFirstIndex(void);

        virtual int getNumberOfParameters(void);

        virtual int getInfo(int index, double & value, std::string & name);

        virtual ~TrackIsoVariables( ); ///< Destructor

    protected:

    private:

        std::vector<std::string> m_variables;

        double m_bdt1;
        double m_bdt2;
        double m_bdt3;

        //distance calculator
        //
        IDVAlgorithm* m_dva;
        const IDistanceCalculator*  m_dist;

        std::string m_weightsName;
        /// TMVA transform
        //Reader
        //TMVA::Reader *m_Reader;
        std::string m_transformName ;
        //variables
        float var_log_doca ;
        float var_angle ;
        float var_fc ;
        float var_PVdist ;
        float var_SVdist ;
        float var_ipchisqany ;

        TMVATransform m_tmva ;
        TMVATransform::optmap m_optmap ;
        const IParticleDictTool m_varmap ; 
        const IParticleDictTool m_out ; 


        //save the vertices
        std::string m_PVInputLocation ;
        LHCb::RecVertex::Container * m_vertices ;

        std::vector<const LHCb::Particle*> m_decayParticles;

        /// Save all particles in your decay descriptor in a vector
        void saveDecayParticles( const LHCb::Particle *top);

        /// Check if your track belongs to your decay or not
        bool isTrackInDecay(const LHCb::Track* track);


        ///============================================================================
        /// Track isolation method
        ///============================================================================

        bool calcBDTValue( const LHCb::Particle * part
                , const LHCb::Tracks * tracks
                , const LHCb::VertexBase * PV
                , const LHCb::VertexBase * SV
                ) ;
        ///============================================================================
        /// Helper functions 
        ///============================================================================
        double calcIPToAnyPV( const LHCb::Track * track ) ;
        double  enclosedAngle( Gaudi::XYZVector, Gaudi::XYZVector );
        void    getPerpFeet(  Gaudi::XYZPoint track_pos,
                Gaudi::XYZVector track_p,
                Gaudi::XYZPoint mu_pos,
                Gaudi::XYZVector mu_p, 
                Gaudi::XYZPoint& perpFootTrack, 
                Gaudi::XYZPoint& perpFootMu, 
                Gaudi::XYZPoint& vertex, bool& fail); 
        double calcFC(  Gaudi::XYZVector track_mom, 
                Gaudi::XYZVector mu_mom, 
                Gaudi::XYZPoint mu_track_vertex,
                const LHCb::VertexBase* PV);
        double calcVertexDist(Gaudi::XYZPoint muTrack, 
                const LHCb::VertexBase* v);



};

#endif // CONEVARIABLES_H
