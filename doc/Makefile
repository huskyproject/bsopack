# A makefile for bsopack documentation
# bsopack/doc/Makefile

.PHONY: bsopack_info bsopack_html bsopack_txt bsopack_dvi bsopack_pdf \
        bsopack_rmdir_DOC bsopack_clean_DOC \
        bsopack_install_info bsopack_install_html bsopack_install_txt \
        bsopack_install_dvi bsopack_install_pdf bsopack_info_uninstall bsopack_docs_uninstall

ifdef DOCDIR
    bsopack_doc: bsopack_info bsopack_html bsopack_txt bsopack_dvi bsopack_pdf ;
else
    bsopack_doc: bsopack_info ;
endif

ifdef INFODIR
    bsopack_info: $(bsopack_BUILDDIR)bsopack.info$(_COMPR)
else
    bsopack_info: ;
endif

ifeq ($(HTML), 1)
    bsopack_html: $(bsopack_BUILDDIR)bsopack.html
else
    bsopack_html: ;
endif

ifeq ($(TXT), 1)
    bsopack_txt: $(bsopack_BUILDDIR)bsopack.txt
else
    bsopack_txt: ;
endif

ifeq ($(DVI), 1)
    bsopack_dvi: $(bsopack_BUILDDIR)bsopack.dvi
else
    bsopack_dvi: ;
endif

ifeq ($(PDF), 1)
    bsopack_pdf: $(bsopack_BUILDDIR)bsopack.pdf
else
    bsopack_pdf: ;
endif

bsopack_DOCDIR_BLD=$(bsopack_BUILDDIR)doc$(DIRSEP)
bsopack_TEXI=$(bsopack_DOCDIR_BLD)bsopack.texi

ifdef COMPRESS
$(bsopack_BUILDDIR)bsopack.info$(_COMPR): $(bsopack_BUILDDIR)bsopack.info
	$(COMPRESS) -c $? > $@
endif

$(bsopack_BUILDDIR)bsopack.info: $(bsopack_TEXI)
	-$(MAKEINFO) $(MINOSPLIT) "$<" $(MIOUT) "$@"

$(bsopack_BUILDDIR)bsopack.html: $(bsopack_TEXI)
	-LC_ALL=C $(MAKEINFO) $(MIHTML) $(MINOSPLIT) "$<" $(MIOUT) "$@"

$(bsopack_BUILDDIR)bsopack.txt: $(bsopack_TEXI)
	-LC_ALL=C $(MAKEINFO) $(MIPTEXT) $(MINOSPLIT) "$<" $(MIOUT) "$@"

$(bsopack_BUILDDIR)bsopack.dvi: $(bsopack_DOCDIR_BLD)bsopack.dvi
	$(LN) $(LNHOPT) "$<" $(bsopack_BUILDDIR)

$(bsopack_DOCDIR_BLD)bsopack.dvi: $(bsopack_TEXI)
	-cd $(bsopack_DOCDIR_BLD); LC_ALL=C $(TEXI2DVI) ../../../"$<"

$(bsopack_BUILDDIR)bsopack.pdf: $(bsopack_DOCDIR_BLD)bsopack.pdf
	$(LN) $(LNHOPT) "$<" $(bsopack_BUILDDIR)

$(bsopack_DOCDIR_BLD)bsopack.pdf: $(bsopack_TEXI)
	-cd $(bsopack_DOCDIR_BLD); \
	LC_ALL=C $(MAKEINFO) $(MIPDF) $(MINOSPLIT) bsopack.texi $(MIOUT) bsopack.pdf

$(bsopack_TEXI): $(bsopack_DOCDIR)bsopack.texi | $(bsopack_DOCDIR_BLD)
	$(LN) $(LNHOPT) "$<" $(bsopack_DOCDIR_BLD)

$(bsopack_DOCDIR_BLD):
	[ -d "$(bsopack_BUILDDIR)" ] || $(MKDIR) $(MKDIROPT) "$(bsopack_BUILDDIR)"
	[ -d "$@" ] || $(MKDIR) $(MKDIROPT) "$@"


# Install
ifdef DOCDIR
# See "all_info_install" in the root Makefile
    bsopack_doc_install: all_info_install bsopack_install_html bsopack_install_txt \
                 bsopack_install_dvi bsopack_install_pdf ;
