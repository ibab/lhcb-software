#ifndef WRITERS_H
#define WRITERS_H

#include "Writer/writernetobject.h"
#include "Writer/writernetfileobject.h"
#include <string>
#include <map>

using namespace std;

typedef map<int, WriterNetObject> AllWriters;
typedef map<int, WriterNetObject>::iterator  AllWritersIterator;

/** \brief Manages a list of all Writers on this machine. */
class Writers {
    private:
        unsigned int writerCount;
    public:
        AllWriters* allWriters; /** A list containing all writers. */
        
        /** Constructor: Initializes the empty list of writers and
         *  sets the writerCount to zero. */
        Writers();

        /** Adds a writer to the list. */
        void addWriter(int writerPID);
        
        /** Searches a writer with the given process id and returns
         *  it if found in the list. */
        WriterNetObject* getWriter(int writerPID);
        
        /** Searches a writer with the given process id and deletes
         *  it from the list if found. */
        void deleteWriter(int writerPID);
        
        /** Adds a file for the writer with the given process id */
        int addFile(int writerPID, const char* fn);

        /** Searches the writer with the given process id and trys
         *  to delete the file with the given fileid */
        void removeFile(int writerPID, const char* fn);
        
        /** Prints all Writers, their files and their properties */
        void debug(FILE*);

        /** Creates or updates a key value for a given file. */
        void addKeyValuePair(int writerPID, const char* fn, const char* key, const char* value);

        /** Destructor: Deletes all writers in the list. */
        ~Writers();
};
#endif

