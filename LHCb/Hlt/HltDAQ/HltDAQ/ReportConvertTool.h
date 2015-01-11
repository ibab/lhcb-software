#ifndef REPORTCONVERTTOOL_H 
#define REPORTCONVERTTOOL_H 1

// Include files
// from STL
#include <string>
#include <unordered_map>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// From PhysEvent
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
#include "Event/RichPID.h"
#include "Event/MuonPID.h"
#include "Event/CaloCluster.h"
#include "Event/RecVertex.h"
#include "Event/Vertex.h"
#include "Event/State.h"

#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"
#include "Event/HltVertexReports.h"
#include "Event/HltDecReports.h"

#include "HltDAQ/HltSelRepRBStdInfo.h"
#include "HltDAQ/IReportConvert.h"
#include "Event/HltObjectSummary.h"

using std::unordered_map;
using std::string;
using std::pair;
using GaudiUtils::VectorMap;
using LHCb::HltObjectSummary;
using LHCb::HltSelRepRBStdInfo;

/** @class ReportConvertTool ReportConvertTool.h
 *  
 *  Tool to check if version number of the HltSelReports 
 *
 *  @author Sean Benson
 *  @date   02/10/2014
 */
class ReportConvertTool : public GaudiTool, virtual public IReportConvert
{
  public:
    typedef unordered_map<string,pair<int,int> >::const_iterator it_unordered_map;
    typedef unordered_map<int, unordered_map<string,pair<int,int> > >::const_iterator it_unordered_map2;
    
