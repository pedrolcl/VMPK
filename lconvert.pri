qtPrepareTool(LCONVERT, lconvert)
isEmpty(LCONVERT_LANGS): LCONVERT_LANGS = es en
isEmpty(LCONVERT_PATTERNS): LCONVERT_PATTERNS = qtbase qtscript qtmultimedia qtxmlpatterns
LCONVERT_OUTPUTS =
for(lang, LCONVERT_LANGS) {
    lang_files =
    for(pat, LCONVERT_PATTERNS) {
        lang_files += $$files($$[QT_INSTALL_TRANSLATIONS]/$${pat}_$${lang}.qm)
    }
    outfile = $$OUT_PWD/qt_$${lang}.qm
    system($$LCONVERT -i $$join(lang_files, ' ') -o $$outfile): LCONVERT_OUTPUTS += $$outfile
}
qm_res.files = $$LCONVERT_OUTPUTS
qm_res.base = $$OUT_PWD
qm_res.prefix = "/"
RESOURCES += qm_res
