#ifndef WRITERNETOBJECT_H
#define WRITERNETOBJECT_H

#include "Writer/writernetfileobject.h"
#include <string>
#include <map>


typedef std::map<std::string, WriterNetFileObject> FilesContainer;
typedef FilesContainer::iterator FilesContainerIterator;

/** \brief Represents a writer process. Manages all files belonging to this process. */
class WriterNetObject {
    private:
        int writerPID;
        unsigned int fileCount;
    public:
        FilesContainer* files;
        
        /** Returns the process id of the writer */
        int getPID();
        
        /** Constructor: needs the process id of the writer and
         * initializes an empty list for the files belonging
         * to this writeri. */
        WriterNetObject(int writerPID);

        /** Copy constructor, used in for container operations */
        WriterNetObject(const WriterNetObject&);
        
        /** Adds a file to this writer */
        void addFile(const char* fn);
        
        /** Searches if the file belongs to this writer and if so
         * removes it from the list. */
        void removeFile(const char* fn);
        
        /** Adds a key/value pair for a file.*/
        void addKeyValuePair(const char* fn, const char* key, const char* value);

        /** Prints its files and delegates to WriterNetFileObject */
        void debug(FILE* fp);

        /** operator== for comparison in container **/
        bool operator==(const WriterNetObject&) const;
        
        /** operator!= for comparison in container **/
        bool operator!=(const WriterNetObject&) const;
        
        /** operator< for comparison in container **/
        bool operator<(const WriterNetObject&) const;
        
        /** operator> for comparison in container **/
        bool operator>(const WriterNetObject&) const;

        /** Search a file, if found, return it, otherwise retun
         *  NULL
         */
        WriterNetFileObject* getFile(const char* fn);

        /** Destructor: deletes all files belonging to this writer */
        virtual ~WriterNetObject();
};
#endif

