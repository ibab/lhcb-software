#include "TH1D.h"
#include "TProfile.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TArrayD.h"
class MyTH1D : public TH1D
{
public:
  friend class MonHist;
  double *GetEntryArr(void){return fArray;};
  double *GetSumw2Arr(void){return GetSumw2()->GetArray();};
};

class MyTProfile : public TProfile
{
public:
  friend class MonHist;
  MyTProfile()
  {

  }
  virtual ~MyTProfile(){};
  double *GetSumwArr(void)
  {
//    printf("MyProfile::GetSumwArr %x\n",fArray);
    return fArray;
  };
  double *GetEntryArr(void)
  {
//    printf("MyProfile::GetEntryArr %x\n",fBinEntries.fArray);
    return fBinEntries.fArray;
  };
  double *GetSumw2Arr(void)
  {
//    printf("MyProfile::GetSumw2Arr %x\n",GetSumw2()->fArray);
    return GetSumw2()->GetArray();
  };
  double *GetBinSumw2Arr(void)
  {
    return GetBinSumw2()->GetArray();
  }
};

class MyTH2D : public TH2D
{
public:
  friend class MonHist;
  double *GetEntryArr(void){return fArray;};
  double *GetSumw2Arr(void){return GetSumw2()->GetArray();};
};
class MyTH3D : public TH3D
{
public:
  friend class MonHist;
  double *GetEntryArr(void){return fArray;};
  double *GetSumw2Arr(void){return GetSumw2()->GetArray();};
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
