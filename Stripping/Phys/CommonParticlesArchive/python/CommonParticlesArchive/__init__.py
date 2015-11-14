
from Gaudi.Configuration import *

class CommonParticlesArchiveConf ( object ) :
    
    def isdir( self, z, name ):
        return any(x.startswith("%s/" % name.rstrip("/")) for x in z.namelist())

    def isValidArchive(self,path):

        zipname = "python.zip"

        if not zipname in path :

            # Standard file system
            import os
            return os.path.exists(path)

        else :

            # Python zip file. So manually handle this ..
            import zipfile
            splits = path.split(zipname+"/")
            zfile = splits[0] + zipname
            entry = splits[1]
            z = zipfile.ZipFile( zfile, 'r' )
            status = self.isdir(z,entry)
            z.close()
            return status

    def redirect(self,stripping):

        import os, sys

        # Handle 'alias' strippings
        # Needs to be in sync with the dict in StrippingArchive.
        # To Do - Find a way to have only one version of this dict.
        duplicate_strippings = { "Stripping20r1p1" : "Stripping20r0p1",
                                 "Stripping20r1p2" : "Stripping20r0p2",
                                 "Stripping20r1p3" : "Stripping20r0p3",
				                         "Stripping21r1"   : "Stripping21",
                                 "Stripping21r1p1" : "Stripping21r0p1"
                                 }
        stripName = stripping.capitalize()
        if stripName in duplicate_strippings.keys() :
            stripName = duplicate_strippings[stripName]

        # Check to see if CommonParticles has already been loaded.
        # If it has, throw an exception, as this method must be called
        # prior to any import of CommonParticles
        if 'CommonParticles' in sys.modules.keys() :
            raise Exception( "Module " + str(sys.modules['CommonParticles']) +
                             " already loaded. " +
                             "redirectCommonParticles must be called *BEFORE* any imports from CommonParticles" )

        # Construct the archive python path from this modules path
        pth = os.path.sep.join( __file__.split(os.path.sep)[0:-1] + [stripName] )

        # Check this is a known archived version
        if not self.isValidArchive(pth) :
            raise Exception( "CommonParticles archive '"+pth+"' is missing" )

        # Append the archive path, so it is picked up first
        log.info( "Redirecting CommonParticles to '" + pth + "'" )
        sys.path.insert(0,pth)
