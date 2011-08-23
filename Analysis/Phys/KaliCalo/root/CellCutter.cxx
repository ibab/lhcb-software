#include "CellCutter.h"

#if !defined( __CINT__ )
  ClassImp( CellCutter );
#endif

CellCutter::CellCutter( TString cut, int maxOpenFiles ) : m_chain(NULL), m_cutFormula(NULL), m_files(NULL) {
  m_cut = cut;
  m_maxOpenFiles = maxOpenFiles;
  m_files = new TList();
  m_chain = configureChain();
}

TChain* CellCutter::configureChain(){
  TChain* chain = new TChain("KaliPi0/Pi0-Tuple");
  // Configure the chain
  chain->SetBranchStatus("*",0); //disable all branches
  chain->SetBranchStatus("m12",1);
  chain->SetBranchStatus("g1E",1);
  chain->SetBranchStatus("g2E",1);
  chain->SetBranchStatus("bkg",1);
  chain->SetBranchStatus("ind1",1);
  chain->SetBranchStatus("ind2",1);
  chain->SetBranchStatus("prs1",1);
  chain->SetBranchStatus("prs2",1);
  chain->SetBranchStatus("dist",1);
  // Set branch address
  chain->SetBranchAddress("m12", &m_m12);
  chain->SetBranchAddress("bkg", &m_bkg);
  chain->SetBranchAddress("ind1", &m_cell1);
  chain->SetBranchAddress("ind2", &m_cell2);
  chain->SetBranchAddress("prs1", &m_prs1);
  chain->SetBranchAddress("prs2", &m_prs2);
  chain->SetBranchAddress("g1E", &m_e1);
  chain->SetBranchAddress("g2E", &m_e2);
  chain->SetBranchAddress("dist", &m_dist);
  return chain;
}

int CellCutter::addFile( TString filename ) {
  if ( !m_chain ) return 0;
  return m_chain->AddFile( filename );
}

int CellCutter::addCell( int cellID, TString filename ){
  // If the cellID didn't exist in the map, it returns 1, otherwise 0
  return int( m_fileMap.insert( std::pair<int, TString>( cellID, filename ) ).second );
}

int CellCutter::cut( float prsMax ){
  std::map<int, TString> tempMap ;
  int nElements = 1 ;
  for (std::map<int, TString>::const_iterator iter = m_fileMap.begin(); iter != m_fileMap.end(); iter++) {
    tempMap.insert( std::pair<int, TString>( iter->first, iter->second ) ) ; 
    if ( nElements == m_maxOpenFiles ){
      cut(prsMax, tempMap) ;
      nElements = 0 ;
      tempMap.clear() ;
    }
    nElements++ ;
  }
}


int CellCutter::cut( float prsMax, std::map<int, TString> fileMap ){
  // Checks
  if (!m_chain) return 1;
  if (!m_files) return 1;
  // Configure cut formula
  //if ( !m_cut.IsNull() ) m_cutFormula = new TTreeFormula("cut", m_cut, m_chain);
  // Let's rock
  for ( int i=0; m_chain->GetEntry(i); i++ ){
    if ( 0 == (i%1000000) ) std::cout << "Entry " << i << std::endl;
    if ( m_m12 > 250.0 ) continue; // cut on mass
    if ( max(m_prs1, m_prs2) > prsMax ) continue ; // Cut on PrS energy
    if ( m_cell1 != 0 && m_cell2 != 0 ) {
      if (fileMap.count(m_cell1)>0){
        fillTree( fileMap[m_cell1] );
      }
      if (fileMap.count(m_cell2)>0){
        fillTree( fileMap[m_cell2] );
      }
    } 
  }
  // Close open files
  TIter fileiter( gROOT->GetListOfFiles() );
  TFile *file;
  while ( (file = (TFile*) fileiter() ) ) {
     if( file->IsWritable() ) file->Write("",TObject::kOverwrite);
  }
  return 0;
}

int CellCutter::fillTree(TString filename)
{
  TFile *input = (TFile*) m_files->FindObject(filename);
  TTree *newtree;
  if ( !input ) {
    // Check if we have space.
    int alreadyOpened = m_files->GetEntries();
    if (alreadyOpened > m_maxOpenFiles) {
      // Close one of the files
      TFile *toclose = (TFile*) m_files->First();
      toclose->Write("",TObject::kOverwrite);
      m_files->RemoveFirst();
      delete toclose;
    }
    // Open
    input = TFile::Open(filename.Data(), "UPDATE");
    m_files->Add(input);
    newtree = (TTree*) input->Get("KaliPi0/Pi0-Tuple");
    if (!newtree) {
      input->mkdir("KaliPi0");
      input->cd("KaliPi0");
      newtree = m_chain->CloneTree(0);
    }
    else {
      // Reconnect the TTree.
      m_chain->AddClone( newtree );
      m_chain->CopyAddresses( newtree );
    }     
  } else {
    // Assumes we already connected the new tree.
    //input->GetObject("PiO-Tuple", newtree);
    newtree = (TTree*) input->Get("KaliPi0/Pi0-Tuple");
    m_chain->AddClone( newtree );
    m_chain->CopyAddresses( newtree );
  }
  return newtree->Fill();
}
