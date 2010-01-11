/*

  Due to the packaging policy within ROOT I had to hack up the code
  of TFormula and TTreeFormula into RootSelect

  The alternative would have been to link ALL Root libraries!

  Perhaps in a future release this mess could be cleared up a bit.

  M.Frank

  */




// @(#)root/treeplayer:$Name: not supported by cvs2svn $:$Id: RootSelect.h,v 1.1 2010-01-11 17:14:49 frankb Exp $
// Author: Rene Brun   19/01/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
// ---------------------------------- TreeFormula.h

#ifndef ROOT_RootSelect
#define ROOT_RootSelect



//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RootSelect                                                         //
//                                                                      //
// The Tree formula class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObjArray
#include "TObjArray.h"
#endif

//const Int_t kMAXFOUND = 200;
//const Int_t MAXCODES = kMAXFOUND; // must be the same as kMAXFOUND in TFormula
//const Int_t MAXFORMDIM = 5; // Maximum number of array dimensions support in RootSelect

class TTree;
class TLeaf;
class TMethodCall;
class TLeafObject;

class RootSelect   {

protected:
  enum { 
        MAXFOUND   = 200, 
        MAXCODES   = MAXFOUND, 
        MAXFORMDIM = 5, 
        METHOD     = 1000,  
        MAXOP      = 1000,
        MAXPAR     = 100,
        MAXCONST   = 100
   };
  Int_t     fNpar;            //Number of parameters
  Int_t     fNdim;            //Dimension of function (1=1-Dim, 2=2-Dim,etc)
  Int_t     fNoper;           //Number of operators
  Int_t     fNconst;          //Number of constants
  Int_t     fNumber;          //formula number identifier
  Int_t     fNval;            //Number of different variables in expression
  Int_t     fNstring;         //Number of different constants character strings

  Int_t     *fOper;           //[fNoper] List of operators
  TString   *fExpr;           //[fNoper] List of expressions
  Double_t  *fConst;          //[fNconst] Array of fNconst formula constants
  Double_t  *fParams;         //[fNpar] Array of fNpar parameters
  TString   *fNames;          //[fNpar] Array of parameter names



   TTree       *fTree;            //pointer to Tree
   Short_t     fCodes[MAXCODES]; //List of leaf numbers referenced in formula
   Int_t       fNcodes;           //Number of leaves referenced in formula
   Int_t       fMultiplicity;     //Number of array elements in leaves in case of a TClonesArray
   Int_t       fInstance;         //Instance number for GetValue
   Int_t       fNindex;           //Size of fIndex
   Int_t       *fIndex;           //array of instances numbers
   TObjArray   fMethods;          //List of leaf method calls
   
   Int_t       fNdimensions[MAXCODES];            //Number of array dimensions in each leaf
   Int_t       fCumulSize[MAXCODES][MAXFORMDIM]; //Accumulated size of lower dimensions for each leaf
   Int_t       fCumulUsedSize[MAXFORMDIM+1];      //Accumulated size of lower dimensions as seen for this formula
   Int_t       fIndexes[MAXCODES][MAXFORMDIM];   //Index of array selected by user for each leaf

   bool        m_isChar;
   void        DefineDimensions(const char *size, Int_t code, Int_t& virt_dim);
   TMethodCall       *GetMethodCall(Int_t code);
public:
             RootSelect(const char *formula, TTree *tree);
   virtual   ~RootSelect();
   virtual Int_t      DefinedVariable(TString &variable);
   virtual Double_t   EvalInstance(Int_t i=0);
   virtual Int_t      GetMultiplicity() {return fMultiplicity;}
   virtual TLeaf     *GetLeaf(Int_t n);
   virtual Int_t      GetNcodes() {return fNcodes;}
   virtual Int_t      GetNdata();
   virtual Double_t   GetValueLeafObject(Int_t i, TLeafObject *leaf);
   virtual Int_t      Compile(const char *expression);
   virtual void       SetTree(TTree* t)  { fTree = t; }
   virtual void       Analyze(int* already_found, const char *schain, Int_t &err);
   virtual void       SetNumber(Int_t number) {fNumber = number;}
   virtual void       SetParName(Int_t ipar, const char *name) {fNames[ipar] = name;}
   Int_t      GetNdim() {return fNdim;}
   Int_t      GetNpar() {return fNpar;}
   Int_t      GetNumber() {return fNumber;}
};

#endif

