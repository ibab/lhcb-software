To add new strippings to StrippingArchive:

1) Copy directly in svn from the TAGGED StrippingSelections
   into the TRUNK StrippingArchive

   This creates a new directory holding all the line builders from
   the tagged selections

   svn cp ...tags/Phys/StrippingSelections/vXrY/python/StrippingSelections
          ...trunk/Phys/StrippingArchive/python/StrippingArchive/StrippingXX

1.b) Modify the file python/StrippingArchive/StrippingXX/__init__.py. The first
     lines of the file should look like

     #########################################################################
     ## Define the version of the archive
     strippingVersion = 'Stripping23'
     _strippingModules = {}
     WGs = []
     wgDirs = os.listdir(os.environ['STRIPPINGARCHIVEROOT']+'/python/StrippingArchive/'+strippingVersion)
     for dirs in [ dir for dir in wgDirs if 'Stripping' in dir ]:
       WGs.append(dirs[9:])

     for WG in WGs:
       _tmpModule = __import__('StrippingArchive.'+strippingVersion+'.Stripping'+WG,fromlist=['_strippingModules'])
       _strippingModules[WG] = _tmpModule._strippingModules
     ###########################################################################

     Take the __init__.py file for Stripping23 as example

2) Add the new builders module to StrippingArchive/__init__.py

   ...
   import Stripping17
   import StrippingXX

3) If there are NO new builders, then the stripping must be a
   copy of previous builders. In that case add the relation
   into the dictionary: INSTEAD

   _duplicate_strippings={ ... "StrippingXX"  : "Stripping13"}

4) Add the description of when/why/what this stripping was in
   the right entry in the dictionary:

   _stripping_help={ ... "StrippingXX" : "An example made in 2010"}

5) svn commit/update and run the tests.
   The tests will fail, but the differences should only be that
   the entire StrippingXX is now checked it can be instantiated
   update the .ref file accordingly

