
namespace Tests {

  struct ICnvFactory {
    virtual ~ICnvFactory() {}
    virtual IConverter* create(ISvcLocator *svcloc) const = 0;
  };

  template <typename T> struct TheCnvFactory : public ICnvFactory {
    static ICnvFactory& instance() { static TheCnvFactory<T> inst; return inst; }
    virtual IConverter* create(ISvcLocator *svcloc) const {  return new T(svcloc);  }
  };
}

#define MAKE_CONVERTER_FACTORY(x) ::Tests::ICnvFactory& x##Factory = TheCnvFactory<x>::instance();
