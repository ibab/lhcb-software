/*

  Due to the packaging policy within ROOT I had to hack up the code
  of TFormula and TTreeFormula into RootSelect

  The alternative would have been to link ALL Root libraries!

  Perhaps in a future release this mess could be cleared up a bit.

  M.Frank

  */
#include "RVersion.h" 
#include "TMethodCall.h"
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,0,0)
  #include "TStreamerInfo.h" // HMK upgrade to Root 3.00
  typedef TMethodCall::EReturnType TMethodCall__EReturnType;
  #define TStreamerInfo__kLong     TStreamerInfo::kLong
  #define TStreamerInfo__kDouble   TStreamerInfo::kDouble
#else
  typedef EReturnType              TMethodCall__EReturnType;
  #define TStreamerInfo__kLong     kLong
  #define TStreamerInfo__kDouble   kDouble
#endif
// @(#)root/treeplayer:$Name: not supported by cvs2svn $:$Id: RootSelect.cpp,v 1.1 2010-01-11 17:14:49 frankb Exp $
// Author: Rene Brun   19/01/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#include "RootSelect.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeafC.h"
#include "TLeafObject.h"
#include "TMethodCall.h"
#include "TRandom.h"
#include "TMath.h"

#include <cstdio>
#include <cmath>
#include <iostream>

#ifdef WIN32
#  include <string.h>
#  define strncasecmp _strnicmp
#else
#  include <strings.h>
#endif

//______________________________________________________________________________
//
//     A TreeFormula is used to pass a selection expression
//     to the Tree drawing routine. See TTree::Draw
//
//  A TreeFormula can contain any arithmetic expression including
//  standard operators and mathematical functions separated by operators.
//  Examples of valid expression:
//          "x<y && sqrt(z)>3.2"
//
//______________________________________________________________________________
RootSelect::RootSelect(const char *expression, TTree *tree)   {
//*-*-*-*-*-*-*-*-*-*-*Normal Tree Formula constructor*-*-*-*-*-*-*-*-*-*-*
//*-*                  ===============================
//
   m_isChar      = false;
   fTree         = tree;
   fNindex       = MAXFOUND;
   fIndex        = new Int_t[fNindex];
   fExpr         = new TString[MAXOP];
   fOper         = new Int_t[MAXOP];
   fConst        = new Double_t[MAXCONST];
   fParams       = new Double_t[MAXPAR];
   fNames        = new TString[MAXPAR];
   fNcodes       = 0;
   fMultiplicity = 0;
   Int_t i,j,k;
      
   for (j=0; j<MAXCODES; j++) {
      fNdimensions[j] = 0;
      for (k = 0; k<MAXFORMDIM; k++) {
         fIndexes[j][k] = -1;
         fCumulSize[j][k] = 1;
      }
   }
   for (k = 0; k<MAXFORMDIM+1; k++) {
      fCumulUsedSize[k] = 1;
   }

   if (Compile(expression)) {fTree = 0; return; }
   if (fNcodes >= MAXFOUND) {
      //Warning("RootSelect","Too many items in expression:%s",expression);
      fNcodes = MAXFOUND;
   }
   for (i=0;i<fNcodes;i++) {
      if (fCodes[i] < 0) continue;
      TLeaf *leaf = GetLeaf(i);
      if (leaf->InheritsFrom("TLeafC") && !leaf->IsUnsigned()) m_isChar = true;
      if (leaf->InheritsFrom("TLeafB") && !leaf->IsUnsigned()) m_isChar = true;

      // Reminder of the meaning of fMultiplicity:
      //  -1: Only one or 0 element per entry, contains variable length array!
      //   0: Only one element per entry, no variable length array
      //   1: loop over the elements of a variable length array
      //   2: loop over elements of fixed length array (nData is the same for all entry)

      if (leaf->GetLeafCount()) {
         // We assume only one possible variable length dimension (the left most)
         fMultiplicity = 1;
      } else {
         if (leaf->GetLenStatic()>1 && fMultiplicity!=1) fMultiplicity = 2;
      }

      if (fIndex[i] == -1 ) fIndex[i] = 0;

      // Because we did not record the number of virtual dimensions associated 
      // with this leaf, we can not use the next loop which has to count down.
      Int_t virt_dim = 0;
      for (k = 0; k < fNdimensions[i]; k++) {
         if ( (fCumulSize[i][k]>=0) && (fIndexes[i][k] >= fCumulSize[i][k]) ) {
            // unreacheable element requested:
            fCumulUsedSize[virt_dim] = 0;
         }
         if ( fIndexes[i][k] < 0 ) virt_dim++;
      }
      // Add up the cumulative size
      for (k = fNdimensions[i]; (k > 0) && (fCumulSize[i][k-1]>=0); k--) {
         fCumulSize[i][k-1] *= fCumulSize[i][k];
      }
      
   }
   for (k = MAXFORMDIM; (k > 0) && (fCumulUsedSize[k-1]>=0); k--) {
      fCumulUsedSize[k-1] *= fCumulUsedSize[k];
   }
   // Now that we know the virtual dimension we know if a loop over EvalInstance
   // is needed or not.
   if (fCumulUsedSize[0]==1 && fMultiplicity!=0) fMultiplicity -= 2;
   
}

//______________________________________________________________________________
 RootSelect::~RootSelect()
{
//*-*-*-*-*-*-*-*-*-*-*Tree Formula default destructor*-*-*-*-*-*-*-*-*-*-*
//*-*                  =================================
   if (fIndex) delete [] fIndex;
   if (fExpr)   { delete [] fExpr;   fExpr   = 0;}
   if (fOper)   { delete [] fOper;   fOper   = 0;}
   if (fConst)  { delete [] fConst;  fConst  = 0;}
   if (fParams) { delete [] fParams; fParams = 0;}
   if (fNames)  { delete [] fNames;  fNames  = 0;}
}

 //______________________________________________________________________________
Int_t RootSelect::Compile(const char *expression)    {
//*-*-*-*-*-*-*-*-*-*-*Compile expression already stored in fTitle*-*-*-*-*-*
//*-*                  ===========================================
//*-*
//*-*   Loop on all subexpressions of formula stored in fTitle
//*-*
//
//
//*-*
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

  Int_t i,j,lc,valeur,err;
  TString ctemp;
  TString chaine = expression;
  // reset all local memory
  fNdim   = 0;
  fNpar   = 0;
  fNoper  = 0;
  fNconst = 0;
  fNumber = 0;
  fNstring= 0;
  fNval   = 0;
  for (i=0; i<MAXCONST; i++) {
      fConst[i] = 0;
  }
  for (i=0; i<MAXPAR; i++) {
      fParams[i] = 0;
      fNames[i] = "";
  }
  for (i=0; i<MAXOP; i++) {
      fExpr[i] = "";
      fOper[i] = 0;
  }
//*-*- Substitution of some operators to C++ style
//*-*  ===========================================
  for (i=1; i<=chaine.Length(); i++) {
    lc = chaine.Length();
    if ( ::strncasecmp(chaine(i-1,5).Data()," AND ", 5) == 0 ) {
      chaine = chaine(0,i-1) + " && " + chaine(i+4,lc-i-4);
      i=0;
    }
    else if ( ::strncasecmp(chaine(i-1,4).Data()," OR ",4) == 0 ) {
      chaine = chaine(0,i-1) + " && " + chaine(i+3,lc-i-3);
      i=0;
    }
    else if (chaine(i-1,2) == "**") {
      chaine = chaine(0,i-1) + "^" + chaine(i+1,lc-i-1);
      i=0;
    }
    else if (chaine(i-1,2) == "++") {
      chaine = chaine(0,i) + chaine(i+1,lc-i-1);
      i=0;
    }
    else if (chaine(i-1,2) == "+-" || chaine(i-1,2) == "-+") {
      chaine = chaine(0,i-1) + "-" + chaine(i+1,lc-i-1);
      i=0;
    } 
    else if (chaine(i-1,2) == "--") {
      chaine = chaine(0,i-1) + "+" + chaine(i+1,lc-i-1);
      i=0;
    } 
    else if (chaine(i-1,1) == "[") {
      for (j=1;j<=chaine.Length()-i;j++) {
        if (chaine(j+i-1,1) == "]" || j+i > chaine.Length()) 
          break;
      }
      ctemp = chaine(i,j-1);
      valeur=0;
      sscanf(ctemp.Data(),"%d",&valeur);
      if (valeur >= fNpar) fNpar = valeur+1;
    } 
    else if (chaine(i-1,1) == " ") {
      chaine = chaine(0,i-1)+chaine(i,lc-i);
      i=0;
    }
  }
  err = 0;
  Int_t already_found[MAXFOUND];
  for(i=0;i<MAXFOUND;i++) already_found[i]=0;
  Analyze(already_found, (const char*)chaine,err);

//*-*- if no errors, copy local parameters to formula objects
  if (!err) {
     if (fNdim <= 0) fNdim = 1;
     if (chaine.Length() > 4 && GetNumber() != 400) SetNumber(0);
     //*-*- if formula is a gaussian, set parameter names
     if (GetNumber() == 100) {
        SetParName(0,"Constant");
        SetParName(1,"Mean");
        SetParName(2,"Sigma");
     }
     //*-*- if formula is an exponential, set parameter names
     if (GetNumber() == 200) {
        SetParName(0,"Constant");
        SetParName(1,"Slope");
     }
     //*-*- if formula is a polynome, set parameter names
     if (GetNumber() == 300+fNpar) {
        for (i=0;i<fNpar;i++) SetParName(i,Form("p%d",i));
     }
     //*-*- if formula is a landau, set parameter names
     if (GetNumber() == 400) {
        SetParName(0,"Constant");
        SetParName(1,"Mean");
        SetParName(2,"Sigma");
     }
  }


//*-* replace 'normal' == or != by ==(string) or !=(string) if needed.
  Int_t is_it_string,last_string=0;
  if (!fOper) fNoper = 0;
  for (i=0; i<fNoper; i++) {
     is_it_string = 0;
     if ((fOper[i]>=105000 && fOper[i]<110000) || fOper[i] == 80000) is_it_string = 1;
     else if (fOper[i] == 62 && last_string == 1) fOper[i] = 76;
     else if (fOper[i] == 63 && last_string == 1) fOper[i] = 77;
     last_string = is_it_string;
  }

  if (err) { fNdim = 0; return 1; }
  return 0;
}

