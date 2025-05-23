cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW) # Support MSVC_RUNTIME_LIBRARY

project(EasyRPG_Editor VERSION 0.1.0 LANGUAGES CXX)

# Extra CMake Module files
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
include(ConfigureWindows)

# C++17 is required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source Files
set(EDITOR_SOURCES
	src/vendor/hash_table8.hpp
	src/common/dbstring.h
	src/common/filefinder.cpp
	src/common/filefinder.h
	src/common/image_loader.cpp
	src/common/image_loader.h
	src/common/lcf_widget_binding.cpp
	src/common/lcf_widget_binding.h
	src/common/scope_guard.h
	src/common/signal_blocker.h
	src/common/sortfilter_proxy_models.cpp
	src/common/sortfilter_proxy_models.h
	src/common/tileops.cpp
	src/common/tileops.h
	src/core.cpp
	src/core.h
	src/defines.h
	src/main.cpp
	src/ui/rpg_painter.cpp
	src/ui/rpg_painter.h
	src/model/actor.cpp
	src/model/actor.h
	src/model/animation.cpp
	src/model/animation.h
	src/model/attribute.cpp
	src/model/attribute.h
	src/model/battleranimation.cpp
	src/model/battleranimation.h
	src/model/chipset.cpp
	src/model/chipset.h
	src/model/class.cpp
	src/model/class.h
	src/model/commonevent.cpp
	src/model/commonevent.h
	src/model/enemy.cpp
	src/model/enemy.h
	src/model/event_command_list.cpp
	src/model/event_command_list.h
	src/model/item.cpp
	src/model/item.h
	src/model/project.cpp
	src/model/project.h
	src/model/project_data.cpp
	src/model/project_data.h
	src/model/rpg_base.cpp
	src/model/rpg_base.h
	src/model/rpg_reflect.h
	src/model/skill.cpp
	src/model/skill.h
	src/model/state.cpp
	src/model/state.h
	src/model/switch.cpp
	src/model/switch.h
	src/model/terrain.cpp
	src/model/terrain.h
	src/model/troop.cpp
	src/model/troop.h
	src/model/variable.cpp
	src/model/variable.h
	src/stringizer.cpp
	src/stringizer.h
	src/ui/commands/all_commands.h
	src/ui/commands/change_actor_face_widget.cpp
	src/ui/commands/change_actor_face_widget.h
	src/ui/commands/change_actor_face_widget.ui
	src/ui/commands/change_actor_graphic_widget.cpp
	src/ui/commands/change_actor_graphic_widget.h
	src/ui/commands/change_actor_graphic_widget.ui
	src/ui/commands/change_actor_name_widget.cpp
	src/ui/commands/change_actor_name_widget.h
	src/ui/commands/change_actor_name_widget.ui
	src/ui/commands/change_actor_title_widget.cpp
	src/ui/commands/change_actor_title_widget.h
	src/ui/commands/change_actor_title_widget.ui
	src/ui/commands/change_experience_widget.cpp
	src/ui/commands/change_experience_widget.h
	src/ui/commands/change_experience_widget.ui
	src/ui/commands/change_item_widget.cpp
	src/ui/commands/change_item_widget.h
	src/ui/commands/change_item_widget.ui
	src/ui/commands/change_level_widget.cpp
	src/ui/commands/change_level_widget.h
	src/ui/commands/change_level_widget.ui
	src/ui/commands/change_money_widget.cpp
	src/ui/commands/change_money_widget.h
	src/ui/commands/change_money_widget.ui
	src/ui/commands/change_party_widget.cpp
	src/ui/commands/change_party_widget.h
	src/ui/commands/change_party_widget.ui
	src/ui/commands/change_vehicle_graphic_widget.cpp
	src/ui/commands/change_vehicle_graphic_widget.h
	src/ui/commands/change_vehicle_graphic_widget.ui
	src/ui/commands/enable_disable_widget.cpp
	src/ui/commands/enable_disable_widget.h
	src/ui/commands/enable_disable_widget.ui
	src/ui/commands/face_graphics_widget.cpp
	src/ui/commands/face_graphics_widget.h
	src/ui/commands/face_graphics_widget.ui
	src/ui/commands/flash_screen_widget.cpp
	src/ui/commands/flash_screen_widget.h
	src/ui/commands/flash_screen_widget.ui
	src/ui/commands/full_heal_widget.cpp
	src/ui/commands/full_heal_widget.h
	src/ui/commands/full_heal_widget.ui
	src/ui/commands/input_number_widget.cpp
	src/ui/commands/input_number_widget.h
	src/ui/commands/input_number_widget.ui
	src/ui/commands/message_options_widget.cpp
	src/ui/commands/message_options_widget.h
	src/ui/commands/message_options_widget.ui
	src/ui/commands/shake_screen_widget.cpp
	src/ui/commands/shake_screen_widget.h
	src/ui/commands/shake_screen_widget.ui
	src/ui/commands/show_choices_widget.cpp
	src/ui/commands/show_choices_widget.h
	src/ui/commands/show_choices_widget.ui
	src/ui/commands/show_message_widget.cpp
	src/ui/commands/show_message_widget.h
	src/ui/commands/show_message_widget.ui
	src/ui/commands/switch_operations_widget.cpp
	src/ui/commands/switch_operations_widget.h
	src/ui/commands/switch_operations_widget.ui
	src/ui/commands/timer_operation_widget.cpp
	src/ui/commands/timer_operation_widget.h
	src/ui/commands/timer_operation_widget.ui
	src/ui/commands/tint_screen_widget.cpp
	src/ui/commands/tint_screen_widget.h
	src/ui/commands/tint_screen_widget.ui
	src/ui/commands/variable_operations_widget.cpp
	src/ui/commands/variable_operations_widget.h
	src/ui/commands/variable_operations_widget.ui
	src/ui/commands/weather_effects_widget.cpp
	src/ui/commands/weather_effects_widget.h
	src/ui/commands/weather_effects_widget.ui
	src/ui/common/edit_dialog.cpp
	src/ui/common/edit_dialog.h
	src/ui/common/edit_dialog.ui
	src/ui/common/encounter_delegate.cpp
	src/ui/common/encounter_delegate.h
	src/ui/common/import_image_dialog.cpp
	src/ui/common/import_image_dialog.h
	src/ui/common/import_image_dialog.ui
	src/ui/common/import_item.cpp
	src/ui/common/import_item.h
	src/ui/common/operand_widget.cpp
	src/ui/common/operand_widget.h
	src/ui/common/palette_scene.cpp
	src/ui/common/palette_scene.h
	src/ui/common/picker_scene.cpp
	src/ui/common/picker_scene.h
	src/ui/common/rpg_combobox.h
	src/ui/common/rpg_model.h
	src/ui/common/rpg_audio_lineedit.cpp
	src/ui/common/rpg_audio_lineedit.h
	src/ui/common/rpg_slider.cpp
	src/ui/common/rpg_slider.h
	src/ui/common/rpg_spinbox.cpp
	src/ui/common/rpg_spinbox.h
	src/ui/common/system_color_combobox.cpp
	src/ui/common/system_color_combobox.h
	src/ui/common/widget_as_dialog_model_wrapper.h
	src/ui/common/widget_as_dialog_wrapper.h
        src/ui/common/chipset_scene.h
        src/ui/common/chipset_scene.cpp
	src/ui/database/actor_delegate.cpp
	src/ui/database/actor_delegate.h
	src/ui/database/actor_widget.cpp
	src/ui/database/actor_widget.h
	src/ui/database/actor_widget.ui
	src/ui/database/attribute_widget.cpp
	src/ui/database/attribute_widget.h
	src/ui/database/attribute_widget.ui
	src/ui/database/battle_animation2_widget.cpp
	src/ui/database/battle_animation2_widget.h
	src/ui/database/battle_animation2_widget.ui
	src/ui/database/battle_animation_widget.cpp
	src/ui/database/battle_animation_widget.h
	src/ui/database/battle_animation_widget.ui
	src/ui/database/battle_screen_widget.cpp
	src/ui/database/battle_screen_widget.h
	src/ui/database/battle_screen_widget.ui
	src/ui/database/class_widget.cpp
	src/ui/database/class_widget.h
	src/ui/database/class_widget.ui
	src/ui/database/common_event_widget.cpp
	src/ui/database/common_event_widget.h
	src/ui/database/common_event_widget.ui
	src/ui/database/database_dialog.cpp
	src/ui/database/database_dialog.h
	src/ui/database/database_dialog.ui
	src/ui/database/database_split_widget.cpp
	src/ui/database/database_split_widget.h
	src/ui/database/database_split_widget.ui
	src/ui/database/enemy_group_widget.cpp
	src/ui/database/enemy_group_widget.h
	src/ui/database/enemy_group_widget.ui
	src/ui/database/enemy_widget.cpp
	src/ui/database/enemy_widget.h
	src/ui/database/enemy_widget.ui
	src/ui/database/item_widget.cpp
	src/ui/database/item_widget.h
	src/ui/database/item_widget.ui
	src/ui/database/skill_widget.cpp
	src/ui/database/skill_widget.h
	src/ui/database/skill_widget.ui
	src/ui/database/state_widget.cpp
	src/ui/database/state_widget.h
	src/ui/database/state_widget.ui
	src/ui/database/switch_widget.cpp
	src/ui/database/switch_widget.h
	src/ui/database/switch_widget.ui
	src/ui/database/system2_widget.cpp
	src/ui/database/system2_widget.h
	src/ui/database/system2_widget.ui
	src/ui/database/system_widget.cpp
	src/ui/database/system_widget.h
	src/ui/database/system_widget.ui
	src/ui/database/vertical_tab_bar.h
	src/ui/database/terrain_widget.cpp
	src/ui/database/terrain_widget.h
	src/ui/database/terrain_widget.ui
        src/ui/database/chipset/chipset_edit_scene.h
        src/ui/database/chipset/chipset_edit_scene.cpp
        src/ui/database/chipset/chipset_widget.cpp
        src/ui/database/chipset/chipset_widget.h
        src/ui/database/chipset/chipset_widget.ui
	src/ui/database/variable_widget.cpp
	src/ui/database/variable_widget.h
	src/ui/database/variable_widget.ui
	src/ui/database/vocabulary_widget.cpp
	src/ui/database/vocabulary_widget.h
	src/ui/database/vocabulary_widget.ui
	src/ui/event/battle_page_widget.cpp
	src/ui/event/battle_page_widget.h
	src/ui/event/battle_page_widget.ui
	src/ui/event/event_command_base_widget.cpp
	src/ui/event/event_command_base_widget.h
	src/ui/event/event_commands_widget.cpp
	src/ui/event/event_commands_widget.h
	src/ui/event/event_dialog.cpp
	src/ui/event/event_dialog.h
	src/ui/event/event_dialog.ui
	src/ui/event/event_page_widget.cpp
	src/ui/event/event_page_widget.h
	src/ui/event/event_page_widget.ui
	src/ui/event/event_raw_widget.cpp
	src/ui/event/event_raw_widget.h
	src/ui/event/event_raw_widget.ui
	src/ui/main_window.cpp
	src/ui/main_window.h
	src/ui/main_window.ui
	src/ui/map/map_scene.cpp
	src/ui/map/map_scene.h
	src/ui/map/undo_draw.cpp
	src/ui/map/undo_draw.h
	src/ui/map/undo_event.cpp
	src/ui/map/undo_event.h
	src/ui/maptree/map_properties_dialog.cpp
	src/ui/maptree/map_properties_dialog.h
	src/ui/maptree/map_properties_dialog.ui
	src/ui/other/import_project_dialog.cpp
	src/ui/other/import_project_dialog.h
	src/ui/other/import_project_dialog.ui
	src/ui/other/new_project_dialog.cpp
	src/ui/other/new_project_dialog.h
	src/ui/other/new_project_dialog.ui
	src/ui/other/open_project_dialog.cpp
	src/ui/other/open_project_dialog.h
	src/ui/other/open_project_dialog.ui
	src/ui/other/resource_manager_dialog.cpp
	src/ui/other/resource_manager_dialog.h
	src/ui/other/resource_manager_dialog.ui
	src/ui/other/rtp_path_dialog.cpp
	src/ui/other/rtp_path_dialog.h
	src/ui/other/rtp_path_dialog.ui
	src/ui/other/run_game_dialog.cpp
	src/ui/other/run_game_dialog.h
	src/ui/other/run_game_dialog.ui
	src/ui/other/search_dialog.cpp
	src/ui/other/search_dialog.h
	src/ui/other/search_dialog.ui
	src/ui/other/volumebutton.cpp
	src/ui/other/volumebutton.h
	src/ui/picker/picker_audio_widget.cpp
	src/ui/picker/picker_audio_widget.h
	src/ui/picker/picker_audio_widget.ui
	src/ui/picker/picker_child_widget.cpp
	src/ui/picker/picker_child_widget.h
	src/ui/picker/picker_charset_widget.cpp
	src/ui/picker/picker_charset_widget.h
	src/ui/picker/picker_charset_widget.ui
	src/ui/picker/picker_faceset_widget.cpp
	src/ui/picker/picker_faceset_widget.h
	src/ui/picker/picker_image_widget.cpp
	src/ui/picker/picker_image_widget.h
	src/ui/picker/picker_dialog.cpp
	src/ui/picker/picker_dialog.h
	src/ui/picker/picker_dialog.ui
	src/ui/viewer/battle_animation_graphics_item.cpp
	src/ui/viewer/battle_animation_graphics_item.h
	src/ui/viewer/charset_graphics_item.cpp
	src/ui/viewer/charset_graphics_item.h
	src/ui/viewer/faceset_graphics_item.cpp
	src/ui/viewer/faceset_graphics_item.h
	src/ui/viewer/rpg_graphics_view.cpp
	src/ui/viewer/rpg_graphics_view.h
	src/ui/viewer/stat_curve_graphics_item.cpp
	src/ui/viewer/stat_curve_graphics_item.h
	src/ui/database/chipset/chipset_edit_mode.h
)

