// $Id: RichMCTruthData.cpp,v 1.2 2009-06-05 17:21:32 jonrob Exp $
// Include files 

// local
#include "RichMCTruthData.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichMCTruthData
//
// 2007-06-26 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichMCTruthData )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichMCTruthData::RichMCTruthData( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichRingRecToolBase ( type, name , parent ),
    m_MCTrueRadius(VVD(0)),
    m_MCTrueNumHit(VVD(0)),
    m_MCTrueBeta(VVD(0)),
    m_MCTrueGamma(VVD(0)),
    m_MCTruePid(VVD(0)),
    m_numTrueTkInfo(VI(0))   
{
  declareInterface<IRichMCTruthData>(this);

}
void RichMCTruthData::ResetMCTruthArrays()
{

  
   int nrd =  RConst()-> maxNumRadiator();

   
   m_MCTrueRadius.clear();m_MCTrueRadius.reserve(nrd);
   m_MCTrueNumHit.clear();m_MCTrueNumHit.reserve(nrd);
   m_MCTrueBeta.clear();m_MCTrueBeta.reserve(nrd);
   m_MCTrueGamma.clear();m_MCTrueGamma.reserve(nrd);
   m_MCTruePid.clear();m_MCTruePid.reserve(nrd);
   //  m_MCTrueMomentum.clear();m_MCTrueMomentum.reserve(nrd);
   m_numTrueTkInfo.clear();m_numTrueTkInfo.reserve(nrd);


}
void RichMCTruthData::StoreBGPid(int ns,  VD aBeta, VD aGamma, VD aPid ){
 
  m_numTrueTkInfo.push_back(ns);
  m_MCTrueBeta.push_back(aBeta);
  m_MCTrueGamma.push_back( aGamma);
  m_MCTruePid.push_back(aPid); 
}
void RichMCTruthData::StoreTrueHit(VD rus, VD nus ) 
{
  m_MCTrueRadius.push_back(rus);
  m_MCTrueNumHit.push_back(nus);
  
}

//=============================================================================
// Destructor
//=============================================================================
RichMCTruthData::~RichMCTruthData() {} 

//=============================================================================
