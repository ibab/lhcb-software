################################################################################
# Package: GiGaVisUI
################################################################################
gaudi_subdir(GiGaVisUI v5r2)

gaudi_depends_on_subdirs(Sim/GiGa
                         Vis/VisSvc
                         Det/DetDesc)

FindG4libs(Tree RayTracer UIGAG FR VRML OpenGL vis_management UIbasic)

gaudi_add_module(GiGaVisUI
                 src/*.cpp
                 INCLUDE_DIRS Vis/VisSvc
                 LINK_LIBRARIES GiGaLib ${GEANT4_LIBS} DetDescLib)

gaudi_env(SET G4DAWNFILE_DEST_DIR ../job/
          SET G4DAWNFILE_MULTI_WINDOW 1
          SET G4DAWNFILE_PS_VIEWER gv
          SET G4DAWN_MULTI_WINDOW 1)