//______________________________________________________________________________
 void RootSelect::DefineDimensions(const char *info, Int_t code, Int_t& virt_dim) {
   // We assume that there are NO white spaces in the info string
   const char * current;
   Int_t size, scanindex;
   Bool_t neg;

   current = info;
   // the next value could be before the string but
   // that's okay because the next operation is ++
   // (this is to avoid (?) a if statement at the end of the
   // loop 
   if (current[0] != '[') current--; 
   while (current) {
      current++;
      scanindex = sscanf(current,"%d",&size);
      // if scanindex is 0 then we have a name index thus a variable
      // array (or TClonesArray!).
      if (scanindex==0) {
         fCumulSize[code][fNdimensions[code]] = -1;
         if ( fIndexes[code][fNdimensions[code]] < 0 ) {
            fCumulUsedSize[virt_dim] = -1 * TMath::Abs(fCumulUsedSize[virt_dim]);
            virt_dim++;
         } 
      } else {
         fCumulSize[code][fNdimensions[code]] = size;
         if ( fIndexes[code][fNdimensions[code]] < 0 ) {
            if ( TMath::Abs(fCumulUsedSize[virt_dim])==1 
                 || (size < TMath::Abs(fCumulUsedSize[virt_dim]) ) ) {
               neg = fCumulUsedSize[virt_dim] < 0;
               fCumulUsedSize[virt_dim] = size;
               if (neg) fCumulUsedSize[virt_dim] *= -1;
            }
            virt_dim++;
         }
      }
      fNdimensions[code] ++;
      if (fNdimensions[code] >= MAXFORMDIM) {
         // NOTE: test that fNdimensions[code] this is NOT too big!!          
         break; 
      }
      current = (char*)strstr( current, "[" );
   }

}

//______________________________________________________________________________
 Int_t RootSelect::DefinedVariable(TString &name)
{
//*-*-*-*-*-*Check if name is in the list of Tree/Branch leaves*-*-*-*-*
//*-*        ==================================================
//
//   This member function redefines the function in TFormula
//   If a leaf has a name corresponding to the argument name, then
//   returns a new code.
//   A RootSelect may contain more than one variable.
//   For each variable referenced, the pointers to the corresponding
//   branch and leaf is stored in the object arrays fBranches and fLeaves.
//
//   name can be :
//      - Leaf_Name (simple variable or data member of a ClonesArray)
//      - Branch_Name.Leaf_Name
//      - Branch_Name.Method_Name
//      - Leaf_Name[index]
//      - Branch_Name.Leaf_Name[index]
// I want to support, with Leaf_Name a 1D array data member.
//      - Branch_Name.Leaf_Name[index1]
//      - Branch_Name.Leaf_Name[][index2]
//      - Branch_Name.Leaf_Name[index1][index2]
//

   if (!fTree) return -1;
   fNpar = 0;
   Int_t nchname = name.Length();
   if (nchname > 60) return -1;
   char branchname[128];
   char leafname[128];
   static char anumber[10];
   static char lname[128];
   static char lmethod[128];
   Int_t i;
   strcpy(lname,name.Data());
   Int_t dot   = 0;
   Int_t index = -1;
   Int_t scanindex = 0;
   char *ref = 0;
   for (i=0;i<nchname;i++) {
      if (lname[i] == '.') {
         if (i == nchname-1) break; // for example 999.
         if (!dot) {
            dot = i;
            strcpy(lmethod,&lname[i+1]);
         }
      }
      if (name[i] == '[' && name[nchname-1] == ']') {
         char * current = &( name[i] );
         Int_t dim = 0;
         while (current) {
            current++;
            if (current[0] == ']') {
               fIndexes[fNcodes][dim] = -1; // Loop over all elements;
            } else {
               scanindex = sscanf(current,"%d",&index);
               if (scanindex) {
                  fIndexes[fNcodes][dim] = index;
               } else {
                  if (ref == 0) ref= anumber;
                  index = 0;
               }
            }
            dim ++;
            if (dim >= MAXFORMDIM) {
               // NOTE: test that dim this is NOT too big!!
               break; 
            }
            current = (char*)strstr( current, "[" );
         }
         lname[i] = 0;
         break;
      }
   }
   TObjArray *lleaves = fTree->GetListOfLeaves();
   Int_t nleaves = lleaves->GetEntriesFast();

//*-*- There is a dot. two cases: data member or member function
   if (dot) {
//          Look for a data member
      for (i=0;i<nleaves;i++) {
         TLeaf *leaf = (TLeaf*)lleaves->UncheckedAt(i);
         strcpy(branchname,leaf->GetBranch()->GetName());
         // do not look at the indexes if any
         char *branch_dim = (char*)strstr(branchname,"[");
         if (branch_dim) { branch_dim[0] = '0'; branch_dim++; }
         if (!strcmp(lname, branchname) ) {
            TMethodCall *method = 0;
            fMethods.Add(method);
            Int_t code = fNcodes;
            fCodes[code] = i;
            fNcodes++;
            if (scanindex) fIndex[code] = index;
            else           fIndex[code] = -1;

            // Let see if we can understand the structure of this branch.
            // Usually we have: leafname[fixed_array] leaftitle[var_array]type
            // (with fixed_array that can be a multi-dimension array.
            // NOTE I removed this? if ( !strcmp(lname,leaf->GetName() ) ) {
            const char *tname = leaf->GetTitle();
            char *dim = (char*)strstr( tname, "[" );
            Int_t virt_dim = 0;
            if (dim) {
               dim++;
               if (!branch_dim || strncmp(branch_dim,dim,strlen(branch_dim))) {
                  // then both are NOT the same so do the title first:
                  DefineDimensions( dim, code, virt_dim);
               }
            }
            if (branch_dim) {
               // then both are NOT same so do the title:
               DefineDimensions( branch_dim, code, virt_dim);
            }
            // should we also check the leaf name?

            if (leaf->InheritsFrom("TLeafC") && !leaf->IsUnsigned()) return 5000+code;
            return code;
         }
      }
//          Look for branchname.leafname
      for (i=0;i<nleaves;i++) {
         TLeaf *leaf = (TLeaf*)lleaves->UncheckedAt(i);
         sprintf(branchname,"%s.%s",leaf->GetBranch()->GetName(),leaf->GetName());
         // do not look at the indexes if any
         char *branch_dim = (char*)strstr(branchname,"[");
         if (branch_dim) { branch_dim[0] = '0'; branch_dim++; }
         if (!strcmp(lname,branchname)) {
            TMethodCall *method = 0;
            fMethods.Add(method);
            Int_t code = fNcodes;
            fCodes[code] = i;
            fNcodes++;
            if (scanindex) fIndex[code] = index;
            else           fIndex[code] = -1;

            // Let see if we can understand the structure of this branch.
            // Usually we have: leafname[fixed_array] leaftitle[var_array]type
            // (with fixed_array that can be a multi-dimension array.
            // NOTE I removed this? if ( !strcmp(lname,leaf->GetName() ) ) {
            const char *tname = leaf->GetTitle();
            char *dim = (char*)strstr( tname, "[" );
            Int_t virt_dim = 0;
            if (dim) {
               dim++;
               if (!branch_dim || strncmp(branch_dim,dim,strlen(branch_dim))) {
                  // then both are NOT the same so do the title first:
                  DefineDimensions( dim, code, virt_dim);
               }
            }
            if (branch_dim) {
               // then both are NOT same so do the title:
               DefineDimensions( branch_dim, code, virt_dim);
            }
            // should we also check the leaf name?

            if (leaf->InheritsFrom("TLeafC")) return 5000+code;
            return code;
         }
      }
//          Look for a member function
      for (i=0;i<nleaves;i++) {
         lname[dot] = 0;
         TLeaf *leaf = (TLeaf*)lleaves->UncheckedAt(i);
         strcpy(branchname,leaf->GetBranch()->GetName());
         // do not look at the indexes if any
         char *branch_dim = (char*)strstr(branchname,"[");
         if (branch_dim) { branch_dim[0] = '0'; branch_dim++; }
         if (!strcmp(lname,branchname)) {
            if (leaf->IsA() != TLeafObject::Class()) return -1;
            TLeafObject *lobj = (TLeafObject*)leaf;
            TMethodCall *method = lobj->GetMethodCall(lmethod);
            if (!method) return -1;
            fMethods.Add(method);
            Int_t code = fNcodes;
            fCodes[code] = i;
            fNcodes++;
            fIndex[code] = code-METHOD;

            // Let see if we can understand the structure of this branch.
            // Usually we have: leafname[fixed_array] leaftitle[var_array]type
            // (with fixed_array that can be a multi-dimension array.
            // NOTE I removed this? if ( !strcmp(lname,leaf->GetName() ) ) {
            const char *tname = leaf->GetTitle();
            char *dim = (char*)strstr( tname, "[" );
            Int_t virt_dim = 0;
            if (dim) {
               dim++;
               if (!branch_dim || strncmp(branch_dim,dim,strlen(branch_dim))) {
                  // then both are NOT the same so do the title first:
                  DefineDimensions( dim, code, virt_dim);
               }
            }
            if (branch_dim) {
               // then both are NOT same so do the title:
               DefineDimensions( branch_dim, code, virt_dim);
            }
            // should we also check the leaf name?

            return code;
         }
      }

//*-*- There is no dot. Look for a data member only
   } else {
      for (i=0;i<nleaves;i++) {
         TLeaf *leaf = (TLeaf*)lleaves->UncheckedAt(i);
         strcpy(branchname,leaf->GetBranch()->GetName());
         strcpy(leafname,leaf->GetName());
         // do not look at the indexes if any
         char *branch_dim = (char*)strstr(branchname,"[");
         if (branch_dim) { branch_dim[0] = '0'; branch_dim++; }
         char *leaf_dim = (char*)strstr(leafname,"[");
         if (leaf_dim) { leaf_dim[0] = '0'; leaf_dim++; }
         if (!strcmp(lname,leaf->GetBranch()->GetName()) ||
             !strcmp(lname,leaf->GetName())) {
            TMethodCall *method = 0;
            fMethods.Add(method);
            Int_t code = fNcodes;
            fCodes[code] = i;
            fNcodes++;
            if (scanindex) fIndex[code] = index;
            else           fIndex[code] = -1;
            
            // Let see if we can understand the structure of this branch.
            // Usually we have: leafname[fixed_array] leaftitle[var_array]type
            // (with fixed_array that can be a multi-dimension array.
            // NOTE I removed this? if ( !strcmp(lname,leaf->GetName() ) ) {
            const char *tname = leaf->GetTitle();
            char *dim = (char*)strstr( tname, "[" );
            Int_t virt_dim = 0;
            if (dim) {
               dim++;
               if (!branch_dim || strncmp(branch_dim,dim,strlen(branch_dim))) {
                  // then both are NOT the same so do the title first:
                  DefineDimensions( dim, code, virt_dim);
               }
            }
            if (branch_dim) {
               // then both are NOT same so do the title:
               DefineDimensions( branch_dim, code, virt_dim);
            }
            // should we also check the leaf name?

            if (ref) printf("Cannot process reference to array index=%s[%s]n",lname,ref);
            if (leaf->InheritsFrom("TLeafC") && !leaf->IsUnsigned()) return 5000+code;
            if (leaf->InheritsFrom("TLeafB") && !leaf->IsUnsigned()) return 5000+code;
            return code;
         }
      }
   }
   return -1;
}

