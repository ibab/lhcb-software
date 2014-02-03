#ifndef OTDET_WALKRELATION
#define OTDET_WALKRELATION

namespace OTDet
{
  /************************************************************************/
  /*                                                                      */
  /************************************************************************/
  class WalkRelation
  {
  private:
    double off;
    double amp;
    double tau;
    double dpt;
    double dptfactor;
    double overtau;

    inline double fastTanh(const double x) const;
    //inline int float2int(const double d ) const;
    //inline int floattoint(const double d ) const;

  public:
    WalkRelation();
    WalkRelation(double doff, double damp, double dtau, double ddpt);

    double walk(double l) const;

    inline double getOff() const { return off; }
    inline double getAmp() const { return amp; }
    inline double getTau() const { return tau; }
    inline double getDpt() const { return dpt; }
  };
}

#endif // OTDET_WALKRELATION
