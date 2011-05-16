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
  public:
    WalkRelation();
    WalkRelation(double off, double amp, double tau, double dpt);

    double walk(double l) const;

    double getOff() const { return off; }
    double getAmp() const { return amp; }
    double getTau() const { return tau; }
    double getDpt() const { return dpt; }
  };
}

#endif // OTDET_WALKRELATION
