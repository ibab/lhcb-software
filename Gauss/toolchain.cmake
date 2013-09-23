# Special toolchain file that inherits the same heptools version as the
# used projects.
find_file(inherit_heptools_module InheritHEPTools.cmake)

macro(_generator name version)
  set(${name}_config_version ${version} CACHE STRING "Version of ${name}")
  mark_as_advanced(${name}_config_version)
  set(${name}_native_version ${${name}_config_version})
  if(NOT ${name}_directory_name)
    set(${name}_directory_name ${name})
  endif()
  list(INSERT CMAKE_PREFIX_PATH 0 ${LCG_external}/MCGenerators_lcgcmt${heptools_version}/${${name}_directory_name}/${${name}_native_version}/${LCG_system})
endmacro()

# this check is needed because the toolchain seem to be called a second time
# without the proper cache
if(inherit_heptools_module)
  include(${inherit_heptools_module})
  inherit_heptools()

  set(powhegbox_directory_name powheg-box)

  # FIXME: generator paths setting must be cleaned up and moved to another file
  _generator(alpgen 2.1.4)
  _generator(lhapdf 5.8.8)
  _generator(herwigpp 2.6.1b)
  _generator(thepeg 1.8.1)
  _generator(powhegbox r2092)
  _generator(photos++ 3.52)
  _generator(tauola++ 1.1.1a)
  _generator(pythia8 175)
  _generator(pythia6 427.2)
  _generator(rivet 1.8.3)
  _generator(hijing 1.383bs.2)

endif()
