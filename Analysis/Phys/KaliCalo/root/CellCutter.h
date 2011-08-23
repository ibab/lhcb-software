#ifndef CELLCUTTER_H
#define CELLCUTTER_H

#include <vector>
#include <map>
#include <TObject.h>
#include <TFile.h>
#include <TTreeFormula.h>
#include <TChain.h>
#include <TString.h>
#include "TROOT.h"
#include <iostream>


class CellCutter : public TObject {
private:
  // Configuration variables
  TChain*                           m_chain;
  TTreeFormula*                     m_cutFormula;
  int                               m_maxOpenFiles;
  TString                           m_cut;
  // Chain variables
  float                             m_m12;
  float                             m_e1;
  float                             m_e2;
  float                             m_prs1;
  float                             m_prs2;
  int                               m_bkg;
  int                               m_cell1;
  int                               m_cell2;
  float                             m_dist;
  // Internal maps
  std::map<int, TString>            m_fileMap;
  TList*                            m_files;
  // Methods
  int cut( float, std::map<int, TString> );
  TChain* configureChain();
  int fillTree( TString );
public:
  // Constructors and configuration
  CellCutter() : m_chain(NULL), m_cutFormula(NULL), m_files(NULL) {};
  CellCutter( TString , int = 100 );
  // Destructor
  ~CellCutter(){ if (m_chain) delete m_chain; if (m_cutFormula) delete m_cutFormula; if (m_files) delete m_files; }
  // Methods
  int addFile( TString );
  int addCell( int, TString );
  int cut( float=1000 );
  //
  ClassDef( CellCutter , 1 );
};

#endif
