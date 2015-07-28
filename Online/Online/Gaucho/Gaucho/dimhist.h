#ifndef DIMHIST_DEF
#define DIMHIST_DEF
//#define DIMTYPE DimUpdatedInfo
#define DIMTYPE DimUpdatedInfo
#define FLAGS_XLABELS 1<<0
#define FLAGS_YLABELS 1<<1

class DimBuffBase
{
public:
  DimBuffBase()
  {
    reclen = sizeof(DimBuffBase);
    nameoff = 0;
    titoff = 0;
    namelen = 0;
    titlen = 0;
    dataoff = 0;
    flags = 0;
  }
  int reclen;
  int type;
  int flags;
  int nameoff;
  int namelen;
  int titoff;
  int titlen;
  int dataoff;
  int addoffset;
};
class DimStatBuff : public DimBuffBase
{
public:
  double nentries;
  double m_sumw;
  double m_sumw2;
  double m_min;
  double m_max;
};
class DimHistbuff1 : public DimBuffBase
{
public:
  int dim;
  double nentries;
  double m_sumw;
  double m_sumw2;
  double m_sumwx;
  double m_sumwx2;
  double m_sumwx3;
  double m_sumwx4;
  double m_sumwy;
  double m_sumwy2;
  double m_sumwy3;
  double m_sumwy4;
  int nxbin;
  double xmin;
  double xmax;
  double yminval;
  double ymaxval;
  int xlablen;
  int xlaboff;
};
class DimHistbuff2 : public DimHistbuff1
{
public:
  int nybin;
  double ymin;
  double ymax;
  int ylablen;
  int ylaboff;
};
class DimHistbuff3 : public DimHistbuff2
{
public:
  int nzbin;
  double zmin;
  double zmax;
  int zlablen;
  int zlaboff;
  double zminval;
  double zmaxval;
};

#endif
