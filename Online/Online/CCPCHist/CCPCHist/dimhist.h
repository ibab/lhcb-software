#ifndef DIMHIST_DEF
#define DIMHIST_DEF
//#define DIMTYPE DimUpdatedInfo
#define DIMTYPE DimUpdatedInfo
class DimBuffBase
{
public:
  int reclen;
  int type;
  int nameoff;
  int namelen;
  int titoff;
  int titlen;
  int dataoff;
  int addoffset;
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
};
class DimHistbuff2 : public DimHistbuff1
{
public:
  int nybin;
  double ymin;
  double ymax;
};

#endif
