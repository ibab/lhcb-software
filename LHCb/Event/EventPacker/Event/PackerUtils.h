#ifndef EVENT_PACKERUTILS_H 
#define EVENT_PACKERUTILS_H 1

namespace LHCb
{
  namespace Packer
  {
    namespace Utils
    {

      /** Returns the full location of the given object in the Data Store
       *
       *  @param pObj Data object
       *
       *  @return Location of given data object
       */
      std::string location( const DataObject * pObj )
      {
        return ( !pObj ? "Null DataObject !" :
                 (pObj->registry() ? pObj->registry()->identifier() : "UnRegistered") );
      }

    }
  }
}     

#endif // EVENT_PACKERUTILS_H