else
    bsopack_doc_install: all_info_install ;
endif

ifeq ($(HTML), 1)
    bsopack_install_html: $(DOCDIR_DST)bsopack.html ;

    $(DOCDIR_DST)bsopack.html: $(bsopack_BUILDDIR)bsopack.html | $(DOCDIR_DST)
		$(INSTALL) $(IMOPT) "$<" "$|"; \
		$(TOUCH) "$@"
else
    bsopack_install_html: ;
endif

ifeq ($(TXT), 1)
    bsopack_install_txt: $(DOCDIR_DST)bsopack.txt ;

    $(DOCDIR_DST)bsopack.txt: $(bsopack_BUILDDIR)bsopack.txt | $(DOCDIR_DST)
		$(INSTALL) $(IMOPT) "$<" "$|"; \
		$(TOUCH) "$@"
else
    bsopack_install_txt: ;
endif

ifeq ($(DVI), 1)
    bsopack_install_dvi: $(DOCDIR_DST)bsopack.dvi ;

    $(DOCDIR_DST)bsopack.dvi: $(bsopack_BUILDDIR)bsopack.dvi | $(DOCDIR_DST)
		$(INSTALL) $(IMOPT) "$<" "$|"; \
		$(TOUCH) "$@"
else
    bsopack_install_dvi: ;
endif

ifeq ($(PDF), 1)
    bsopack_install_pdf: $(DOCDIR_DST)bsopack.pdf ;

    $(DOCDIR_DST)bsopack.pdf: $(bsopack_BUILDDIR)bsopack.pdf | $(DOCDIR_DST)
		$(INSTALL) $(IMOPT) "$<" "$|"; \
		$(TOUCH) "$@"
else
    bsopack_install_pdf: ;
endif


# Clean
bsopack_doc_clean: bsopack_rmdir_DOC ;

bsopack_rmdir_DOC: bsopack_clean_DOC
	-[ -d "$(bsopack_DOCDIR_BLD)" ] && $(RMDIR) $(bsopack_DOCDIR_BLD) || true

bsopack_clean_DOC:
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.fns
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.aux
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.toc
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.cp
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.fn
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.vr
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.kr
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.ke
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.tp
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.kes
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.krs
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.ky
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.pg
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.cps
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)*.log
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.texi
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.dvi
	-$(RM) $(RMOPT) $(bsopack_DOCDIR_BLD)bsopack.pdf

# Distclean
bsopack_doc_distclean: bsopack_doc_clean
	-$(RM) $(RMOPT) $(bsopack_BUILDDIR)bsopack*dvi
	-$(RM) $(RMOPT) $(bsopack_BUILDDIR)bsopack.inf
	-$(RM) $(RMOPT) $(bsopack_BUILDDIR)bsopack*html
	-$(RM) $(RMOPT) $(bsopack_BUILDDIR)bsopack.ps
	-$(RM) $(RMOPT) $(bsopack_BUILDDIR)bsopack.doc
	-$(RM) $(RMOPT) $(bsopack_BUILDDIR)bsopack*txt
	-$(RM) $(RMOPT) $(bsopack_BUILDDIR)bsopack.pdf
	-$(RM) $(RMOPT) $(bsopack_BUILDDIR)bsopack.info*


# Uninstall
ifdef DOCDIR
    bsopack_doc_uninstall: bsopack_info_uninstall bsopack_docs_uninstall ;
else
    bsopack_doc_uninstall: bsopack_info_uninstall ;
endif

ifdef INFODIR
# See "all_info_uninstall" in the root Makefile
    bsopack_info_uninstall: all_info_uninstall
		-$(RM) $(RMOPT) $(INFODIR_DST)bsopack.info$(_COMPR)
else
    bsopack_info_uninstall: ;
endif

bsopack_docs_uninstall:
ifeq ($(HTML), 1)
	-$(RM) $(RMOPT) $(DOCDIR_DST)bsopack.html
endif
ifeq ($(TXT), 1)
	-$(RM) $(RMOPT) $(DOCDIR_DST)bsopack.txt
endif
ifeq ($(DVI), 1)
	-$(RM) $(RMOPT) $(DOCDIR_DST)bsopack.dvi
endif
ifeq ($(PDF), 1)
	-$(RM) $(RMOPT) $(DOCDIR_DST)bsopack.pdf
endif