    /// Standard constructor
    ReportConvertTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);
    ///  Destructor
    virtual ~ReportConvertTool( ) { };
 
    void setReportVersion(int);
    void setReportVersionLatest();
    int getReportVersion();

    float floatFromInt(unsigned int);

    //===========================================================================
    /// Convert the sub bank to a HltObjectSummary.
    void SummaryFromRaw( HltObjectSummary::Info*, HltSelRepRBStdInfo::StdInfo*, int );

    /// Put the information in to the HltObjectSummary
    void ParticleObject2Summary( HltObjectSummary::Info*, const LHCb::Particle* , bool) ;
    void ProtoParticleObject2Summary( HltObjectSummary::Info*, const LHCb::ProtoParticle*, bool) ;
    void TrackObject2Summary( HltObjectSummary::Info*, const LHCb::Track*, bool) ;
    void RichPIDObject2Summary( HltObjectSummary::Info*, const LHCb::RichPID*, bool) ;
    void MuonPIDObject2Summary( HltObjectSummary::Info*, const LHCb::MuonPID*, bool) ;
    void CaloClusterObject2Summary( HltObjectSummary::Info*, const LHCb::CaloCluster*, bool) ;
    void RecVertexObject2Summary( HltObjectSummary::Info*, const LHCb::RecVertex*, bool) ;
    void VertexObject2Summary( HltObjectSummary::Info*, const LHCb::Vertex*, bool) ;
    //
    /// Put the information in the summary back in the object
    void ParticleObjectFromSummary( const HltObjectSummary::Info*, LHCb::Particle*,bool) ;
    void ProtoParticleObjectFromSummary( const HltObjectSummary::Info*, LHCb::ProtoParticle*,bool) ;
    void TrackObjectFromSummary( const HltObjectSummary::Info*, LHCb::Track*,bool) ;
    void RichPIDObjectFromSummary( const HltObjectSummary::Info*, LHCb::RichPID*,bool) ;
    void MuonPIDObjectFromSummary( const HltObjectSummary::Info*, LHCb::MuonPID*,bool) ;
    void CaloClusterObjectFromSummary( const HltObjectSummary::Info*, LHCb::CaloCluster*,bool) ;
    void RecVertexObjectFromSummary( const HltObjectSummary::Info*, LHCb::RecVertex*,bool) ;
    void VertexObjectFromSummary( const HltObjectSummary::Info*, LHCb::Vertex*,bool) ;

  protected:
  
  private:
    int m_version;
    int m_LatestVersion;

    //===========================================================================
    /// Version unordered_map for LHCb::Particle in the Turbo stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_particle_unordered_map2_Turbo {
      { 3
        , {{"0#Particle.particleID.pid",                {0, 0}}
          , {"1#Particle.measuredMass",                 {1, 1}}
          , {"2#Particle.referencePoint.z",            {12, 2}}
          , {"3#Particle.referencePoint.x",            {23, 3}}
          , {"4#Particle.referencePoint.y",            {32, 4}}
          , {"5#Particle.slopes.x",                    {33, 5}}
          , {"6#Particle.slopes.y",                    {34, 6}}
          , {"7#Particle.1/p",                         {35, 7}}
          , {"8#Particle.conflevel",                   {36, 8}}
          , {"9#Particle.massErr",                     {37, 9}}
          , {"10#Particle.momCov00",                   {2, 10}}
          , {"11#Particle.momCov11",                   {3, 11}}
          , {"12#Particle.momCov22",                   {4, 12}}
          , {"13#Particle.momCov33",                    {5,13}}
          , {"14#Particle.momCov10",                    {6,14}}
          , {"15#Particle.momCov20",                    {7,15}}
          , {"16#Particle.momCov21",                    {8,16}}
          , {"17#Particle.momCov30",                    {9,17}}
          , {"18#Particle.momCov31",                   {10,18}}
          , {"19#Particle.momCov32",                   {11,19}}
          , {"20#Particle.posmomCov00",                {13,20}}
          , {"21#Particle.posmomCov11",                {14,21}}
          , {"22#Particle.posmomCov22",                {15,22}}
          , {"23#Particle.posmomCov10",                {16,23}}
          , {"24#Particle.posmomCov01",                {17,24}}
          , {"25#Particle.posmomCov20",                {18,25}}
          , {"26#Particle.posmomCov02",                {19,26}}
          , {"27#Particle.posmomCov21",                {20,27}}
          , {"28#Particle.posmomCov12",                {21,28}}
          , {"29#Particle.posmomCov30",                {22,29}}
          , {"30#Particle.posmomCov31",                {24,30}}
          , {"31#Particle.posmomCov32",                {25,31}}
          , {"32#Particle.posCov00",                   {26,32}}
          , {"33#Particle.posCov11",                   {27,33}}
          , {"34#Particle.posCov22",                   {28,34}}
          , {"35#Particle.posCov10",                   {29,35}}
          , {"36#Particle.posCov20",                   {30,36}}
          , {"37#Particle.posCov21",                   {31,37}}}
      }
    };
    //===========================================================================
    /// Version unordered_map for LHCb::Particle in the Full stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_particle_unordered_map2 {
      { 1
        , {{"0#Particle.particleID.pid",   {0,0}}
          , {"1#Particle.measuredMass",     {1,1}}
          , {"2#Particle.referencePoint.z", {2,2}}
          , {"3#Particle.referencePoint.x", {3,3}}
          , {"4#Particle.referencePoint.y", {4,4}}
          , {"5#Particle.slopes.x",         {5,5}}
          , {"6#Particle.slopes.y",         {6,6}}
          , {"7#Particle.1/p",              {7,7}}}
      }
      , { 2
        , {{"0#Particle.particleID.pid",   {0,0}}
          , {"1#Particle.measuredMass",     {1,1}}
          , {"2#Particle.referencePoint.z", {2,2}}
          , {"3#Particle.referencePoint.x", {3,3}}
          , {"4#Particle.referencePoint.y", {4,4}}
          , {"5#Particle.slopes.x",         {5,5}}
          , {"6#Particle.slopes.y",         {6,6}}
          , {"7#Particle.1/p",              {7,7}}}
      }
      , { 3
        , {{"0#Particle.particleID.pid",   {0,0}}
          , {"1#Particle.measuredMass",     {1,1}}
          , {"2#Particle.referencePoint.z", {2,2}}
          , {"3#Particle.referencePoint.x", {3,3}}
          , {"4#Particle.referencePoint.y", {4,4}}
          , {"5#Particle.slopes.x",         {5,5}}
          , {"6#Particle.slopes.y",         {6,6}}
          , {"7#Particle.1/p",              {7,7}}}
      }
    };

    //===========================================================================
    /// Version unordered_map for LHCb::ProtoParticle in the Turbo stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_proto_unordered_map2_Turbo {
      { 3
        , {{"0#Proto.extraInfo.IsPhoton",               {0,0}}}
      }
    };
    //===========================================================================
    /// Version unordered_map for LHCb::ProtoParticle in the Full stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_proto_unordered_map2 {
    };

    //===========================================================================
    /// Version unordered_map for LHCb::Track for the Turbo stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_track_unordered_map2_Turbo {
      { 3
        , {{"0#Track.firstState.z",             {0,0}}
          , {"1#Track.firstState.x",            {1,1}}
          , {"2#Track.firstState.y",            {9,2}}
          , {"3#Track.firstState.tx",          {10,3}}
          , {"4#Track.firstState.ty",          {11,4}}
          , {"5#Track.firstState.qOverP",      {12,5}}
          , {"6#Track.chi2PerDoF",             {13,6}}
          , {"7#Track.nDoF",                   {14,7}}
          , {"8#Track.Likelihood",             {15,8}}
          , {"9#Track.GhostProb",              {16,9}}
          , {"10#Track.flags",                 {2,10}}
          , {"11#Track.lastState.z",           {3,11}}
          , {"12#Track.lastState.x",           {4,12}}
          , {"13#Track.lastState.y",           {5,13}}
          , {"14#Track.lastState.tx",          {6,14}}
          , {"15#Track.lastState.ty",          {7,15}}
          , {"16#Track.lastState.qOverP",      {8,16}}}
      }
    };
    //===========================================================================
    /// Version unordered_map for LHCb::Track
    unordered_map<int, unordered_map<string,pair<int, int> > > const m_track_unordered_map2 {
      { 1
        , {{"0#Track.firstState.z",             {0,0}}
          , {"1#Track.firstState.x",            {1,1}}
          , {"2#Track.firstState.y",            {2,2}}
          , {"3#Track.firstState.tx",           {3,3}}
          , {"4#Track.firstState.ty",           {4,4}}
          , {"5#Track.firstState.qOverP",       {5,5}}}
      }
      , { 2
        , {{"0#Track.firstState.z",             {0,0}}
          , {"1#Track.firstState.x",            {1,1}}
          , {"2#Track.firstState.y",            {2,2}}
          , {"3#Track.firstState.tx",           {3,3}}
          , {"4#Track.firstState.ty",           {4,4}}
          , {"5#Track.firstState.qOverP",       {5,5}}
          , {"6#Track.chi2PerDoF",              {6,6}}
          , {"7#Track.nDoF",                    {7,7}}}
      }
      , { 3
        , {{"0#Track.firstState.z",             {0,0}}
          , {"1#Track.firstState.x",            {1,1}}
          , {"2#Track.firstState.y",            {2,2}}
          , {"3#Track.firstState.tx",           {3,3}}
          , {"4#Track.firstState.ty",           {4,4}}
          , {"5#Track.firstState.qOverP",       {5,5}}
          , {"6#Track.chi2PerDoF",              {6,6}}
          , {"7#Track.nDoF",                    {7,7}}}
      }
    };

    //===========================================================================
    /// Version unordered_map for LHCb::RichPID in the Turbo stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_rpid_unordered_map2_Turbo {
      { 3
        , {{"0#Rich.pidResultCode",             {0,0}}
          , {"1#Rich.DLLe",                     {1,1}}
          , {"2#Rich.DLLmu",                    {2,2}}
          , {"3#Rich.DLLpi",                    {3,3}}
          , {"4#Rich.DLLK",                     {4,4}}
          , {"5#Rich.DLLp",                     {5,5}}}
      }
    };
    //===========================================================================
    /// Version unordered_map for LHCb::RichPID in the Full stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_rpid_unordered_map2 {
    };

    //===========================================================================
    /// Version unordered_map for LHCb::MuonPID in the Turbo stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_mpid_unordered_map2_Turbo {
      { 3
        , {{"0#Muon.MuonLLMu",                  {0,0}}
          , {"1#Muon.MuonLLBg",                 {1,1}}
          , {"2#Muon.NShared",                  {2,2}}
          , {"3#Muon.Status",                   {3,3}}
          , {"4#Muon.IsMuon",                   {4,4}}
          , {"5#Muon.IsMuonLoose",              {5,5}}
          , {"6#Muon.IsMuonTight",              {6,6}}}
      }
    };
    //===========================================================================
    /// Version unordered_map for LHCb::MuonPID in the Full stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_mpid_unordered_map2 {
    };

    //===========================================================================
    /// Version unordered_map for LHCb::CaloCluster in the Turbo stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_calo_unordered_map2_Turbo {
      { 3
        , {{"0#CaloCluster.e",                        {0,0}}
          , {"1#CaloCluster.position.x",              {1,1}}
          , {"2#CaloCluster.position.y",              {2,2}}
          , {"3#CaloCluster.position.z",              {3,3}}}
      }
    };
    //===========================================================================
    /// Version unordered_map for LHCb::CaloCluster in the Full stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_calo_unordered_map2 {
      { 1
        , {{"0#CaloCluster.e",                        {0,0}}
          , {"1#CaloCluster.position.x",              {1,1}}
          , {"2#CaloCluster.position.y",              {2,2}}
          , {"3#CaloCluster.position.z",              {3,3}}}
      }
      , { 2
        , {{"0#CaloCluster.e",                        {0,0}}
          , {"1#CaloCluster.position.x",              {1,1}}
          , {"2#CaloCluster.position.y",              {2,2}}
          , {"3#CaloCluster.position.z",              {3,3}}}
      }
      , { 3
        , {{"0#CaloCluster.e",                        {0,0}}
          , {"1#CaloCluster.position.x",              {1,1}}
          , {"2#CaloCluster.position.y",              {2,2}}
          , {"3#CaloCluster.position.z",              {3,3}}}
      }
    };

    //===========================================================================
    /// Version unordered_map for LHCb::RecVertex in the Turbo stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_recvertex_unordered_map2_Turbo {
      { 3
        , {{"0#RecVertex.position.x",                 {0,0}}
          , {"1#RecVertex.position.y",                {1,1}}
          , {"2#RecVertex.position.z",                {2,2}}
          , {"3#RecVertex.chi2",                      {3,3}}}
      }
    };
    //===========================================================================
    /// Version unordered_map for LHCb::RecVertex in the Full stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_recvertex_unordered_map2 {
      { 1
        , {{"0#RecVertex.position.x",                 {0,0}}
          , {"1#RecVertex.position.y",                {1,1}}
          , {"2#RecVertex.position.z",                {2,2}}}
      }
      , { 2
        , {{"0#RecVertex.position.x",                 {0,0}}
          , {"1#RecVertex.position.y",                {1,1}}
          , {"2#RecVertex.position.z",                {2,2}}
          , {"3#RecVertex.chi2",                      {3,3}}}
      }
      , { 3
        , {{"0#RecVertex.position.x",                 {0,0}}
          , {"1#RecVertex.position.y",                {1,1}}
          , {"2#RecVertex.position.z",                {2,2}}
          , {"3#RecVertex.chi2",                      {3,3}}}
      }
    };

    //===========================================================================
    /// Version unordered_map for LHCb::Vertex in the Turbo stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_vertex_unordered_map2_Turbo {
      { 3
        , {{"0#Vertex.chi2",                {0,0}}
          , {"1#Vertex.ndf",                {1,1}}
          , {"2#Vertex.position.x",         {4,2}}
          , {"3#Vertex.position.y",         {5,3}}
          , {"4#Vertex.position.z",         {6,4}}
          , {"5#Vertex.technique",          {7,5}}
          , {"6#Vertex.cov00",              {8,6}}
          , {"7#Vertex.cov11",              {9,7}}
          , {"8#Vertex.cov22",             {10,8}}
          , {"9#Vertex.cov10",             {11,9}}
          , {"10#Vertex.cov20",             {2,10}}
          , {"11#Vertex.cov21",             {3,11}}}
      }
    };
    //===========================================================================
    /// Version unordered_map for LHCb::Vertex in the Full stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_vertex_unordered_map2 {
    };
}; // End of class header. 

#endif // REPORTCONVERTTOOL_H
