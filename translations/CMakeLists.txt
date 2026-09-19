# unmaintained translations, not installed
#    vmpk_nl.ts
#    vmpk_tr.ts

set( TRANSLATIONS_FILES
     vmpk_cs.ts
     vmpk_de.ts
     vmpk_es.ts
     vmpk_fr.ts
     vmpk_gl.ts
     vmpk_it.ts
     vmpk_ru.ts
     vmpk_sr.ts
     vmpk_sv.ts
     vmpk_zh_CN.ts
)

if (QT_VERSION VERSION_LESS 5.15.0)
    qt5_add_translation(QM_FILES ${TRANSLATIONS_FILES})
else()
    qt_add_translation(QM_FILES ${TRANSLATIONS_FILES})
endif()

add_custom_target(translations ALL DEPENDS ${QM_FILES})

if (WIN32)
     install( FILES ${QM_FILES}
              DESTINATION translations )
elseif (UNIX AND NOT APPLE)
    install( FILES ${QM_FILES}
             DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/vmpk/locale )
endif ()

get_target_property(DRUMSTICK_LOCATION Drumstick::Widgets INTERFACE_INCLUDE_DIRECTORIES)
get_filename_component(DRUMSTICK_PREFIX ${DRUMSTICK_LOCATION} DIRECTORY)
if (WIN32)
    file(GLOB WIDGETS_TRANS "${DRUMSTICK_PREFIX}/share/translations/drumstick-widgets_*.qm")
    install(FILES ${WIDGETS_TRANS} DESTINATION translations )
elseif(INSTALL_DEPENDENCIES)
    file(GLOB WIDGETS_TRANS "${DRUMSTICK_PREFIX}/share/drumstick/drumstick-widgets_*.qm")
    install(FILES ${WIDGETS_TRANS} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/drumstick )
endif()
