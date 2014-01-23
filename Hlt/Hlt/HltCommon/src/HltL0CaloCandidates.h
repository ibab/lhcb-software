// $Id: HltL0CaloCandidates.h,v 1.5 2009-12-23 16:14:31 graven Exp $
#ifndef HLTCOMMON_HLTL0ALOCANDIDATES_H
#define HLTCOMMON_HLTL0ALOCANDIDATES_H 1

// Include files
// from Gaudi
// #include "HltBase/HltSelectionContainer.hs"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/Property.h"

#include "Event/L0DUBase.h"
#include "Event/L0DUReport.h"
#include "Event/L0CaloCandidate.h"
#include "CaloInterfaces/ICaloClusterization.h"
#include "CaloInterfaces/IL0Calo2Calo.h"

/** @class HltL0CaloCandidates HltL0CaloCandidates.h
 *
 *
 *  @author Albert Puig Navarro
 *  @date   2011-04-14
 *  @based on J.A. Hernando's and Gerhard Raven's work
 *  @based on the ideas of the HltIsPhotonTool by M.Witek
 */

class SortL0CaloByEt
{
  public:
    bool operator()( const LHCb::L0CaloCandidate* c1,
                     const LHCb::L0CaloCandidate* c2 )
    {
        return c1->etCode() > c2->etCode();
    }
};

class HltL0CaloCandidates : public GaudiHistoAlg
{
  public:
    /// Standard constructor
    HltL0CaloCandidates( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~HltL0CaloCandidates(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution

  private:
    class L0CaloCandidateCut
    {
      public:
        L0CaloCandidateCut()
            : m_type( (L0DUBase::CaloType::Type)0 )
            , m_hasThreshold( false )
            , m_threshold( -1 )
        {
        }
        L0CaloCandidateCut( L0DUBase::CaloType::Type type )
            : m_type( type ), m_hasThreshold( false ), m_threshold( -1 )
        {
        }
        L0CaloCandidateCut( L0DUBase::CaloType::Type type, int threshold )
            : m_type( type ), m_hasThreshold( true ), m_threshold( threshold )
        {
        }
        bool operator()( const LHCb::L0CaloCandidate* calo ) const
        {
            return ( calo != 0 ) && ( calo->type() == m_type ) &&
                   ( !m_hasThreshold || calo->et() > m_threshold );
        }

      private:
        L0DUBase::CaloType::Type m_type;
        bool m_hasThreshold;
        int m_threshold;
    };
    bool checkID( LHCb::CaloCellID id, std::vector<LHCb::CaloCellID>& ids );

    // Configuration
    std::string m_l0Location;
    std::string m_TESprefix;
    int m_lowEtThreshold;
    int m_highEtThreshold;
    // Cuts
    std::map<L0DUBase::CaloType::Type, L0CaloCandidateCut> m_cutsLow;
    std::map<L0DUBase::CaloType::Type, L0CaloCandidateCut> m_cutsHigh;
    // For histos
    AIDA::IHistogram1D* m_et;
    AIDA::IHistogram1D* m_etMax;
    // Tools
    IL0Calo2Calo* m_l02CaloTool;
    // ICaloClusterization* m_clusterTool;
    int m_level;
    std::string m_l0loc;
};

#endif // HLTCOMMON_HLTL0ALOCANDIDATES_H

