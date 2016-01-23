
namespace ANNGlobalPID
{

#ifndef IClassifierReader__def
#define IClassifierReader__def
  /** @class IClassifierReader TMVAImpFactory.h
   *
   *  Interface class for TMVA C++ implementation of MVAs
   */
  class IClassifierReader
  {
  public:
    /// Destructor
    virtual ~IClassifierReader() { }
    /// return classifier response
    virtual double GetMvaValue( const std::vector<double>& inputValues ) const = 0;
    /// returns classifier status
    bool IsStatusClean() const { return fStatusIsClean; }
  protected:
    bool fStatusIsClean{true}; ///< Status flag
  };
#endif

}
