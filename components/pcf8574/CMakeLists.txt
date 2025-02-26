set(GIT_DIR_LOOKUP_POLICY ALLOW_LOOKING_ABOVE_CMAKE_SOURCE_DIR)
idf_build_get_property(target IDF_TARGET)

idf_component_register(
    SRCS "PCF8574_library/PCF8574.cpp"
    INCLUDE_DIRS PCF8574_library
    REQUIRES arduino
)
