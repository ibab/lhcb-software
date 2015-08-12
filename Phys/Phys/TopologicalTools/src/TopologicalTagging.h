// $Id: TopologicalTagging.h,v 1.3 2007/06/18 11:27:44 jpalac Exp $
// TopologicalTagging created 25.02.06
//
//  author: Cedric Potteraz
//  Based on FilterDesktop

#ifndef TOPOLOGICALTAGGING_H
#define TOPOLOGICALTAGGING_H 1

// ============================================================================
// Include files
// ============================================================================
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// DaVinci Kernel
// ============================================================================
#include "Kernel/DaVinciAlgorithm.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/RecHeader.h"


#include "Kernel/ITriggerTisTos.h"
#include "Kernel/IAccept.h"
#include "Event/HltObjectSummary.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
using namespace LHCb ;
using namespace std ;



// ============================================================================
// Main Algorithm
// ============================================================================
class TopologicalTagging :  public DaVinciAlgorithm
{
    friend class AlgFactory<TopologicalTagging> ;
    public:
    /** Standard constructor
     *  @param name instance name
     *  @param pSvc pointer to Service Locator
     */
    TopologicalTagging(const std::string& name,
            ISvcLocator* pSvcLocator) ;


    /// destructor
    virtual ~TopologicalTagging( ){};
    virtual StatusCode initialize ();

    /** standard execution of the algorithm
     *  @return status code
     */
    virtual StatusCode execute   () ;
    private:
    // the default constructor is disabled
    TopologicalTagging () ;
    // the copy constructor is disabled
    TopologicalTagging ( const  TopologicalTagging& )  ;
    // number of particle that have to verify the cut
    TopologicalTagging operator=( const  TopologicalTagging& )  ;

    protected:
    // ==========================================================================
    /// Write empty container if selection fails.
    void writeEmptyKeyedContainers  ( ) const;
    /// Write empty container if selection fails.
    void writeEmptySharedContainers ( ) const;
    // ==========================================================================
    private:
    // ==========================================================================
    /// save (clone if needed) selected particles in TES
    /// Success if number of saved particles == number saved
    /// to TES.
    /// Overwritten from DaVinciAlgorithm. Is called automatically.
    virtual StatusCode _saveInTES () ;
    /// Write empty containers if selection fails.
    virtual void writeEmptyTESContainers() ;

    private:
    // ============================================================================
    /// save (clone if needed) selected particles in TES
    template <class PARTICLES>
        StatusCode _save () const ;

    private:

    // name to retrieve tool
    std::string m_tagToolName;

    bool m_save_all;
    bool m_save_addinfo;
    int  m_start_addinfo;
    std::string  m_SubPID;
    double  m_cut;

    std::vector<LHCb::Particle*>  i_markedParticles;



    ITriggerTisTos* m_TriggerTisTosTool;
    public:
    std::string m_TLine; // algorithm mode - PtRel or IPSig
    std::string m_tinputs;
    int m_abspid;


    private:
    Gaudi::XYZVector getHltObjVtxPos (const LHCb::HltObjectSummary *obj) const;
    //StatusCode printObject(const LHCb::HltObjectSummary* sum, std::string increment) const;
    StatusCode getJetLHCbIDs(const LHCb::Particle* part, std::vector< LHCb::LHCbID > &AllIDs) const;
    StatusCode getHltObjLHCbIDs(const LHCb::HltObjectSummary* sum, std::vector< LHCb::LHCbID > &AllIDs) const;
    StatusCode printObject(const LHCb::HltObjectSummary* sum, std::string increment) const;
    void getTracks(const LHCb::Particle* p, LHCb::Track::ConstVector& tracks) const;


};


#endif // TOPOLOGICALTAGGING_H