if(APPLE)
	# Do not code sign
	set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
	set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
	set(CMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "org.easyrpg.editor")

	# App Icon
	set(EDITOR_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/app/Editor.icns")
	set_source_files_properties(${EDITOR_BUNDLE_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

	set(EXE_NAME "EasyRPG-Editor.app")
	set(EXE_OUTPUT_NAME "EasyRPG Editor")
	add_executable(${EXE_NAME} MACOSX_BUNDLE "src/main.cpp" ${EDITOR_SOURCES} ${EDITOR_BUNDLE_ICON})
	set_target_properties(${EXE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/resources/app/Info.plist.in")
	set_target_properties(${EXE_NAME} PROPERTIES OUTPUT_NAME ${EXE_OUTPUT_NAME})
else()
	# Executable
	set(EXE_NAME ${PROJECT_NAME})
        add_executable(${EXE_NAME} WIN32 "src/main.cpp" ${EDITOR_SOURCES})
	if(WIN32)
		# Open console for Debug builds
		if(CMAKE_BUILD_TYPE MATCHES "Debug")
			set_target_properties(${EXE_NAME} PROPERTIES WIN32_EXECUTABLE FALSE)
		endif()
		set_target_properties(${EXE_NAME} PROPERTIES OUTPUT_NAME "Editor")
		# Add icon
		target_sources(${EXE_NAME} PRIVATE "resources/Resources.rc")
	else()
		set_target_properties(${EXE_NAME} PROPERTIES OUTPUT_NAME "easyrpg-editor")
	endif()
endif()

# Include directories
target_include_directories(${EXE_NAME}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/src
	INTERFACE
		$<BUILD_INTERFACE:${AUTOGEN_INCLUDE_DIR}>
)

# IDE source grouping
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Editor" FILES ${EDITOR_SOURCES})

# Add autogen include dirs to include path, otherwise the includes are not found
# when used in the header (via https://stackoverflow.com/q/44746308/)
get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_isMultiConfig)
	set(AUTOGEN_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_autogen/include_$<CONFIG>)
else()
	set(AUTOGEN_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_autogen/include)
endif()

# Qt Magic
set_target_properties(${EXE_NAME} PROPERTIES
	AUTOMOC ON
	AUTOUIC ON
	AUTORCC ON # Enable resource compiler for exe
)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Dependencies
find_package(Qt6 CONFIG REQUIRED COMPONENTS Widgets Core Multimedia Gui Svg)
find_package(Qt6LinguistTools CONFIG QUIET)
find_package(ZLIB REQUIRED)
# liblcf
option(EDITOR_BUILD_LIBLCF "Instead of detecting liblcf the liblcf repository is cloned into lib/liblcf and built together with the Player. This is convenient for development" OFF)
set(EDITOR_BUILD_LIBLCF_GIT "https://github.com/EasyRPG/liblcf.git" CACHE STRING "Git repository of liblcf to clone when building liblcf. Requires EDITOR_BUILD_LIBLCF=ON.")

if(EDITOR_BUILD_LIBLCF)
	# liblcf is built as part of this cmake file
	set(LIBLCF_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/liblcf")
	if(NOT EXISTS ${LIBLCF_PATH})
		find_package(Git REQUIRED)
		execute_process(COMMAND ${GIT_EXECUTABLE} clone "--depth=1"
			"${EDITOR_BUILD_LIBLCF_GIT}"
			"${LIBLCF_PATH}")
	endif()
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/liblcf/builds/cmake/Modules")

	option(BUILD_SHARED_LIBS "Not used. Supresses CMP0077 in liblcf" ON)

	# Always build static liblcf and do not install it
	function(add_liblcf)
		set(BUILD_SHARED_LIBS OFF)
		set(LIBLCF_ENABLE_INSTALL OFF)
		add_subdirectory(${LIBLCF_PATH})
	endfunction()
	add_liblcf()
	target_link_libraries(${EXE_NAME} lcf)
else()
	# Use system package
	find_package(liblcf REQUIRED)
	target_link_libraries(${EXE_NAME} liblcf::liblcf)
endif()

# Libraries
target_link_libraries(${EXE_NAME}
	ZLIB::ZLIB
        Qt6::Widgets
        Qt6::Gui
        Qt6::Core
        Qt6::Multimedia
        Qt6::Svg
)

# Add resources
target_sources(${EXE_NAME} PRIVATE "resources/Resources.qrc")

# Translations
if(TARGET Qt6::lrelease)
	set(TRANSLATION_SOURCES
		translations/editor_de.ts
	)
	set(TRANSLATION_RES ${CMAKE_CURRENT_BINARY_DIR}/translations/editor.qrc)
	configure_file(translations/editor.qrc ${TRANSLATION_RES} COPYONLY)
	set_source_files_properties(${TRANSLATION_SOURCES} PROPERTIES
		OUTPUT_LOCATION "translations")
        qt6_add_translation(EDITOR_TRANSLATIONS ${TRANSLATION_SOURCES}
		OPTIONS -compress -nounfinished -removeidentical)
	target_sources(${EXE_NAME} PRIVATE ${TRANSLATION_RES} ${EDITOR_TRANSLATIONS})
	add_custom_target(translations
		COMMAND ${Qt6_LUPDATE_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/src -ts ${TRANSLATION_SOURCES}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Create/Update translations..."
		COMMAND_EXPAND_LISTS VERBATIM)
else()
	message(STATUS "Disabling translation support")
endif()

# Installation
include(GNUInstallDirs)
if(APPLE)
	# Find Qt deployment apps
        get_target_property(_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
	get_filename_component(_QT_BIN_DIR "${_QMAKE_EXECUTABLE}" DIRECTORY)
	find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_QT_BIN_DIR}")

	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
	install(TARGETS ${EXE_NAME} RUNTIME DESTINATION BUNDLE DESTINATION "${CMAKE_BINARY_DIR}/Package")
	# Run macdeployqt
	install(CODE "
		execute_process(COMMAND
			${MACDEPLOYQT_EXECUTABLE}
			\"${CMAKE_BINARY_DIR}/Package/${EXE_OUTPUT_NAME}.app\"
			-always-overwrite
		)
	")
	# Run cmake fixup_bundle to fix runpath for any non-Qt dylibs and to verify
	install(CODE "
		include(BundleUtilities)
		set(BU_CHMOD_BUNDLE_ITEMS TRUE)
		fixup_bundle(\"${CMAKE_BINARY_DIR}/Package/${EXE_OUTPUT_NAME}.app\" \"\" \"\")
	" COMPONENT RUNTIME)
else()
	install(TARGETS ${EXE_NAME}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
	if(UNIX)
		# Install desktop entry and icon
		install(FILES resources/unix/easyrpg-editor.desktop
			DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
		# legacy path, remove 2030 :)
		install(FILES resources/app/easyrpg.png
			DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
			RENAME easyrpg-editor.png)
		# FIXME: use scalable icon
		install(FILES resources/app/easyrpg.png
			DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps
			RENAME easyrpg-editor.png)
		# FIXME: Fill appdata file
		#install(FILES resources/unix/easyrpg-editor.appdata.xml
		#	DESTINATION ${CMAKE_INSTALL_DATADIR}/appdata)
	endif()
endif()
