################################################################################
# Package: BenderTools
################################################################################
gaudi_subdir(BenderTools v26r1)

gaudi_depends_on_subdirs(Phys/Bender
                         Phys/PhysConf)

gaudi_install_python_modules()
gaudi_install_scripts()

gaudi_alias(db_tags
            get_dbtags.py)
gaudi_alias(get_metainfo
            get_metainfo.py)
gaudi_alias(dst_dump
            dst-dump.py)
gaudi_alias(dump_dst
            dst-dump.py)
gaudi_alias(get_db_tags
            get_dbtags.py)
gaudi_alias(get_dbtags
            get_dbtags.py)
gaudi_alias(check_trg
            trg_check)
gaudi_alias(get_meta_info
            get_metainfo.py)
gaudi_alias(dst_Explorer
            dst_explorer)
gaudi_alias(no_mc_decay
            no_mc_decays.py)
gaudi_alias(dst_content
            dst-dump.py)
gaudi_alias(dstExplorer
            dst_explorer)
gaudi_alias(no_mc_decays
            no_mc_decays.py)
gaudi_alias(Dst_Explorer
            dst_explorer)

