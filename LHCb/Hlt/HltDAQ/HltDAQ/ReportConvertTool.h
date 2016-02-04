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
    int getLatestVersion();
    int getSizeSelRepParticleLatest();

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
    void CaloHypoObject2Summary( HltObjectSummary::Info*, const LHCb::CaloHypo*, bool) ;
    void RecVertexObject2Summary( HltObjectSummary::Info*, const LHCb::RecVertex*, bool) ;
    void VertexObject2Summary( HltObjectSummary::Info*, const LHCb::Vertex*, bool) ;
    void RecSummaryObject2Summary( HltObjectSummary::Info*, const LHCb::RecSummary*) ;
    void GenericMapObject2Summary( HltObjectSummary::Info*, const GaudiUtils::VectorMap<short,float>*) ;
    //
    /// Put the information in the summary back in the object
    void ParticleObjectFromSummary( const HltObjectSummary::Info*, LHCb::Particle*,bool) ;
    void ProtoParticleObjectFromSummary( const HltObjectSummary::Info*, LHCb::ProtoParticle*,bool) ;
    void TrackObjectFromSummary( const HltObjectSummary::Info*, LHCb::Track*,bool) ;
    void RichPIDObjectFromSummary( const HltObjectSummary::Info*, LHCb::RichPID*,bool) ;
    void MuonPIDObjectFromSummary( const HltObjectSummary::Info*, LHCb::MuonPID*,bool) ;
    void CaloClusterObjectFromSummary( const HltObjectSummary::Info*, LHCb::CaloCluster*,bool) ;
    void CaloHypoObjectFromSummary( const HltObjectSummary::Info*, LHCb::CaloHypo*,bool) ;
    void RecVertexObjectFromSummary( const HltObjectSummary::Info*, LHCb::RecVertex*,bool) ;
    void VertexObjectFromSummary( const HltObjectSummary::Info*, LHCb::Vertex*,bool) ;
    void RecSummaryObjectFromSummary( const HltObjectSummary::Info*, LHCb::RecSummary*) ;
    void GenericMapObjectFromSummary( const HltObjectSummary::Info*, GaudiUtils::VectorMap<short,float>*) ;

    int findBestPrevious(unordered_map<int, unordered_map<string,pair<int,int> > >,int) ;
    

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
      , { 4
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
      , { 6
        , {{"0#Particle.particleID.pid",                {0, 0}}
          , {"1#Particle.measuredMass",                 {1, 1}}
          , {"2#Particle.referencePoint.z",            {12, 2}}
          , {"3#Particle.referencePoint.x",            {23, 3}}
          , {"4#Particle.referencePoint.y",            {34, 4}}
          , {"5#Particle.slopes.x",                    {37, 5}}
          , {"6#Particle.slopes.y",                    {38, 6}}
          , {"7#Particle.1/p",                         {39, 7}}
          , {"8#Particle.conflevel",                   {40, 8}}
          , {"9#Particle.massErr",                     {41, 9}}
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
          , {"37#Particle.posCov21",                   {31,37}}
          , {"38#Particle.raw.m",                      {32,38}}
          , {"39#Particle.raw.p1",                     {33,39}}
          , {"40#Particle.raw.p2",                     {35,40}}
          , {"41#Particle.raw.p3",                     {36,41}}}
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
      , { 4
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
      , { 4
        , {{"0#Proto.extraInfo.IsPhoton",               {0,0}}
        ,  {"1#Proto.extraInfo.IsNotE",                 {1,1}}
        ,  {"2#Proto.extraInfo.IsNotH",                 {12,2}}
        ,  {"3#Proto.extraInfo.EcalPIDe",               {23,3}}
        ,  {"4#Proto.extraInfo.PrsPIDe",                {34,4}}
        ,  {"5#Proto.extraInfo.BremPIDe",               {45,5}}
        ,  {"6#Proto.extraInfo.HcalPIDe",               {51,6}}
        ,  {"7#Proto.extraInfo.HcalPIDmu",              {52,7}}
        ,  {"8#Proto.extraInfo.EcalPIDmu",              {53,8}}
        ,  {"9#Proto.extraInfo.CaloTrMatch",            {54,9}}
        ,  {"10#Proto.extraInfo.CaloElectronMatch",     {2,10}}
        ,  {"11#Proto.extraInfo.CaloBremMatch",         {3,11}}
        ,  {"12#Proto.extraInfo.CaloNeutralSpd",        {4,12}}
        ,  {"13#Proto.extraInfo.CaloNeutralPrs",        {5,13}}
        ,  {"14#Proto.extraInfo.CaloNeutralEcal",       {6,14}}
        ,  {"15#Proto.extraInfo.CaloNeutralHcal2Ecal",  {7,15}}
        ,  {"16#Proto.extraInfo.CaloNeutralE49",        {8,16}}
        ,  {"17#Proto.extraInfo.CaloNeutralID",         {9,17}}
        ,  {"18#Proto.extraInfo.CaloDepositID",         {10,18}}
        ,  {"19#Proto.extraInfo.ShowerShape",           {11,19}}
        ,  {"20#Proto.extraInfo.ClusterMass",           {13,20}}
        ,  {"21#Proto.extraInfo.CaloSpdE",              {14,21}}
        ,  {"22#Proto.extraInfo.CaloPrsE",              {15,22}}
        ,  {"23#Proto.extraInfo.CaloEcalE",             {16,23}}
        ,  {"24#Proto.extraInfo.CaloHcalE",             {17,24}}
        ,  {"25#Proto.extraInfo.CaloEcalChi2",          {18,25}}
        ,  {"26#Proto.extraInfo.CaloBremChi2",          {19,26}}
        ,  {"27#Proto.extraInfo.CaloClusChi2",          {20,27}}
        ,  {"28#Proto.extraInfo.CaloNeutralPrsM",       {21,28}}
        ,  {"29#Proto.extraInfo.CaloShapeFr2r4",        {22,29}}
        ,  {"30#Proto.extraInfo.CaloShapeKappa",        {24,30}}
        ,  {"31#Proto.extraInfo.CaloShapeAsym",         {25,31}}
        ,  {"32#Proto.extraInfo.CaloShapeE1",           {26,32}}
        ,  {"33#Proto.extraInfo.CaloShapeE2",           {27,33}}
        ,  {"34#Proto.extraInfo.CaloPrsShapeE2",        {28,34}}
        ,  {"35#Proto.extraInfo.CaloPrsShapeEmax",      {29,35}}
        ,  {"36#Proto.extraInfo.CaloPrsShapeFr2",       {30,36}}
        ,  {"37#Proto.extraInfo.CaloPrsShapeAsym",      {31,37}}
        ,  {"38#Proto.extraInfo.CaloPrsM",              {32,38}}
        ,  {"39#Proto.extraInfo.CaloPrsM15",            {33,39}}
        ,  {"40#Proto.extraInfo.CaloPrsM30",            {35,40}}
        ,  {"41#Proto.extraInfo.CaloPrsM45",            {36,41}}
        ,  {"42#Proto.extraInfo.CaloClusterCode",       {37,42}}
        ,  {"43#Proto.extraInfo.CaloClusterFrac",       {38,43}}
        ,  {"44#Proto.extraInfo.CombDLLe",              {39,44}}
        ,  {"45#Proto.extraInfo.CombDLLmu",             {40,45}}
        ,  {"46#Proto.extraInfo.CombDLLpi",             {41,46}}
        ,  {"47#Proto.extraInfo.CombDLLk",              {42,47}}
        ,  {"48#Proto.extraInfo.CombDLLp",              {43,48}}
        ,  {"49#Proto.extraInfo.InAccBrem",             {44,49}}
        ,  {"50#Proto.extraInfo.InAccSpd",              {46,50}}
        ,  {"51#Proto.extraInfo.InAccPrs",              {47,51}}
        ,  {"52#Proto.extraInfo.InAccEcal",             {48,52}}
        ,  {"53#Proto.extraInfo.InAccHcal",             {49,53}}
        ,  {"54#Proto.extraInfo.VeloCharge",            {50,54}}}
      }
      , { 8
        , {{"0#Proto.extraInfo.IsPhoton",               {0,0}}
        ,  {"1#Proto.extraInfo.IsNotE",                 {1,1}}
        ,  {"2#Proto.extraInfo.IsNotH",                 {12,2}}
        ,  {"3#Proto.extraInfo.EcalPIDe",               {23,3}}
        ,  {"4#Proto.extraInfo.PrsPIDe",                {34,4}}
        ,  {"5#Proto.extraInfo.BremPIDe",               {45,5}}
        ,  {"6#Proto.extraInfo.HcalPIDe",               {56,6}}
        ,  {"7#Proto.extraInfo.HcalPIDmu",              {57,7}}
        ,  {"8#Proto.extraInfo.EcalPIDmu",              {58,8}}
        ,  {"9#Proto.extraInfo.CaloTrMatch",            {59,9}}
        ,  {"10#Proto.extraInfo.CaloElectronMatch",     {2,10}}
        ,  {"11#Proto.extraInfo.CaloBremMatch",         {3,11}}
        ,  {"12#Proto.extraInfo.CaloNeutralSpd",        {4,12}}
        ,  {"13#Proto.extraInfo.CaloNeutralPrs",        {5,13}}
        ,  {"14#Proto.extraInfo.CaloNeutralEcal",       {6,14}}
        ,  {"15#Proto.extraInfo.CaloNeutralHcal2Ecal",  {7,15}}
        ,  {"16#Proto.extraInfo.CaloNeutralE49",        {8,16}}
        ,  {"17#Proto.extraInfo.CaloNeutralID",         {9,17}}
        ,  {"18#Proto.extraInfo.CaloDepositID",         {10,18}}
        ,  {"19#Proto.extraInfo.ShowerShape",           {11,19}}
        ,  {"20#Proto.extraInfo.ClusterMass",           {13,20}}
        ,  {"21#Proto.extraInfo.CaloSpdE",              {14,21}}
        ,  {"22#Proto.extraInfo.CaloPrsE",              {15,22}}
        ,  {"23#Proto.extraInfo.CaloEcalE",             {16,23}}
        ,  {"24#Proto.extraInfo.CaloHcalE",             {17,24}}
        ,  {"25#Proto.extraInfo.CaloEcalChi2",          {18,25}}
        ,  {"26#Proto.extraInfo.CaloBremChi2",          {19,26}}
        ,  {"27#Proto.extraInfo.CaloClusChi2",          {20,27}}
        ,  {"28#Proto.extraInfo.CaloNeutralPrsM",       {21,28}}
        ,  {"29#Proto.extraInfo.CaloShapeFr2r4",        {22,29}}
        ,  {"30#Proto.extraInfo.CaloShapeKappa",        {24,30}}
        ,  {"31#Proto.extraInfo.CaloShapeAsym",         {25,31}}
        ,  {"32#Proto.extraInfo.CaloShapeE1",           {26,32}}
        ,  {"33#Proto.extraInfo.CaloShapeE2",           {27,33}}
        ,  {"34#Proto.extraInfo.CaloPrsShapeE2",        {28,34}}
        ,  {"35#Proto.extraInfo.CaloPrsShapeEmax",      {29,35}}
        ,  {"36#Proto.extraInfo.CaloPrsShapeFr2",       {30,36}}
        ,  {"37#Proto.extraInfo.CaloPrsShapeAsym",      {31,37}}
        ,  {"38#Proto.extraInfo.CaloPrsM",              {32,38}}
        ,  {"39#Proto.extraInfo.CaloPrsM15",            {33,39}}
        ,  {"40#Proto.extraInfo.CaloPrsM30",            {35,40}}
        ,  {"41#Proto.extraInfo.CaloPrsM45",            {36,41}}
        ,  {"42#Proto.extraInfo.CaloClusterCode",       {37,42}}
        ,  {"43#Proto.extraInfo.CaloClusterFrac",       {38,43}}
        ,  {"44#Proto.extraInfo.CombDLLe",              {39,44}}
        ,  {"45#Proto.extraInfo.CombDLLmu",             {40,45}}
        ,  {"46#Proto.extraInfo.CombDLLpi",             {41,46}}
        ,  {"47#Proto.extraInfo.CombDLLk",              {42,47}}
        ,  {"48#Proto.extraInfo.CombDLLp",              {43,48}}
        ,  {"49#Proto.extraInfo.InAccBrem",             {44,49}}
        ,  {"50#Proto.extraInfo.InAccSpd",              {46,50}}
        ,  {"51#Proto.extraInfo.InAccPrs",              {47,51}}
        ,  {"52#Proto.extraInfo.InAccEcal",             {48,52}}
        ,  {"53#Proto.extraInfo.InAccHcal",             {49,53}}
        ,  {"54#Proto.extraInfo.VeloCharge",            {50,54}}
        ,  {"55#Proto.extraInfo.RichPIDStatus",         {51,55}}
        ,  {"56#Proto.extraInfo.CaloChargedID",         {52,56}}
        ,  {"57#Proto.extraInfo.CaloChargedEcal",       {53,57}}
        ,  {"58#Proto.extraInfo.CaloChargedPrs",        {54,58}}
        ,  {"59#Proto.extraInfo.CaloChargedSpd",        {55,59}}}
      }
    };
    //===========================================================================
    /// Version unordered_map for LHCb::ProtoParticle in the Full stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_proto_unordered_map2 {
      { 4
        , {{"0#Proto.extraInfo.CombDLLe",              {0,44}}
        ,  {"1#Proto.extraInfo.CombDLLmu",             {1,45}}
        ,  {"2#Proto.extraInfo.CombDLLpi",             {2,46}}
        ,  {"3#Proto.extraInfo.CombDLLk",              {3,47}}
        ,  {"4#Proto.extraInfo.CombDLLp",              {4,48}}
        ,  {"5#Proto.extraInfo.RichPIDStatus",         {5,55}}}
      }
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
      , { 4
        , {{"0#Track.firstState.z",            {0,0}}
          , {"1#Track.firstState.x",           {1,1}}
          , {"2#Track.firstState.y",           {12,2}}
          , {"3#Track.firstState.tx",          {16,3}}
          , {"4#Track.firstState.ty",          {17,4}}
          , {"5#Track.firstState.qOverP",      {18,5}}
          , {"6#Track.chi2PerDoF",             {19,6}}
          , {"7#Track.nDoF",                   {20,7}}
          , {"8#Track.Likelihood",             {21,8}}
          , {"9#Track.GhostProb",              {22,9}}
          , {"10#Track.flags",                 {2,10}}
          , {"11#Track.lastState.z",           {3,11}}
          , {"12#Track.lastState.x",           {4,12}}
          , {"13#Track.lastState.y",           {5,13}}
          , {"14#Track.lastState.tx",          {6,14}}
          , {"15#Track.lastState.ty",          {7,15}}
          , {"16#Track.lastState.qOverP",      {8,16}}
          , {"17#Track.CloneDist",             {9,17}}
          , {"18#Track.FitMatchChi2",          {10,18}}
          , {"19#Track.FitVeloChi2",           {11,19}}
          , {"20#Track.FitTChi2",              {13,20}}
          , {"21#Track.FitVeloNDoF",           {14,21}}
          , {"22#Track.FitTNDoF",              {15,22}}}
      }
      , { 5
        , {{"0#Track.firstState.z",            {0,0}}
          , {"1#Track.firstState.x",           {1,1}}
          , {"2#Track.firstState.y",           {12,2}}
          , {"3#Track.firstState.tx",          {23,3}}
          , {"4#Track.firstState.ty",          {34,4}}
          , {"5#Track.firstState.qOverP",      {35,5}}
          , {"6#Track.chi2PerDoF",             {36,6}}
          , {"7#Track.nDoF",                   {37,7}}
          , {"8#Track.Likelihood",             {38,8}}
          , {"9#Track.GhostProb",              {39,9}}
          , {"10#Track.flags",                 {2,10}}
          , {"11#Track.lastState.z",           {3,11}}
          , {"12#Track.lastState.x",           {4,12}}
          , {"13#Track.lastState.y",           {5,13}}
          , {"14#Track.lastState.tx",          {6,14}}
          , {"15#Track.lastState.ty",          {7,15}}
          , {"16#Track.lastState.qOverP",      {8,16}}
          , {"17#Track.CloneDist",             {9,17}}
          , {"18#Track.FitMatchChi2",          {10,18}}
          , {"19#Track.FitVeloChi2",           {11,19}}
          , {"20#Track.FitTChi2",              {13,20}}
          , {"21#Track.FitVeloNDoF",           {14,21}}
          , {"22#Track.FitTNDoF",              {15,22}}
          , {"23#Track.firstStateFlags",       {16,23}}
          , {"24#Track.lastStateFlags",        {17,24}}
          , {"25#Track.firstStateCov00",       {18,25}}
          , {"26#Track.firstStateCov11",       {19,26}}
          , {"27#Track.firstStateCov22",       {20,27}}
          , {"28#Track.firstStateCov33",       {21,28}}
          , {"29#Track.firstStateCov44",       {22,29}}
          , {"30#Track.firstStateCov01",       {24,30}}
          , {"31#Track.firstStateCov02",       {25,31}}
          , {"32#Track.firstStateCov03",       {26,32}}
          , {"33#Track.firstStateCov04",       {27,33}}
          , {"34#Track.firstStateCov12",       {28,34}}
          , {"35#Track.firstStateCov13",       {29,35}}
          , {"36#Track.firstStateCov14",       {30,36}}
          , {"37#Track.firstStateCov23",       {31,37}}
          , {"38#Track.firstStateCov24",       {32,38}}
          , {"39#Track.firstStateCov34",       {33,39}}}
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
      , { 4
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
      , { 4
        , {{"0#Rich.pidResultCode",             {0,0}}
          , {"1#Rich.DLLe",                     {1,1}}
          , {"2#Rich.DLLmu",                    {2,2}}
          , {"3#Rich.DLLpi",                    {3,3}}
          , {"4#Rich.DLLK",                     {4,4}}
          , {"5#Rich.DLLp",                     {5,5}}
          , {"6#Rich.BelowThreshold",           {6,6}}}
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
      , { 4
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
      { 4
        , {{"4#Muon.IsMuon",                   {0,4}}}
      }
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
      , { 4
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
      , { 4
        , {{"0#CaloCluster.e",                        {0,0}}
          , {"1#CaloCluster.position.x",              {1,1}}
          , {"2#CaloCluster.position.y",              {2,2}}
          , {"3#CaloCluster.position.z",              {3,3}}}
      }
    };
    
    //===========================================================================
    /// Version unordered_map for LHCb::CaloHypo in the Turbo stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_calohypo_unordered_map2_Turbo {
      { 8
        , {{"0#CaloHypo.e",                        {0,0}}
          , {"1#CaloHypo.position.x",              {1,1}}
          , {"2#CaloHypo.position.y",              {2,2}}
          , {"3#CaloHypo.position.z",              {3,3}}
          , {"4#CaloHypo.lh",                      {4,4}}
          , {"5#CaloHypo.h",                       {5,5}}}
      }
    };
    //===========================================================================
    /// Version unordered_map for LHCb::CaloHypo in the Full stream
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_calohypo_unordered_map2 {
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
      , { 4
        , {{"0#RecVertex.position.x",                 {0,0}}
          , {"1#RecVertex.position.y",                {1,1}}
          , {"2#RecVertex.position.z",                {4,2}}
          , {"3#RecVertex.chi2",                      {5,3}}
          , {"4#RecVertex.ndf",                       {6,4}}
          , {"5#RecVertex.technique",                 {7,5}}
          , {"6#RecVertex.cov00",                     {8,6}}
          , {"7#RecVertex.cov11",                     {9,7}}
          , {"8#RecVertex.cov22",                     {10,8}}
          , {"9#RecVertex.cov10",                     {11,9}}
          , {"10#RecVertex.cov20",                    {2,10}}
          , {"11#RecVertex.cov21",                    {3,11}}}
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
      , { 4
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
      , { 4
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

    //===========================================================================
    /// Version unordered_map for LHCb::RecSummary
    unordered_map<int, unordered_map<string,pair<int,int> > > const m_recsummary_unordered_map2 {
      { 4
        , {{"0#RecSummary.nLongTracks",                {0,0}}
          , {"1#RecSummary.nDownstreamTracks",         {1,1}}
          , {"2#RecSummary.nUpstreamTracks",           {12,2}}
          , {"3#RecSummary.nVeloTracks",               {15,3}}
          , {"4#RecSummary.nTTracks",                  {16,4}}
          , {"5#RecSummary.nBackTracks",               {17,5}}
          , {"6#RecSummary.nTracks",                   {18,6}}
          , {"7#RecSummary.nRich1Hits",                {19,7}}
          , {"8#RecSummary.nRich2Hits",                {20,8}}
          , {"9#RecSummary.nVeloClusters",             {21,9}}
          , {"10#RecSummary.nITClusters",              {2,10}}
          , {"11#RecSummary.nTTClusters",              {3,11}}
          , {"12#RecSummary.nUTClusters",              {4,12}}
          , {"13#RecSummary.nOTClusters",              {5,13}}
          , {"14#RecSummary.nFTClusters",              {6,14}}
          , {"15#RecSummary.nSPDhits",                 {7,15}}
          , {"16#RecSummary.nMuonCoordsS0",            {8,16}}
          , {"17#RecSummary.nMuonCoordsS1",            {9,17}}
          , {"18#RecSummary.nMuonCoordsS2",            {10,18}}
          , {"19#RecSummary.nMuonCoordsS3",            {11,19}}
          , {"20#RecSummary.nMuonCoordsS4",            {13,20}}
          , {"21#RecSummary.nMuonTracks",              {14,21}}}
      }
      , { 6
        , {{"0#RecSummary.nLongTracks",                {0,0}}
          , {"1#RecSummary.nDownstreamTracks",         {1,1}}
          , {"2#RecSummary.nUpstreamTracks",           {12,2}}
          , {"3#RecSummary.nVeloTracks",               {16,3}}
          , {"4#RecSummary.nTTracks",                  {17,4}}
          , {"5#RecSummary.nBackTracks",               {18,5}}
          , {"6#RecSummary.nTracks",                   {19,6}}
          , {"7#RecSummary.nRich1Hits",                {20,7}}
          , {"8#RecSummary.nRich2Hits",                {21,8}}
          , {"9#RecSummary.nVeloClusters",             {22,9}}
          , {"10#RecSummary.nITClusters",              {2,10}}
          , {"11#RecSummary.nTTClusters",              {3,11}}
          , {"12#RecSummary.nUTClusters",              {4,12}}
          , {"13#RecSummary.nOTClusters",              {5,13}}
          , {"14#RecSummary.nFTClusters",              {6,14}}
          , {"15#RecSummary.nSPDhits",                 {7,15}}
          , {"16#RecSummary.nMuonCoordsS0",            {8,16}}
          , {"17#RecSummary.nMuonCoordsS1",            {9,17}}
          , {"18#RecSummary.nMuonCoordsS2",            {10,18}}
          , {"19#RecSummary.nMuonCoordsS3",            {11,19}}
          , {"20#RecSummary.nMuonCoordsS4",            {13,20}}
          , {"21#RecSummary.nMuonTracks",              {14,21}}
          , {"22#RecSummary.nPVs",                     {15,22}}}
      }
    };
}; // End of class header. 

#endif // REPORTCONVERTTOOL_H