//______________________________________________________________________________
Double_t RootSelect::EvalInstance(Int_t instance)
{
//*-*-*-*-*-*-*-*-*-*-*Evaluate this treeformula*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  =========================
//

  const Int_t kMAXSTRINGFOUND = 10;
  Int_t i,pos,pos2,int1,int2,real_instance,virt_dim;
  Float_t aresult;
  Double_t tab[MAXFOUND];
  Double_t param[MAXFOUND];
  Double_t dexp;
  char *tab2[kMAXSTRINGFOUND];

  if (fNoper == 1 && fNcodes > 0 && fCodes[0] >= 0 ) {
     TLeaf *leaf = GetLeaf(0);

     // Now let calculate what physical instance we really need.
     // Some redundant code is used to speed up the cases where
     // they are no dimensions.
     real_instance = 0;
     Int_t max_dim = fNdimensions[0];
     if ( max_dim ) {
        virt_dim = 0;
        max_dim--;
        for (Int_t dim = 0; dim < max_dim; dim++) {
           if (fIndexes[0][dim]>=0) { 
              real_instance += fIndexes[0][dim] * fCumulSize[0][dim+1];
           } else {
              if (fCumulUsedSize[virt_dim]>1) {
                 real_instance += ( ( instance % fCumulUsedSize[virt_dim] ) 
                                    / fCumulUsedSize[virt_dim+1]) 
                                  * fCumulSize[0][dim+1];
              } else {
                 real_instance += ( instance / fCumulUsedSize[virt_dim+1]) 
                                  * fCumulSize[0][dim+1];
              }              
              virt_dim ++;
           }
        }
        if (fIndexes[0][max_dim]>=0) { 
           real_instance += fIndexes[0][max_dim];
        } else {
           if (fCumulUsedSize[virt_dim]>1) {
              real_instance += instance % fCumulUsedSize[virt_dim];
           } else {
              real_instance += instance;
           }
        }
     }  
     
     if (instance) {
        if (real_instance < leaf->GetNdata()) return leaf->GetValue(real_instance);
        else                                  return leaf->GetValue(0);
     } else {
        leaf->GetBranch()->GetEntry(fTree->GetReadEntry());
        if (!(leaf->IsA() == TLeafObject::Class())) return leaf->GetValue(real_instance);
        return GetValueLeafObject(fIndex[0],(TLeafObject *)leaf);
     }
  }
  for(i=0;i<fNval;i++) {
     if (fCodes[i] >= 0) {
        TLeaf *leaf = GetLeaf(i);

        // Now let calculate what physical instance we really need.
        // Some redundant code is used to speed up the cases where
        // they are no dimensions.
        real_instance = 0;
        Int_t max_dim = fNdimensions[i];
        if ( max_dim ) {
           virt_dim = 0;
           max_dim--;
           for (Int_t dim = 0; dim < max_dim; dim++) {
              if (fIndexes[i][dim]>=0) { 
                 real_instance += fIndexes[i][dim] * fCumulSize[i][dim+1];
              } else {
                 if (fCumulUsedSize[virt_dim]>1) {
                    real_instance += ( ( instance % fCumulUsedSize[virt_dim] ) 
                                       / fCumulUsedSize[virt_dim+1]) 
                                     * fCumulSize[i][dim+1];
                 } else {
                    real_instance += ( instance / fCumulUsedSize[virt_dim+1]) 
                                     * fCumulSize[i][dim+1];
                 }   
                 virt_dim ++;
              }
           }
           if (fIndexes[i][max_dim]>=0) { 
              real_instance += fIndexes[i][max_dim];
           } else {
              if (fCumulUsedSize[virt_dim]>1) {
                 real_instance += instance % fCumulUsedSize[virt_dim];
              } else {
                 real_instance += instance;
              }
           }
        }
        if (instance) {
           if (real_instance < leaf->GetNdata()) param[i] = leaf->GetValue(real_instance);
           else                                  param[i] = leaf->GetValue(0);
        } else {
           leaf->GetBranch()->GetEntry(fTree->GetReadEntry());
           if (!(leaf->IsA() == TLeafObject::Class())) param[i] = leaf->GetValue(real_instance);
           else param[i] = GetValueLeafObject(fIndex[i],(TLeafObject *)leaf);
        }
     }
  }
  pos  = 0;
  pos2 = 0;
  for (i=0; i<fNoper; i++) {
    Int_t action = fOper[i];
//*-*- a tree string
    if (action >= 105000) {
       TLeaf *leafc = GetLeaf(action-105000);
       leafc->GetBranch()->GetEntry(fTree->GetReadEntry());
       pos2++; tab2[pos2-1] = (char*)leafc->GetValuePointer();
       continue;
    }
//*-*- a tree variable
    if (action >= 100000) {
       pos++; tab[pos-1] = param[action-100000];
       continue;
    }
//*-*- String
    if (action == 80000) {
       pos2++; tab2[pos2-1] = (char*)fExpr[i].Data();
       continue;
    }
//*-*- numerical value
    if (action >= 50000) {
       pos++; tab[pos-1] = fConst[action-50000];
       continue;
    }
    if (action == 0) {
      pos++;
      sscanf((const char*)fExpr[i],"%g",&aresult);
      tab[pos-1] = aresult;
//*-*- basic operators and mathematical library
    } else if (action < 100) {
        switch(action) {
          case   1 : pos--; tab[pos-1] += tab[pos]; break;
          case   2 : pos--; tab[pos-1] -= tab[pos]; break;
          case   3 : pos--; tab[pos-1] *= tab[pos]; break;
          case   4 : pos--; if (tab[pos] == 0) tab[pos-1] = 0; //  division by 0
                            else               tab[pos-1] /= tab[pos];
                     break;
          case   5 : {pos--; int1=Int_t(tab[pos-1]); int2=Int_t(tab[pos]); tab[pos-1] = Double_t(int1%int2); break;}
          case  10 : tab[pos-1] = TMath::Cos(tab[pos-1]); break;
          case  11 : tab[pos-1] = TMath::Sin(tab[pos-1]); break;
          case  12 : if (TMath::Cos(tab[pos-1]) == 0) {tab[pos-1] = 0;} // { tangente indeterminee }
                     else tab[pos-1] = TMath::Tan(tab[pos-1]);
                     break;
          case  13 : if (TMath::Abs(tab[pos-1]) > 1) {tab[pos-1] = 0;} //  indetermination
                     else tab[pos-1] = TMath::ACos(tab[pos-1]);
                     break;
          case  14 : if (TMath::Abs(tab[pos-1]) > 1) {tab[pos-1] = 0;} //  indetermination
                     else tab[pos-1] = TMath::ASin(tab[pos-1]);
                     break;
          case  15 : tab[pos-1] = TMath::ATan(tab[pos-1]); break;
          case  70 : tab[pos-1] = TMath::CosH(tab[pos-1]); break;
          case  71 : tab[pos-1] = TMath::SinH(tab[pos-1]); break;
          case  72 : if (TMath::CosH(tab[pos-1]) == 0) {tab[pos-1] = 0;} // { tangente indeterminee }
                     else tab[pos-1] = TMath::TanH(tab[pos-1]);
                     break;
          case  73 : if (tab[pos-1] < 1) {tab[pos-1] = 0;} //  indetermination
                     else tab[pos-1] = TMath::ACosH(tab[pos-1]);
                     break;
          case  74 : tab[pos-1] = TMath::ASinH(tab[pos-1]); break;
          case  75 : if (TMath::Abs(tab[pos-1]) > 1) {tab[pos-1] = 0;} // indetermination
                     else tab[pos-1] = TMath::ATanH(tab[pos-1]); break;
          case  16 : pos--; tab[pos-1] = TMath::ATan2(tab[pos-1],tab[pos]); break;
          case  17 : pos--; tab[pos-1] = fmod(tab[pos-1],tab[pos]); break;
          case  20 : pos--; tab[pos-1] = TMath::Power(tab[pos-1],tab[pos]); break;
          case  21 : tab[pos-1] = tab[pos-1]*tab[pos-1]; break;
          case  22 : tab[pos-1] = TMath::Sqrt(TMath::Abs(tab[pos-1])); break;
          case  23 : pos2 -= 2; pos++;if (strstr(tab2[pos2],tab2[pos2+1])) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  30 : if (tab[pos-1] > 0) tab[pos-1] = TMath::Log(tab[pos-1]);
                     else {tab[pos-1] = 0;} //{indetermination }
                     break;
          case  31 : dexp = tab[pos-1];
                     if (dexp < -70) {tab[pos-1] = 0; break;}
                     if (dexp >  70) {tab[pos-1] = TMath::Exp(70); break;}
                     tab[pos-1] = TMath::Exp(dexp); break;
          case  32 : if (tab[pos-1] > 0) tab[pos-1] = TMath::Log10(tab[pos-1]);
                     else {tab[pos-1] = 0;} //{indetermination }
                     break;
          case  40 : pos++; tab[pos-1] = TMath::ACos(-1); break;
          case  41 : tab[pos-1] = TMath::Abs(tab[pos-1]); break;
          case  42 : if (tab[pos-1] < 0) tab[pos-1] = -1; else tab[pos-1] = 1; break;
          case  43 : tab[pos-1] = Double_t(Int_t(tab[pos-1])); break;
          case  50 : pos++; tab[pos-1] = gRandom->Rndm(1); break;
          case  60 : pos--; if (tab[pos-1]!=0 && tab[pos]!=0) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  61 : pos--; if (tab[pos-1]!=0 || tab[pos]!=0) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  62 : pos--; if (tab[pos-1] == tab[pos]) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  63 : pos--; if (tab[pos-1] != tab[pos]) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  64 : pos--; if (tab[pos-1] < tab[pos]) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  65 : pos--; if (tab[pos-1] > tab[pos]) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  66 : pos--; if (tab[pos-1]<=tab[pos]) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  67 : pos--; if (tab[pos-1]>=tab[pos]) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  68 : if (tab[pos-1]!=0) tab[pos-1] = 0; else tab[pos-1] = 1; break;
          case  76 : pos2 -= 2; pos++; if (!strcmp(tab2[pos2+1],tab2[pos2])) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  77 : pos2 -= 2; pos++;if (strcmp(tab2[pos2+1],tab2[pos2])) tab[pos-1]=1;
                            else tab[pos-1]=0; break;
          case  78 : pos--; tab[pos-1]= ((Int_t) tab[pos-1]) & ((Int_t) tab[pos]); break;
          case  79 : pos--; tab[pos-1]= ((Int_t) tab[pos-1]) | ((Int_t) tab[pos]); break;
       }
    }
  }
  Double_t result = tab[0];
  return result;
}

