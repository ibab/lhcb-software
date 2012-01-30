#ifndef DETDESC_ELEMENTS_H
#define DETDESC_ELEMENTS_H

#include <string>
#include <typeinfo>
#include <stdexcept>
#include <TNamed.h>

class TObject;
class TObjArray;
class TGeoManager;

/*
 *   Gaudi namespace declaration
 */
namespace DetDesc {

  namespace Geometry  {
    struct  LCDD;
    //struct  Element;
    //struct  RefElement;

    typedef TObject     Element_t;
    typedef TObjArray   NodeList_t;
    typedef TGeoManager Document_t;

    std::string    _toString(bool value);
    std::string    _toString(int value);
    std::string    _toString(float value);
    std::string    _toString(double value);

    bool           _toBool  (const std::string& value);
    int            _toInt   (const std::string& value);
    float          _toFloat (const std::string& value);
    double         _toDouble(const std::string& value);

    inline bool    _toBool(bool value)       {  return value; }
    inline int     _toInt(int value)         {  return value; }
    inline float   _toFloat(float value)     {  return value; }
    inline double  _toDouble(double value)   {  return value; }

    void _toDictionary(const std::string& name, const std::string& value);

    template<typename T> inline void deletePtr(T*& p) { if(p) delete p; p=0; }

    template <typename Q, typename P> struct Value : public Q, public P  {
      typedef  Q first_base;
      typedef  P second_base;
      virtual ~Value() {}
    };

    template <typename T> struct Element_type  {
      typedef T Implementation;

      Implementation* m_element;
      Element_type() : m_element(0) {}
      Element_type(Implementation* e) : m_element(e) {}
      Element_type(const Element_type<Implementation>& e) : m_element(e.m_element) {}

      template<typename Q> Element_type(Q* e) : m_element((T*)e) { verifyObject<T>(); }

      template<typename Q> Element_type(const Element_type<Q>& e) 
	: m_element((T*)e.m_element) { verifyObject<T>(); }

      T* ptr() const                          {  return m_element;      }
      template <typename Q> Q* _ptr() const   {  return (Q*)m_element;  }
      bool isValid() const                    {  return 0 != m_element; }
      bool operator!() const                  {  return 0 == m_element; }
      template <typename P, typename Q> Q* data() const  {
	return (Value<P,Q>*)m_element;
      }
      template<typename Q> void verifyObject() const {
	if ( dynamic_cast<Q*>(ptr()) == 0 )  {
	  bad_assignment(ptr() ? typeid(*ptr()) : typeid(void),typeid(Q));
	}
      }
      static void bad_assignment(const std::type_info& from, const std::type_info& to);
    };

    template <typename T> struct RefElement_type : public Element_type<T>  {
      typedef T Implementation;
      RefElement_type() : Element_type<T>() {}
      RefElement_type(const Element_type<T>& e) : Element_type<T>(e) {}

      template<typename Q> 
      RefElement_type(const Element_type<Q>& e) : Element_type<T>(e) {}

      const char* name() const;
      const char* refName() const;
      void setName(const std::string& new_name);
      void assign(Implementation* n, const std::string& nam, const std::string& title);
      operator RefElement_type<TNamed>() const { return RefElement_type<TNamed>(this->m_element); }
    };

    typedef RefElement_type<TNamed> RefElement;
    typedef Element_type<TObject>    Element;

  }       /* End namespace Geometry  */
}         /* End namespace DetDesc   */
#endif    /* DETDESC_ELEMENTS_H      */
