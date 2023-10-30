# bsopack/Makefile
#
# This file is part of bsopack, part of the Husky fidonet software project
# Use with GNU make v.3.82 or later
# Requires: husky enviroment
#

bsopack_LIBS := $(fidoconf_TARGET_BLD) $(smapi_TARGET_BLD) $(huskylib_TARGET_BLD)

bsopack_CDEFS := $(CDEFS) -I$(fidoconf_ROOTDIR) \
                        -I$(smapi_ROOTDIR) \
                        -I$(huskylib_ROOTDIR) \
                        -I$(bsopack_ROOTDIR)$(bsopack_H_DIR)

bsopack_TARGET     = bsopack$(_EXE)
bsopack_TARGET_BLD = $(bsopack_BUILDDIR)$(bsopack_TARGET)
bsopack_TARGET_DST = $(BINDIR_DST)$(bsopack_TARGET)

ifdef MAN1DIR
    bsopack_MAN1PAGES := bsopack.1
    bsopack_MAN1BLD := $(bsopack_BUILDDIR)$(bsopack_MAN1PAGES)$(_COMPR)
    bsopack_MAN1DST := $(DESTDIR)$(MAN1DIR)$(DIRSEP)$(bsopack_MAN1PAGES)$(_COMPR)
endif


.PHONY: bsopack_build bsopack_install bsopack_uninstall bsopack_clean bsopack_distclean \
        bsopack_depend bsopack_doc bsopack_doc_install bsopack_doc_uninstall \
        bsopack_doc_clean bsopack_doc_distclean bsopack_rmdir_DEP bsopack_rm_DEPS \
        bsopack_clean_OBJ bsopack_main_distclean

bsopack_build: $(bsopack_TARGET_BLD) $(bsopack_MAN1BLD) bsopack_doc

ifneq ($(MAKECMDGOALS), depend)
    include $(bsopack_DOCDIR)Makefile
    ifneq ($(MAKECMDGOALS), distclean)
        ifneq ($(MAKECMDGOALS), uninstall)
            include $(bsopack_DEPS)
        endif
    endif
endif


# Build application
$(bsopack_TARGET_BLD): $(bsopack_ALL_OBJS) $(bsopack_LIBS) | do_not_run_make_as_root
	$(CC) $(LFLAGS) $(EXENAMEFLAG) $@ $^ $(bsopack_LIBZ)

# Compile .c files
$(bsopack_ALL_OBJS): $(bsopack_OBJDIR)%$(_OBJ): $(bsopack_SRCDIR)%.c | $(bsopack_OBJDIR)
	$(CC) $(bsopack_CFLAGS) $(bsopack_CDEFS) -o $(bsopack_OBJDIR)$*$(_OBJ) $(bsopack_SRCDIR)$*.c

$(bsopack_OBJDIR): | $(bsopack_BUILDDIR) do_not_run_make_as_root
	[ -d $@ ] || $(MKDIR) $(MKDIROPT) $@


# Build man pages
ifdef MAN1DIR
    $(bsopack_MAN1BLD): $(bsopack_MANDIR)$(bsopack_MAN1PAGES)
	@[ $$($(ID) $(IDOPT)) -eq 0 ] && echo "DO NOT run \`make\` from root" && exit 1 || true
    ifdef COMPRESS
		$(COMPRESS) -c $< > $@
    else
		$(CP) $(CPOPT) $< $@
    endif

else
    $(bsopack_MAN1BLD): ;
endif


# Install
ifneq ($(MAKECMDGOALS), install)
    bsopack_install: ;
else
    bsopack_install: $(bsopack_TARGET_DST) bsopack_install_man bsopack_doc_install ;
endif

$(bsopack_TARGET_DST): $(bsopack_TARGET_BLD) | $(DESTDIR)$(BINDIR)
	$(INSTALL) $(IBOPT) $< $(DESTDIR)$(BINDIR); \
	$(TOUCH) "$@"

ifndef MAN1DIR
    bsopack_install_man: ;
else
    bsopack_install_man: $(bsopack_MAN1DST)

    $(bsopack_MAN1DST): $(bsopack_MAN1BLD) | $(DESTDIR)$(MAN1DIR)
	$(INSTALL) $(IMOPT) $< $(DESTDIR)$(MAN1DIR); $(TOUCH) "$@"
endif


# Clean
bsopack_clean: bsopack_clean_OBJ bsopack_doc_clean
	-[ -d "$(bsopack_OBJDIR)" ] && $(RMDIR) $(bsopack_OBJDIR) || true

bsopack_clean_OBJ:
	-$(RM) $(RMOPT) $(bsopack_OBJDIR)*

# Distclean
bsopack_distclean: bsopack_doc_distclean bsopack_main_distclean bsopack_rmdir_DEP
	-[ -d "$(bsopack_BUILDDIR)" ] && $(RMDIR) $(bsopack_BUILDDIR) || true

bsopack_rmdir_DEP: bsopack_rm_DEPS
	-[ -d "$(bsopack_DEPDIR)" ] && $(RMDIR) $(bsopack_DEPDIR) || true

bsopack_rm_DEPS:
	-$(RM) $(RMOPT) $(bsopack_DEPDIR)*

bsopack_main_distclean: bsopack_clean
	-$(RM) $(RMOPT) $(bsopack_TARGET_BLD)
ifdef MAN1DIR
	-$(RM) $(RMOPT) $(bsopack_MAN1BLD)
endif


# Uninstall
bsopack_uninstall: bsopack_doc_uninstall
	-$(RM) $(RMOPT) $(bsopack_TARGET_DST)
ifdef MAN1DIR
	-$(RM) $(RMOPT) $(bsopack_MAN1DST)
endif