//______________________________________________________________________________
 TLeaf *RootSelect::GetLeaf(Int_t n)
{
//*-*-*-*-*-*-*-*Return leaf corresponding to serial number n*-*-*-*-*-*
//*-*            ============================================
//

   return (TLeaf*)fTree->GetListOfLeaves()->UncheckedAt(fCodes[n]);
}

//______________________________________________________________________________
 TMethodCall *RootSelect::GetMethodCall(Int_t code)
{
//*-*-*-*-*-*-*-*Return methodcall corresponding to code*-*-*-*-*-*
//*-*            =======================================
//
//  function called by TLeafObject::GetValue
//  with the value of fIndex computed in RootSelect::DefinedVariable

   return (TMethodCall *)fMethods.UncheckedAt(code+METHOD);

}

//______________________________________________________________________________
 Int_t RootSelect::GetNdata()
{
//*-*-*-*-*-*-*-*Return number of data words in the leaf*-*-*-*-*-*-*-*
//*-*-*-*-*-*-*-*Changed to Return number of available instances in the formula*-*-*-*-*-*-*-*
//*-*            =======================================
//

   // new version of GetNData:
   // Possible problem: we only allow one variable dimension so far.
   if (fMultiplicity==0) return 1;

   if (fMultiplicity==2) return fCumulUsedSize[0];
   
   // We have at least one leaf with a variable size:
   Int_t  overall, current;

   overall = 1;
   for (Int_t i=0;i<fNcodes;i++) {
      if (fCodes[i] < 0) continue;
      TLeaf *leaf = GetLeaf(i);
      if (leaf->GetLeafCount()) {
         TBranch *branch = leaf->GetLeafCount()->GetBranch();
         branch->GetEntry(fTree->GetReadEntry());
         current = leaf->GetLen() / leaf->GetLenStatic();
         if (fIndexes[i][0] < 0 ) {
            if (overall==1 || (current!=1 && current<overall) ) overall = current;
         } else if (fIndexes[i][0] >= current) {
            // unreacheable element requested:
            overall = 0;
         }
      } 
   }
   if (overall==0) return 0;
   if (fCumulUsedSize[0] >= 0 ) return fCumulUsedSize[0];
   else {
      Int_t tmp = -1*fCumulUsedSize[0];
      if (tmp!=1 && (tmp<overall) ) return tmp*fCumulUsedSize[1];
      return overall*fCumulUsedSize[1];
   }

}

//______________________________________________________________________________
 Double_t RootSelect::GetValueLeafObject(Int_t i, TLeafObject *leaf)
{
//*-*-*-*-*-*-*-*Return result of a leafobject method*-*-*-*-*-*-*-*
//*-*            ====================================
//

   if (i>=0) return 0; // case where we do NOT have a method defined   
   TMethodCall *m = GetMethodCall(i);
   if (!m)   return 0;

   void *thisobj = leaf->GetObject();

   // HMK add TMethodCall qualifier to upgrade to Root 3
   TMethodCall__EReturnType r = m->ReturnType();

   if (int(r) == int(TStreamerInfo__kLong)) {
      Long_t l;
      m->Execute(thisobj, l);
      return (Double_t) l;
   }
   if (int(r) == int(TStreamerInfo__kDouble)) {
      Double_t d;
      m->Execute(thisobj, d);
      return (Double_t) d;
   }
   m->Execute(thisobj);
   return 0;
}

