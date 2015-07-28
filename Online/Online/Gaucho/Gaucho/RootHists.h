#include "TH1D.h"
#include "TProfile.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TArrayD.h"
#include "TProfile2D.h"
#include "Gaucho/Utilities.h"
class MyTH1D : public TH1D
{
public:
  friend class MonHist;
  double *GetEntryArr(void){return fArray;};
  double *GetSumw2Arr(void){return GetSumw2()->GetArray();};
  void movetodimbuffer(void *base)
  {
    int m_blocksize;
    m_blocksize = fN*sizeof(double);
    memcpy(AddPtr(base,0*m_blocksize),GetEntryArr(),m_blocksize);
    memcpy(AddPtr(base,1*m_blocksize),GetSumw2Arr(),m_blocksize);
    return;
  }
  void movefromdimbuffer(void *base)
  {
    int m_blocksize;
    m_blocksize = fN*sizeof(double);
    memcpy(GetEntryArr(),AddPtr(base,0*m_blocksize),m_blocksize);
    memcpy(GetSumw2Arr(),AddPtr(base,1*m_blocksize),m_blocksize);
    return;
  }
};

class MyTProfile : public TProfile
{
public:
  friend class MonHist;
  double *GetSumwArr(void){return fArray;};
  double *GetEntryArr(void){return fBinEntries.fArray;};
  double *GetSumw2Arr(void){return GetSumw2()->GetArray();};
  double *GetBinSumw2Arr(void){return GetBinSumw2()->GetArray();};
  void movetodimbuffer(void *base)
  {
    int m_blocksize;
    m_blocksize = fN*sizeof(double);
    memcpy(AddPtr(base,0*m_blocksize),GetEntryArr(),m_blocksize);
    memcpy(AddPtr(base,1*m_blocksize),GetSumwArr(),m_blocksize);
    memcpy(AddPtr(base,2*m_blocksize),GetSumw2Arr(),m_blocksize);
    memcpy(AddPtr(base,3*m_blocksize),GetBinSumw2Arr(),m_blocksize);
    return;
  }
  void movefromdimbuffer(void *base)
  {
    int m_blocksize;
    m_blocksize = fN*sizeof(double);
    memcpy(GetEntryArr(),AddPtr(base,0*m_blocksize),m_blocksize);
    memcpy(GetSumwArr(),AddPtr(base,1*m_blocksize),m_blocksize);
    memcpy(GetSumw2Arr(),AddPtr(base,2*m_blocksize),m_blocksize);
    memcpy(GetBinSumw2Arr(),AddPtr(base,3*m_blocksize),m_blocksize);
    return;
  }
};

class MyTH2D : public TH2D
{
public:
  friend class MonHist;
  double *GetEntryArr(void){return fArray;};
  double *GetSumw2Arr(void){return GetSumw2()->GetArray();};
  void movetodimbuffer(void *base)
  {
    int m_blocksize;
    m_blocksize = fN*sizeof(double);
    memcpy(AddPtr(base,0*m_blocksize),GetEntryArr(),m_blocksize);
    memcpy(AddPtr(base,1*m_blocksize),GetSumw2Arr(),m_blocksize);
    return;
  }
  void movefromdimbuffer(void *base)
  {
    int m_blocksize;
    m_blocksize = fN*sizeof(double);
    memcpy(GetEntryArr(),AddPtr(base,0*m_blocksize),m_blocksize);
    memcpy(GetSumw2Arr(),AddPtr(base,1*m_blocksize),m_blocksize);
    return;
  }
};
class MyTH3D : public TH3D
{
public:
  friend class MonHist;
  double *GetEntryArr(void){return fArray;};
  double *GetSumw2Arr(void){return GetSumw2()->GetArray();};
  void movetodimbuffer(void *base)
  {
    int m_blocksize;
    m_blocksize = fN*sizeof(double);
    memcpy(AddPtr(base,0*m_blocksize),GetEntryArr(),m_blocksize);
    memcpy(AddPtr(base,1*m_blocksize),GetSumw2Arr(),m_blocksize);
    return;
  }
  void movefromdimbuffer(void *base)
  {
    int m_blocksize;
    m_blocksize = fN*sizeof(double);
    memcpy(GetEntryArr(),AddPtr(base,0*m_blocksize),m_blocksize);
    memcpy(GetSumw2Arr(),AddPtr(base,1*m_blocksize),m_blocksize);
    return;
  }
};
class MyTProfile2D : public TProfile2D
{
public:
  friend class MonHist;
  double *GetSumwArr(void){return fArray;};
  double *GetEntryArr(void){return fBinEntries.GetArray();};
  double *GetSumw2Arr(void){return GetSumw2()->GetArray();};
  double *GetBinSumw2Arr(void){return GetBinSumw2()->GetArray();};
  int NEntries(void){return fN;}
  int NBinEntries(void){return fBinEntries.fN;};
  int NSumw2(void){return GetSumw2()->fN;}
  int NBinSumw2(void){return GetBinSumw2()->fN;}
  void SetfZmin(double d){fZmin =d;return;};
  void SetfZmax(double d){fZmax =d;return;};
  void movetodimbuffer(void *base)
  {
    int m_blocksize;
    m_blocksize = fN*sizeof(double);
    memcpy(AddPtr(base,0*m_blocksize),GetEntryArr(),m_blocksize);
    memcpy(AddPtr(base,1*m_blocksize),GetSumwArr(),m_blocksize);
    memcpy(AddPtr(base,2*m_blocksize),GetSumw2Arr(),m_blocksize);
    memcpy(AddPtr(base,3*m_blocksize),GetBinSumw2Arr(),m_blocksize);
    return;
  }
  void movefromdimbuffer(void *base)
  {
    int m_blocksize;
    m_blocksize = fN*sizeof(double);
    memcpy(GetEntryArr(),AddPtr(base,0*m_blocksize),m_blocksize);
    memcpy(GetSumwArr(),AddPtr(base,1*m_blocksize),m_blocksize);
    memcpy(GetSumw2Arr(),AddPtr(base,2*m_blocksize),m_blocksize);
    memcpy(GetBinSumw2Arr(),AddPtr(base,3*m_blocksize),m_blocksize);
    return;
  }
};
class MyTArrayD : public TArrayD
{
public:
  void Dump(char *nam=0)
  {
    if (nam == 0)
    {
      nam = (char*)"TArrayD<noname>";
    }
    for (int i=0;i<fN;i++)
    {
      printf("   %s %d %f\n",nam,i,fArray[i]);
    }
  }
};
