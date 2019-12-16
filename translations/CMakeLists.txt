# unmaintained translations, not installed
#    vmpk_nl.ts
#    vmpk_tr.ts
#    vmpk_zh_CN.ts

set( TRANSLATIONS_FILES
     vmpk_cs.ts
     vmpk_de.ts
     vmpk_es.ts
     vmpk_fr.ts
     vmpk_gl.ts
     vmpk_ru.ts
     vmpk_sr.ts
     vmpk_sv.ts
)

qt5_add_translation(QM_FILES ${TRANSLATIONS_FILES})
add_custom_target(translations ALL DEPENDS ${QM_FILES})

if (UNIX AND NOT APPLE)
    install( FILES ${QM_FILES}
             DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/vmpk/locale )
endif ()

if (WIN32)
    install( FILES ${QM_FILES}
             DESTINATION . )
endif ()