//______________________________________________________________________________
void RootSelect::Analyze(int* already_found, const char *schain, Int_t &err)      {
//*-*-*-*-*-*-*-*-*Analyze a sub-expression in one formula*-*-*-*-*-*-*-*-*-*-*
//*-*              =======================================
//*-*
//*-*   Expressions in one formula are recursively analyzed.
//*-*   Result of analysis is stored in the object tables.
//*-*
//*-*                  Table of function codes and errors
//*-*                  ==================================
//*-*
//*-*   * functions :
//*-*
//*-*     +           1                   pow          20
//*-*     -           2                   sq           21
//*-*     *           3                   sqrt         22
//*-*     /           4                   strstr       23
//*-*     %           5
//*-*                                     log          30
//*-*     cos         10                  exp          31
//*-*     sin         11                  log10        32
//*-*     tan         12
//*-*     acos        13                  abs          41
//*-*     asin        14                  sign         42
//*-*     atan        15                  int          43
//*-*     atan2       16
//*-*     fmod        17                  rndm         50
//*-*
//*-*     cosh        70                  acosh        73
//*-*     sinh        71                  asinh        74
//*-*     tanh        72                  atanh        75
//*-*
//*-*     expo      10xx                  gaus       20xx
//*-*     expo(0)   1000                  gaus(0)    2000
//*-*     expo(1)   1001                  gaus(1)    2001
//*-*     xexpo     10xx                  xgaus      20xx
//*-*     yexpo     11xx                  ygaus      21xx
//*-*     zexpo     12xx                  zgaus      22xx
//*-*     xyexpo    15xx                  xygaus     25xx
//*-*     yexpo(5)  1105                  ygaus(5)   2105
//*-*     xyexpo(2) 1502                  xygaus(2)  2502
//*-*
//*-*     landau      40xx
//*-*     landau(0)   4000
//*-*     landau(1)   4001
//*-*     xlandau     40xx
//*-*     ylandau     41xx
//*-*     zlandau     42xx
//*-*     xylandau    45xx
//*-*     ylandau(5)  4105
//*-*     xylandau(2) 4502
//*-*
//*-*     pol0      100xx                 pol1      101xx
//*-*     pol0(0)   10000                 pol1(0)   10100
//*-*     pol0(1)   10001                 pol1(1)   10101
//*-*     xpol0     100xx                 xpol1     101xx
//*-*     ypol0     200xx                 ypol1     201xx
//*-*     zpol0     300xx                 zpol1     301xx
//*-*     ypol0(5)  20005                 ypol1(5)  20105
//*-*
//*-*     pi          40
//*-*
//*-*     &&          60                  <            64
//*-*     ||          61                  >            65
//*-*     ==          62                  <=           66
//*-*     !=          63                  =>           67
//*-*     !           68
//*-*     ==(string)  76                  &            78
//*-*     !=(string)  77                  |            79
//*-*
//*-*   * constants :
//*-*
//*-*    c0  50000      c1  50001  etc..
//*-*
//*-*   * strings :
//*-*
//*-*    s0  80000      s1  80001  etc..
//*-*
//*-*   * variables :
//*-*
//*-*     x    100000     y    100001     z    100002     t    100003
//*-*
//*-*   * parameters :
//*-*
//*-*     [1]        101
//*-*     [2]        102
//*-*     etc.
//*-*
//*-*   * errors :
//*-*
//*-*     1  : Division By Zero
//*-*     2  : Invalid Floating Point Operation
//*-*     4  : Empty String
//*-*     5  : invalid syntax
//*-*     6  : Too many operators
//*-*     7  : Too many parameters
//*-*    10  : z specified but not x and y
//*-*    11  : z and y specified but not x
//*-*    12  : y specified but not x
//*-*    13  : z and x specified but not y
//*-*    20  : non integer value for parameter number
//*-*    21  : atan2 requires two arguments
//*-*    22  : pow requires two arguments
//*-*    23  : degree of polynomial not specified
//*-*    24  : Degree of polynomial must be positive
//*-*    25  : Degree of polynomial must be less than 20
//*-*    26  : Unknown name
//*-*    27  : Too many constants in expression
//*-*    28  : strstr requires two arguments
//*-*    40  : '(' is expected
//*-*    41  : ')' is expected
//*-*    42  : '[' is expected
//*-*    43  : ']' is expected
//
/*

*/
//
//*-*
//*-*
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*


   Int_t valeur,find,n,i,j,k,lchain,nomb,virgule,inter;
   Int_t compt,compt2,compt3,compt4,hexa;
   Float_t vafConst;
   UInt_t vafConst2;
   Bool_t parenthese;
   TString s,chaine_error,chaine1ST;
   TString s1,s2,s3,ctemp;
   TString chaine = schain;
   //TFormula *oldformula;
   Int_t modulo,plus,puiss10,puiss10bis,moins,multi,divi,puiss,et,ou,petit,grand,egal,diff,peteg,grdeg,etx,oux;
   char t;

  Int_t inter2 = 0;
  SetNumber(0);

//*-*- Verify correct matching of parenthesis and remove unnecessary parenthesis.
//*-*  ========================================================================
  lchain = chaine.Length();
  //if (chaine(lchain-2,2) == "^2") chaine = "sq(" + chaine(0,lchain-2) + ")";
  parenthese = kTRUE;
  lchain = chaine.Length();
  while (parenthese && lchain>0 && err==0){
    compt  = 0;
    compt2 = 0;
    lchain = chaine.Length();
    if (lchain==0) err=4;
    else {
      for (i=1; i<=lchain; ++i) {
        if (chaine(i-1,1) == "[") compt2++;
        if (chaine(i-1,1) == "]") compt2--;
        if (chaine(i-1,1) == "(") compt++;
        if (chaine(i-1,1) == ")") compt--;
        if (compt < 0) err = 40; // more open parenthesis than close paraenthesis
        if (compt2< 0) err = 42; // more ] than [
        if (compt==0 && (i!=lchain || lchain==1)) parenthese = kFALSE;
        // if (lchain<3 && chaine(0,1)!="(" && chaine(lchain-1,1)!=")") parenthese = kFALSE;
      }
      if (compt > 0) err = 41; // more ( than )
      if (compt2> 0) err = 43; // more [ than ]
      if (parenthese) chaine = chaine(1,lchain-2);
    }
  }
  if (lchain==0) err=4; // empty string
  modulo=plus=moins=multi=divi=puiss=et=ou=petit=grand=egal=diff=peteg=grdeg=etx=oux=0;

//*-*- Look for simple operators
//*-*  =========================

if (err==0) {
  compt = compt2 = compt3 = compt4 = 0;puiss10=0;puiss10bis = 0;
  j = lchain;
  for (i=1;i<=lchain; i++) {
    puiss10=puiss10bis=0;
    if (i>2) {
       t = chaine[i-3];
       if (strchr("0123456789",t) && chaine[i-2] == 'e' ) puiss10 = 1;
       else if (i>3) {
          t = chaine[i-4];
          if (strchr("0123456789",t) && chaine(i-3,2) == ".e" ) puiss10 = 1;
          }
    }
    if (j>2) {
       t = chaine[j-3];
       if (strchr("0123456789",t) && chaine[j-2] == 'e' ) puiss10bis = 1;
       else if (j>3) {
          t = chaine[j-4];
          if (strchr("0123456789",t) && chaine(j-3,2) == ".e" ) puiss10bis = 1;
          }
    }
    if (chaine(i-1,1) == "[") compt2++;
    if (chaine(i-1,1) == "]") compt2--;
    if (chaine(i-1,1) == "(") compt++;
    if (chaine(i-1,1) == ")") compt--;
    if (chaine(j-1,1) == "[") compt3++;
    if (chaine(j-1,1) == "]") compt3--;
    if (chaine(j-1,1) == "(") compt4++;
    if (chaine(j-1,1) == ")") compt4--;
    if (chaine(i-1,2)=="&&" && compt==0 && compt2==0 && et==0) {et=i;puiss=0;}
    if (chaine(i-1,2)=="||" && compt==0 && compt2==0 && ou==0) {puiss10=0; ou=i;}
    if (chaine(i-1,1)=="&" && compt==0 && compt2==0 && etx==0) {etx=i;puiss=0;}
    if (chaine(i-1,1)=="|" && compt==0 && compt2==0 && oux==0) {puiss10=0; oux=i;}
    if (chaine(i-1,1)==">" && compt==0 && compt2==0 && grand==0) {puiss10=0; grand=i;}
    if (chaine(i-1,1)=="<" && compt==0 && compt2==0 && petit==0) {puiss10=0; petit=i;}
    if ((chaine(i-1,2)=="<=" || chaine(i-1,2)=="=<") && compt==0 && compt2==0
        && peteg==0) {peteg=i; puiss10=0; petit=0;}
    if ((chaine(i-1,2)=="=>" || chaine(i-1,2)==">=") && compt==0 && compt2==0
        && grdeg==0) {puiss10=0; grdeg=i; grand=0;}
    if (chaine(i-1,2) == "==" && compt == 0 && compt2 == 0 && egal == 0) {puiss10=0; egal=i;}
    if (chaine(i-1,2) == "!=" && compt == 0 && compt2 == 0 && diff == 0) {puiss10=0; diff=i;}
    if (i>1 && chaine(i-1,1) == "+" && compt == 0 && compt2 == 0 && puiss10==0) plus=i;
    if (chaine(j-1,1) == "-" && chaine(j-2,1) != "*" && chaine(j-2,1) != "/"
       && chaine(j-2,1)!="^" && compt3==0 && compt4==0 && moins==0 && puiss10bis==0) moins=j;
    if (chaine(i-1,1)=="%" && compt==0 && compt2==0 && modulo==0) {puiss10=0; modulo=i;}
    if (chaine(i-1,1)=="*" && compt==0 && compt2==0 && multi==0)  {puiss10=0; multi=i;}
    if (chaine(j-1,1)=="/" && compt4==0 && compt3==0 && divi==0)  {puiss10=0; divi=j;}
    if (chaine(j-1,1)=="^" && compt4==0 && compt3==0 && puiss==0) {puiss10=0; puiss=j;}
    j--;
  }

//*-*- If operator found, analyze left and right part of the statement
//*-*  ===============================================================

  if (ou != 0) {    //check for ||
    if (ou==1 || ou==lchain-1) {
       err=5;
       chaine_error="||";
       }
    else {
       ctemp = chaine(0,ou-1);
       Analyze(already_found, ctemp.Data(),err);
       ctemp = chaine(ou+1,lchain-ou-1);
       Analyze(already_found, ctemp.Data(),err);
       fExpr[fNoper] = "||";
       fOper[fNoper] = 61;
       fNoper++;
    }
  } else if (et!=0) {
    if (et==1 || et==lchain-1) {
      err=5;
      chaine_error="&&";
      }
    else {
      ctemp = chaine(0,et-1);
      Analyze(already_found, ctemp.Data(),err);
      ctemp = chaine(et+1,lchain-et-1);
      Analyze(already_found, ctemp.Data(),err);
      fExpr[fNoper] = "&&";
      fOper[fNoper] = 60;
      fNoper++;
    }
  } else if (oux!=0) {
    if (oux==1 || oux==lchain) {
      err=5;
      chaine_error="|";
      }
    else {
      ctemp = chaine(0,oux-1);
      Analyze(already_found, ctemp.Data(),err);
      ctemp = chaine(oux,lchain-oux);
      Analyze(already_found, ctemp.Data(),err);
      fExpr[fNoper] = "|";
      fOper[fNoper] = 79;
      fNoper++;
    }
  } else if (etx!=0) {
    if (etx==1 || etx==lchain) {
      err=5;
      chaine_error="&";
      }
    else {
      ctemp = chaine(0,etx-1);
      Analyze(already_found, ctemp.Data(),err);
      ctemp = chaine(etx,lchain-etx);
      Analyze(already_found, ctemp.Data(),err);
      fExpr[fNoper] = "&";
      fOper[fNoper] = 78;
      fNoper++;
    }
  } else if (petit != 0) {
    if (petit==1 || petit==lchain) {
       err=5;
       chaine_error="<";
       }
    else {
       ctemp = chaine(0,petit-1);
       Analyze(already_found, ctemp.Data(),err);
       ctemp = chaine(petit,lchain-petit);
       Analyze(already_found, ctemp.Data(),err);
       fExpr[fNoper] = "<";
       fOper[fNoper] = 64;
       fNoper++;
    }
  } else if (grand != 0) {
    if (grand==1 || grand==lchain) {
       err=5;
       chaine_error=">";
       }
    else {
       ctemp = chaine(0,grand-1);
       Analyze(already_found, ctemp.Data(),err);
       ctemp = chaine(grand,lchain-grand);
       Analyze(already_found, ctemp.Data(),err);
       fExpr[fNoper] = ">";
       fOper[fNoper] = 65;
       fNoper++;
    }
  } else if (peteg != 0) {
    if (peteg==1 || peteg==lchain-1) {
       err=5;
       chaine_error="<=";
       }
    else {
       ctemp = chaine(0,peteg-1);
       Analyze(already_found, ctemp.Data(),err);
       ctemp = chaine(peteg+1,lchain-peteg-1);
       Analyze(already_found, ctemp.Data(),err);
       fExpr[fNoper] = "<=";
       fOper[fNoper] = 66;
       fNoper++;
    }
  } else if (grdeg != 0) {
    if (grdeg==1 || grdeg==lchain-1) {
       err=5;
       chaine_error="=>";
       }
    else {
       ctemp = chaine(0,grdeg-1);
       Analyze(already_found, ctemp.Data(),err);
       ctemp = chaine(grdeg+1,lchain-grdeg-1);
       Analyze(already_found, ctemp.Data(),err);
       fExpr[fNoper] = "=>";
       fOper[fNoper] = 67;
       fNoper++;
    }
  } else if (egal != 0) {
    if (egal==1 || egal==lchain-1) {
       err=5;
       chaine_error="==";
       }
    else {
       ctemp = chaine(0,egal-1);
       Analyze(already_found, ctemp.Data(),err);
       ctemp = chaine(egal+1,lchain-egal-1);
       Analyze(already_found, ctemp.Data(),err);
       fExpr[fNoper] = "==";
       fOper[fNoper] = 62;
       fNoper++;
    }
  } else if (diff != 0) {
    if (diff==1 || diff==lchain-1) {
       err=5;
       chaine_error = "!=";
       }
    else {
       ctemp = chaine(0,diff-1);
       Analyze(already_found, ctemp.Data(),err);
       ctemp = chaine(diff+1,lchain-diff-1);
       Analyze(already_found, ctemp.Data(),err);
       fExpr[fNoper] = "!=";
       fOper[fNoper] = 63;
       fNoper++;
    }
  } else
  if (plus != 0) {
    if (plus==lchain) {
      err=5;
      chaine_error = "+";
      }
    else {
       ctemp = chaine(0,plus-1);
       Analyze(already_found, ctemp.Data(),err);
       ctemp = chaine(plus,lchain-plus);
       Analyze(already_found, ctemp.Data(),err);
       fExpr[fNoper] = "+";
       fOper[fNoper] = 1;
       fNoper++;
    }

  } else {
    if (moins != 0) {
      if (moins == 1) {
        ctemp = chaine(moins,lchain-moins);
        Analyze(already_found, ctemp.Data(),err);
        fExpr[fNoper] = "-1";
        fOper[fNoper] = 0;
        fNoper++;
        fExpr[fNoper] = "-";
        fOper[fNoper] = 3;
        fNoper++;
      } else {
        if (moins == lchain) {
          err=5;
          chaine_error = "-";
        } else {
          ctemp = chaine(0,moins-1);
          Analyze(already_found, ctemp.Data(),err);
          ctemp = chaine(moins,lchain-moins);
          Analyze(already_found, ctemp.Data(),err);
          fExpr[fNoper] = "-";
          fOper[fNoper] = 2;
          fNoper++;
        }
      }
    } else if (modulo != 0) {
         if (modulo == 1 || modulo == lchain) {
            err=5;
            chaine_error="%";
         } else {
           ctemp = chaine(0,modulo-1);
           Analyze(already_found, ctemp.Data(),err);
           ctemp = chaine(modulo,lchain-modulo);
           Analyze(already_found, ctemp.Data(),err);
           fExpr[fNoper] = "%";
           fOper[fNoper] = 5;
           fNoper++;
         }
    } else {
      if (multi != 0) {
        if (multi == 1 || multi == lchain) {
          err=5;
          chaine_error="*";
          }
        else {
          ctemp = chaine(0,multi-1);
          Analyze(already_found, ctemp.Data(),err);
          ctemp = chaine(multi,lchain-multi);
          Analyze(already_found, ctemp.Data(),err);
          fExpr[fNoper] = "*";
          fOper[fNoper] = 3;
          fNoper++;
        }
      } else {
        if (divi != 0) {
          if (divi == 1 || divi == lchain) {
             err=5;
             chaine_error = "/";
             }
          else {
            ctemp = chaine(0,divi-1);
            Analyze(already_found, ctemp.Data(),err);
            ctemp = chaine(divi,lchain-divi);
            Analyze(already_found, ctemp.Data(),err);
            fExpr[fNoper] = "/";
            fOper[fNoper] = 4;
            fNoper++;
          }
        } else {
          if (puiss != 0) {
            if (puiss == 1 || puiss == lchain) {
               err = 5;
               chaine_error = "**";
               }
            else {
              if (chaine(lchain-2,2) == "^2") {
                 ctemp = "sq(" + chaine(0,lchain-2) + ")";
                 Analyze(already_found, ctemp.Data(),err);
              } else {
                 ctemp = chaine(0,puiss-1);
                 Analyze(already_found, ctemp.Data(),err);
                 ctemp = chaine(puiss,lchain-puiss);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "^";
                 fOper[fNoper] = 20;
                 fNoper++;
              }
            }
//*-*- Look for an already defined expression
          } else {
              find=0;
              //oldformula = 0;
              /*
              (TFormula*)ROOT::GetROOT()->GetListOfFunctions()->FindObject((const char*)chaine);
               if (oldformula && !oldformula->GetNumber()) {
                 Analyze(already_found, oldformula->GetTitle(),err);
                 find=1;
                 if (!err) {
                    Int_t npold = oldformula->GetNpar();
                    fNpar += npold;
                    for (Int_t ipar=0;ipar<npold;ipar++) {
                       fParams[ipar+fNpar-npold] = oldformula->GetParameter(ipar);
                    }
                 }
              }
                 */
          if (find == 0) {
//*-*- Check if chaine is a defined variable.
//*-*- Note that DefinedVariable can be overloaded
               k = DefinedVariable(chaine);
               if (k >= 5000 && k < 10000) {
                  fExpr[fNoper] = chaine;
                  fOper[fNoper] = 100000 + k;
                  fNstring++;
                  fNoper++;
               } else if ( k >= 0 ) {
                  fExpr[fNoper] = chaine;
                  fOper[fNoper] = 100000 + k;
                  if (k <MAXFOUND && !already_found[k]) {
                     already_found[k] = 1;
                     fNval++;
                  }
                  fNoper++;
               } else if (chaine(0,1) == "!") {
                  ctemp = chaine(1,lchain-1);
                  Analyze(already_found, ctemp.Data(),err);
                  fExpr[fNoper] = "!";
                  fOper[fNoper] = 68;
                  fNoper++;
               } else if (chaine(0,4) == "cos(") {
                  ctemp = chaine(3,lchain-3);
                  Analyze(already_found, ctemp.Data(),err);
                  fExpr[fNoper] = "cos";
                  fOper[fNoper] = 10;
                  fNoper++;
               } else if (chaine(0,4) == "sin(") {
                 ctemp = chaine(3,lchain-3);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "sin";
                 fOper[fNoper] = 11;
                 fNoper++;
               } else if (chaine(0,4) == "tan(") {
                 ctemp = chaine(3,lchain-3);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "tan";
                 fOper[fNoper] = 12;
                 fNoper++;
               } else if (chaine(0,5) == "acos(") {
                 ctemp = chaine(4,lchain-4);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "acos";
                 fOper[fNoper] = 13;
                 fNoper++;
               } else if (chaine(0,5) == "asin(") {
                 ctemp = chaine(4,lchain-4);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "asin";
                 fOper[fNoper] = 14;
                 fNoper++;
               } else if (chaine(0,5) == "atan(") {
                 ctemp = chaine(4,lchain-4);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "atan";
                 fOper[fNoper] = 15;
                 fNoper++;
                } else if (chaine(0,5) == "cosh(") {
                  ctemp = chaine(4,lchain-4);
                  Analyze(already_found, ctemp.Data(),err);
                  fExpr[fNoper] = "cosh";
                  fOper[fNoper] = 70;
                  fNoper++;
               } else if (chaine(0,5) == "sinh(") {
                 ctemp = chaine(4,lchain-4);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "sinh";
                 fOper[fNoper] = 71;
                 fNoper++;
               } else if (chaine(0,5) == "tanh(") {
                 ctemp = chaine(4,lchain-4);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "tanh";
                 fOper[fNoper] = 72;
                 fNoper++;
               } else if (chaine(0,6) == "acosh(") {
                 ctemp = chaine(5,lchain-5);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "acosh";
                 fOper[fNoper] = 73;
                 fNoper++;
               } else if (chaine(0,6) == "asinh(") {
                 ctemp = chaine(5,lchain-5);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "asinh";
                 fOper[fNoper] = 74;
                 fNoper++;
               } else if (chaine(0,6) == "atanh(") {
                 ctemp = chaine(5,lchain-5);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "atanh";
                 fOper[fNoper] = 75;
                 fNoper++;
               } else if (chaine(0,3) == "sq(") {
                 ctemp = chaine(2,lchain-2);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "sq";
                 fOper[fNoper] = 21;
                 fNoper++;
               } else if (chaine(0,4) == "log(") {
                 ctemp = chaine(3,lchain-3);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "log";
                 fOper[fNoper] = 30;
                 fNoper++;
               } else if (chaine(0,6) == "log10(") {
                 ctemp = chaine(5,lchain-5);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "log10";
                 fOper[fNoper] = 32;
                 fNoper++;
               } else if (chaine(0,4) == "exp(") {
                 ctemp = chaine(3,lchain-3);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "exp";
                 fOper[fNoper] = 31;
                 fNoper++;
               } else if (chaine(0,4) == "abs(") {
                 ctemp = chaine(3,lchain-3);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "abs";
                 fOper[fNoper] = 41;
                 fNoper++;
               } else if (chaine(0,5) == "sign(") {
                 ctemp = chaine(4,lchain-4);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "sign";
                 fOper[fNoper] = 42;
                 fNoper++;
               } else if (chaine(0,4) == "int(") {
                 ctemp = chaine(3,lchain-3);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "int";
                 fOper[fNoper] = 43;
                 fNoper++;
               } else if (chaine(0,4) == "rndm") {
                 fExpr[fNoper] = "rndm";
                 fOper[fNoper] = 50;
                 fNoper++;
               } else if (chaine(0,5) == "sqrt(") {
                 ctemp = chaine(4,lchain-4);
                 Analyze(already_found, ctemp.Data(),err);
                 fExpr[fNoper] = "sqrt";
                 fOper[fNoper] = 22;
                 fNoper++;
			
//*-*- Look for an exponential
//*-*  =======================
               } else if (chaine(0,4)=="expo" || chaine(1,4)=="expo" || chaine(2,4)=="expo") {
                 chaine1ST=chaine;
                 if (chaine(1,4) == "expo") {
                    ctemp=chaine(0,1);
                    if (ctemp=="x") {
                       inter2=0;
                       if (fNdim < 1) fNdim = 1; }
                    else if (ctemp=="y") {
                            inter2=1;
                            if (fNdim < 2) fNdim = 2; }
                         else if (ctemp=="z") {
                                inter2=2;
                                if (fNdim < 3) fNdim = 3; }
                              else if (ctemp=="t") {
                                     inter2=3;
                                     if (fNdim < 4) fNdim = 4; }
                                   else {
                                      err=26; // unknown name;
                                      chaine_error=chaine1ST;
                                   }
                   chaine=chaine(1,lchain-1);
                   lchain=chaine.Length();
  // a partir d'ici indentation decalee de 4 vers la gauche
             } else inter2=0;
             if (chaine(2,4) == "expo") {
               if (chaine(0,2) != "xy") {
                   err=26; // unknown name
                   chaine_error=chaine1ST;
                   }
               else {
                   inter2=5;
                   if (fNdim < 2) fNdim = 2;
                   chaine=chaine(2,lchain-2);
                   lchain=chaine.Length();
               }
            }
            if (lchain == 4) {
                if (fNpar>=MAXPAR) err=7; // too many parameters
                if (!err) {
                   fExpr[fNoper] = "E";
                   fOper[fNoper] = 1001+100*inter2;
                   if (inter2 == 5 && fNpar < 3) fNpar = 3;
                   if (fNpar < 2) fNpar = 2;
                   if (fNpar>=MAXPAR) err=7; // too many parameters
                   if (!err) {
                      fNoper++;
                      if (fNdim < 1) fNdim = 1;
                      SetNumber(200);
                   }
                }
            } else if (chaine(4,1) == "(") {
                      ctemp = chaine(5,lchain-6);
                      fExpr[fNoper] = "E";
                      for (j=0; j<ctemp.Length(); j++) {
                        t=ctemp[j];
                        if (strchr("0123456789",t)==0 && (ctemp(j,1)!="+" || j!=0)) {
                           err=20;
                           chaine_error=chaine1ST;
                        }
                      }
                      if (err==0) {
                         sscanf(ctemp.Data(),"%d",&inter);
                         if (inter>=0) {
                            fOper[fNoper] = 1001+inter+100*inter2;
                            if (inter2 == 5) inter++;
                            if (inter+2>fNpar) fNpar = inter+2;
                            if (fNpar>=MAXPAR) err=7; // too many parameters
                            if (!err) fNoper++;
                            SetNumber(200);
                         } else err=20;
                      } else err = 20; // non integer value for parameter number
                    } else {
                        err=26; // unknown name
                        chaine_error=chaine;
                      }
//*-*- Look for gaus, xgaus,ygaus,xygaus
//*-*  =================================
          } else if (chaine(0,4)=="gaus" || chaine(1,4)=="gaus" || chaine(2,4)=="gaus") {
            chaine1ST=chaine;
            if (chaine(1,4) == "gaus") {
               ctemp=chaine(0,1);
               if (ctemp=="x") {
                  inter2=0;
                  if (fNdim < 1) fNdim = 1; }
               else if (ctemp=="y") {
                       inter2=1;
                       if (fNdim < 2) fNdim = 2; }
                    else if (ctemp=="z") {
                            inter2=2;
                            if (fNdim < 3) fNdim = 3; }
                         else if (ctemp=="t") {
                                 inter2=3;
                                 if (fNdim < 4) fNdim = 4; }
                              else {
                                  err=26; // unknown name
                                  chaine_error=chaine1ST;
                              }
               chaine=chaine(1,lchain-1);
               lchain=chaine.Length();
            } else inter2=0;
            if (chaine(2,4) == "gaus") {
               if (chaine(0,2) != "xy") {
                   err=26; // unknown name
                   chaine_error=chaine1ST;
                   }
               else {
                   inter2=5;
                   if (fNdim < 2) fNdim = 2;
                   chaine=chaine(2,lchain-2);
                   lchain=chaine.Length();
               }
            }
            if (lchain == 4 && err==0) {
                if (fNpar>=MAXPAR) err=7; // too many parameters
                if (!err) {
                   fExpr[fNoper] = "G";
                   fOper[fNoper] = 2001+100*inter2;
                   if (inter2 == 5 && fNpar < 5) fNpar = 5;
                   if (3>fNpar) fNpar = 3;
                   if (fNpar>=MAXPAR) err=7; // too many parameters
                   if (!err) {
                      fNoper++;
                      if (fNdim < 1) fNdim = 1;
                      SetNumber(100);
                   }
                }
            } else if (chaine(4,1) == "(" && err==0) {
                      ctemp = chaine(5,lchain-6);
                      fExpr[fNoper] = "G";
                      for (j=0; j<ctemp.Length(); j++) {
                        t=ctemp[j];
                        if (strchr("0123456789",t)==0 && (ctemp(j,1)!="+" || j!=0)) {
                           err=20;
                           chaine_error=chaine1ST;
                        }
                      }
                      if (err==0) {
                          sscanf(ctemp.Data(),"%d",&inter);
                          if (inter >= 0) {
                             fOper[fNoper] = 2001+inter+100*inter2;
                             if (inter2 == 5) inter += 2;
                             if (inter+3>fNpar) fNpar = inter+3;
                             if (fNpar>=MAXPAR) err=7; // too many parameters
                             if (!err) fNoper++;
                             SetNumber(100);
                         } else err = 20; // non integer value for parameter number
                      }
                   } else if (err==0) {
                       err=26; // unknown name
                       chaine_error=chaine1ST;
                     }
//*-*- Look for landau, xlandau,ylandau,xylandau
//*-*  =================================
          } else if (chaine(0,6)=="landau" || chaine(1,6)=="landau" || chaine(2,6)=="landau") {
            chaine1ST=chaine;
            if (chaine(1,6) == "landau") {
               ctemp=chaine(0,1);
               if (ctemp=="x") {
                  inter2=0;
                  if (fNdim < 1) fNdim = 1; }
               else if (ctemp=="y") {
                       inter2=1;
                       if (fNdim < 2) fNdim = 2; }
                    else if (ctemp=="z") {
                            inter2=2;
                            if (fNdim < 3) fNdim = 3; }
                         else if (ctemp=="t") {
                                 inter2=3;
                                 if (fNdim < 4) fNdim = 4; }
                              else {
                                  err=26; // unknown name
                                  chaine_error=chaine1ST;
                              }
               chaine=chaine(1,lchain-1);
               lchain=chaine.Length();
            } else inter2=0;
            if (chaine(2,6) == "landau") {
               if (chaine(0,2) != "xy") {
                   err=26; // unknown name
                   chaine_error=chaine1ST;
                   }
               else {
                   inter2=5;
                   if (fNdim < 2) fNdim = 2;
                   chaine=chaine(2,lchain-2);
                   lchain=chaine.Length();
               }
            }
            if (lchain == 6 && err==0) {
                if (fNpar>=MAXPAR) err=7; // too many parameters
                if (!err) {
                   fExpr[fNoper] = "L";
                   fOper[fNoper] = 4001+100*inter2;
                   if (inter2 == 5 && fNpar < 5) fNpar = 5;
                   if (3>fNpar) fNpar = 3;
                   if (fNpar>=MAXPAR) err=7; // too many parameters
                   if (!err) {
                      fNoper++;
                      if (fNdim < 1) fNdim = 1;
                      SetNumber(400);
                   }
                }
            } else if (chaine(6,1) == "(" && err==0) {
                      ctemp = chaine(7,lchain-8);
                      fExpr[fNoper] = "L";
                      for (j=0; j<ctemp.Length(); j++) {
                        t=ctemp[j];
                        if (strchr("0123456789",t)==0 && (ctemp(j,1)!="+" || j!=0)) {
                           err=20;
                           chaine_error=chaine1ST;
                        }
                      }
                      if (err==0) {
                          sscanf(ctemp.Data(),"%d",&inter);
                          if (inter >= 0) {
                             fOper[fNoper] = 4001+inter+100*inter2;
                             if (inter2 == 5) inter += 2;
                             if (inter+3>fNpar) fNpar = inter+3;
                             if (fNpar>=MAXPAR) err=7; // too many parameters
                             if (!err) fNoper++;
                             SetNumber(400);
                         } else err = 20; // non integer value for parameter number
                      }
                   } else if (err==0) {
                       err=26; // unknown name
                       chaine_error=chaine1ST;
                     }
//*-*- Look for a polynomial
//*-*  =====================
          } else if (chaine(0,3) == "pol" || chaine(1,3) == "pol") {
            chaine1ST=chaine;
            if (chaine(1,3) == "pol") {
               ctemp=chaine(0,1);
               if (ctemp=="x") {
                  inter2=1;
                  if (fNdim < 1) fNdim = 1; }
               else if (ctemp=="y") {
                       inter2=2;
                       if (fNdim < 2) fNdim = 2; }
                    else if (ctemp=="z") {
                            inter2=3;
                            if (fNdim < 3) fNdim = 3; }
                         else if (ctemp=="t") {
                                 inter2=4;
                                 if (fNdim < 4) fNdim = 4; }
                              else {
                                 err=26; // unknown name;
                                 chaine_error=chaine1ST;
                              }
               chaine=chaine(1,lchain-1);
               lchain=chaine.Length();
            } else inter2=1;
            if (chaine(lchain-1,1) == ")") {
                nomb = 0;
                for (j=3;j<lchain;j++) if (chaine(j,1)=="(" && nomb == 0) nomb = j;
                if (nomb == 3) err = 23; // degree of polynomial not specified
                if (nomb == 0) err = 40; // '(' is expected
                ctemp = chaine(nomb+1,lchain-nomb-2);
                for (j=0; j<ctemp.Length(); j++) {
                        t=ctemp[j];
                        if (strchr("0123456789",t)==0 && (ctemp(j,1)!="+" || j!=0)) {
                           err=20;
                           chaine_error=chaine1ST;
                        }
                }
                if (!err) {
                   sscanf(ctemp.Data(),"%d",&inter);
                   if (inter < 0) err = 20;
                }
            }
            else {
              nomb = lchain;
              inter = 0;
            }
            if (!err) {
              inter--;
              ctemp = chaine(3,nomb-3);
              if (sscanf(ctemp.Data(),"%d",&n) > 0) {
                 if (n < 0  ) err = 24; //Degree of polynomial must be positive
                 if (n >= 20) err = 25; //Degree of polynomial must be less than 20
              } else err = 20;
            }
            if (!err) {
              fExpr[fNoper] = "P";
              fOper[fNoper] = 10000*inter2+n*100+inter+2;
              if (inter+n+1>=fNpar) fNpar = inter + n + 2;
              if (fNpar>=MAXPAR) err=7; // too many parameters
              if (!err) {
                 fNoper++;
                 if (fNdim < 1) fNdim = 1;
                 SetNumber(300+n);
              }
            }
//*-*- Look for pow,atan2,etc
//*-*  ======================
          } else if (chaine(0,4) == "pow(") {
            compt = 4; nomb = 0; virgule = 0;
            while(compt != lchain) {
              compt++;
              if (chaine(compt-1,1) == ",") nomb++;
              if (nomb == 1 && virgule == 0) virgule = compt;
            }
            if (nomb != 1) err = 22; // There are plus or minus than 2 arguments for pow
            else {
              ctemp = chaine(4,virgule-5);
              Analyze(already_found, ctemp.Data(),err);
              ctemp = chaine(virgule,lchain-virgule-1);
              Analyze(already_found, ctemp.Data(),err);
              fExpr[fNoper] = "";
              fOper[fNoper] = 20;
              fNoper++;
            }
		  } else if (chaine(0,7) == "strstr(") {
            compt = 7; nomb = 0; virgule = 0;
            while(compt != lchain) {
              compt++;
              if (chaine(compt-1,1) == ",") nomb++;
              if (nomb == 1 && virgule == 0) virgule = compt;
            }
            if (nomb != 1) err = 28; // There are plus or minus than 2 arguments for strstr
            else {
              ctemp = chaine(7,virgule-8);
              Analyze(already_found, ctemp.Data(),err);
              ctemp = chaine(virgule,lchain-virgule-1);
              Analyze(already_found, ctemp.Data(),err);
              fExpr[fNoper] = "";
              fOper[fNoper] = 23;
              fNoper++;
            }

          } else if (chaine(0,6) == "atan2(") {
            compt = 6; nomb = 0; virgule = 0;
            while(compt != lchain) {
              compt++;
              if (chaine(compt-1,1) == ",") nomb++;
              if (nomb == 1 && virgule == 0) virgule = compt;
            }
            if (nomb != 1) err = 21;  //{ There are plus or minus than 2 arguments for atan2
            else {
              ctemp = chaine(6,virgule-7);
              Analyze(already_found, ctemp.Data(),err);
              ctemp = chaine(virgule,lchain-virgule-1);
              Analyze(already_found, ctemp.Data(),err);
              fExpr[fNoper] = "";
              fOper[fNoper] = 16;
              fNoper++;
            }
          } else if (chaine(0,5) == "fmod(") {
            compt = 5; nomb = 0; virgule = 0;
            while(compt != lchain) {
              compt++;
              if (chaine(compt-1,1) == ",") nomb++;
              if (nomb == 1 && virgule == 0) virgule = compt;
            }
            if (nomb != 1) err = 21;  //{ There are plus or minus than 2 arguments for atan2
            else {
              ctemp = chaine(5,virgule-6);
              Analyze(already_found, ctemp.Data(),err);
              ctemp = chaine(virgule,lchain-virgule-1);
              Analyze(already_found, ctemp.Data(),err);
              fExpr[fNoper] = "";
              fOper[fNoper] = 17;
              fNoper++;
            }
          } else if (chaine(0,1) == "[" && chaine(lchain-1,1) == "]") {
            fExpr[fNoper] = chaine;
            fNoper++;
            ctemp = chaine(1,lchain-2);
            for (j=0; j<ctemp.Length(); j++) {
                t=ctemp[j];
                if (strchr("0123456789",t)==0 && (ctemp(j,1)!="+" || j!=0)) {
                   err=20;
                   chaine_error=chaine1ST; // le numero ? de par[?] n'est pas un entier }
                }
            }
            if (!err) {
              sscanf(ctemp.Data(),"%d",&valeur);
              fOper[fNoper-1] = valeur + 101;
            }
          } else if (chaine == "pi") {
            fExpr[fNoper] = "P";
            fOper[fNoper] = 40;
            fNoper++;
//*-*- None of the above. Must be a numerical expression
//*-*  =================================================
          }
//*-*- Maybe it is a string
          else
            if (chaine(0,1)==""" && chaine(chaine.Length()-1,1)==""") {
               //*-* It is a string !!!
               fExpr[fNoper] = chaine(1,chaine.Length()-2);
               fOper[fNoper] = 80000;
               fNoper++;
            }
            else {
            compt=0;compt2=0;hexa=0;
            if ((chaine(0,2)=="0x")||(chaine(0,2)=="0X")) hexa=1;
            for (j=0; j<chaine.Length(); j++) {
                t=chaine[j];
                if (hexa==0) {
              	      if (chaine(j,1)=="e" || chaine(j,2)=="e+" || chaine(j,2)=="e-") {
                 	  compt2++;
                 	  compt = 0;
                 	  if (chaine(j,2)=="e+" || chaine(j,2)=="e-") j++;
                	  if (compt2>1) {
                	       err=26;
                 	       chaine_error=chaine;
                	  }
                      }
                      else {
                          if (chaine(j,1) == ".") compt++;
                          else {
                             if (!strchr("0123456789",t) && (chaine(j,1)!="+" || j!=0) && compt!=1) {
                                err=26;
                                chaine_error=chaine;
                             }
                          }
                      }
                }
                else {
                      if (!strchr("0123456789abcdefABCDEF",t) && (j>1)) {
                          err=26;
                          chaine_error=chaine;
                      }
                }
            }
            if (fNconst >= MAXCONST) err = 27;
            if (!err) {
               if (hexa==0) {if (sscanf((const char*)chaine,"%g",&vafConst) > 0) err = 0; else err =1;}
               else {if (sscanf((const char*)chaine,"%x",&vafConst2) > 0) err = 0; else err=1;
               vafConst = (Float_t) vafConst2;}
               fExpr[fNoper] = chaine;
               k = -1;
               for (j=0;j<fNconst;j++) {
                  if (vafConst == fConst[j] ) k= j;
               }
               if ( k < 0) {  k = fNconst; fNconst++; fConst[k] = vafConst; }
               fOper[fNoper] = 50000 + k;
               fNoper++;
            }
          }
        }
      }
      }
    }
  }
 }

//   Test  * si y existe :  que x existe
//         * si z existe :  que x et y existent

  nomb = 1;
  for (i=1; i<=fNoper; i++) {
   if (fOper[i-1] == 97 && nomb > 0) nomb *= -1;
    if (fOper[i-1] == 98 && TMath::Abs(nomb) != 2) nomb *= 2;
    if (fOper[i-1] == 99 && TMath::Abs(nomb) != 20 && TMath::Abs(nomb) != 10) nomb *= 10;
  }
  if (nomb == 10)  err = 10; //{variable z sans x et y }
  if (nomb == 20)  err = 11; //{variables z et y sans x }
  if (nomb == 2)   err = 12; //{variable y sans x }
  if (nomb == -10) err = 13; //{variables z et x sans y }
//*-*- Overflows
  if (fNoper>=MAXOP) err=6; // too many operators

}

//*-*- errors!
  if (err>1) {
    std::cout<<std::endl<<"*ERROR "<<err<<" : "<<std::endl;
     switch(err) {
      case  2 : std::cout<<" Invalid Floating Point Operation"<<std::endl; break;
      case  4 : std::cout<<" Empty String"<<std::endl; break;
      case  5 : std::cout<<" Invalid Syntax ""<<(const char*)chaine_error<<"""<<std::endl; break;
      case  6 : std::cout<<" Too many operators !"<<std::endl; break;
      case  7 : std::cout<<" Too many parameters !"<<std::endl; break;
      case 10 : std::cout<<" z specified but not x and y"<<std::endl; break;
      case 11 : std::cout<<" z and y specified but not x"<<std::endl; break;
      case 12 : std::cout<<" y specified but not x"<<std::endl; break;
      case 13 : std::cout<<" z and x specified but not y"<<std::endl; break;
      case 20 : std::cout<<" Non integer value for parameter number : "<<(const char*)chaine_error<<std::endl; break;
      case 21 : std::cout<<" ATAN2 requires two arguments"<<std::endl; break;
      case 22 : std::cout<<" POW requires two arguments"<<std::endl; break;
      case 23 : std::cout<<" Degree of polynomial not specified"<<std::endl; break;
      case 24 : std::cout<<" Degree of polynomial must be positive"<<std::endl; break;
      case 25 : std::cout<<" Degree of polynomial must be less than 20"<<std::endl; break;
      case 26 : std::cout<<" Unknown name : ""<<(const char*)chaine_error<<"""<<std::endl; break;
      case 27 : std::cout<<" Too many constants in expression"<<std::endl; break;
      case 28 : std::cout<<" strstr requires tow arguments"<<std::endl; break;
      case 40 : std::cout<<" '(' is expected"<<std::endl; break;
      case 41 : std::cout<<" ')' is expected"<<std::endl; break;
      case 42 : std::cout<<" '[' is expected"<<std::endl; break;
      case 43 : std::cout<<" ']' is expected"<<std::endl; break;
     }
  err=1;
  }

}
