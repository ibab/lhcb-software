// $Id: TsaSpacePointCreator.cpp,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $


// Gaudo
#include "GaudiKernel/AlgFactory.h"

// boost
#include "boost/lexical_cast.hpp"

// Tsa
#include "TsaKernel/IITBoxProcessor.h"
#include "TsaKernel/TsaDataFunctor.h"
#include "TsaSpacePointCreator.h"

// factory defs
static const AlgFactory<TsaSpacePointCreator> s_factory;
const IAlgFactory& TsaSpacePointCreatorFactory = s_factory;

TsaSpacePointCreator::TsaSpacePointCreator(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  TsaBaseAlg(name, pSvcLocator),
  m_tmpCont(0)
{
  // constructor
  declareProperty("processor", m_itProcessorName = "ITBoxProcessor");
  declareProperty("nBox", m_nProcessor = 12);
}

TsaSpacePointCreator::~TsaSpacePointCreator()
{
  // destructor
  delete m_tmpCont;    
}

StatusCode TsaSpacePointCreator::initialize()
{
  // Initialization
  
  StatusCode sc = TsaBaseAlg::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }

  // make the it box processors.....
  for (int iB = 1; iB<= m_nProcessor; ++iB){
    std::string aName = "box"+boost::lexical_cast<std::string>(iB);
    IITBoxProcessor* aBox = tool<IITBoxProcessor>(m_itProcessorName,aName);
    m_boxs.push_back(aBox);
  }

  // tmp storage container for IT
  m_tmpCont = new Tsa::SpacePointVector();
  m_tmpCont->reserve(200);
  
  return StatusCode::SUCCESS;
}

StatusCode TsaSpacePointCreator::execute(){

  
  Tsa::SpacePoints* pCont = new Tsa::SpacePoints();
  pCont->reserve(1000);

  // loop over boxes and make spacepoints
  
  for (int iP = 0 ; iP < m_nProcessor ; ++iP){
    m_boxs[iP]->execute(m_tmpCont);

    // copy to output container    
    for (Tsa::SpacePointVector::iterator iterSP =  m_tmpCont->begin(); 
           iterSP != m_tmpCont->end() ; ++iterSP){
        pCont->insert(*iterSP);
    } // iterSP
      
    m_tmpCont->clear();

  } // iBox

  // sort sort sort, order station then by slope....
  std::sort(pCont->begin(),pCont->end(),
            TsaDataFunctor::Less_by_Tx<const Tsa::SpacePoint*>());
  std::stable_sort(pCont->begin(),pCont->end(),
            TsaDataFunctor::Less_by_Station<const Tsa::SpacePoint*>());

  put(pCont,Tsa::SpacePointLocation::Default);

  return StatusCode::SUCCESS;
}


StatusCode TsaSpacePointCreator::finalize(){
   
  return TsaBaseAlg::finalize();
}

