# unmaintained translations, not installed
# vmpk_tr.ts

SET( TRANSLATIONS_FILES
     vmpk_cs.ts
     vmpk_de.ts
     vmpk_es.ts
     vmpk_fr.ts
#    vmpk_nl.ts
     vmpk_ru.ts
     vmpk_sv.ts
#    vmpk_zh_CN.ts
)

QT4_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
ADD_CUSTOM_TARGET(translations ALL DEPENDS ${QM_FILES})

IF (UNIX AND NOT APPLE)
    INSTALL( FILES ${QM_FILES}
             DESTINATION share/vmpk/locale )
ELSE (UNIX AND NOT APPLE)
    INSTALL( FILES ${QM_FILES}
             DESTINATION . )
ENDIF (UNIX AND NOT APPLE)
