# Ensure the module is correctly registered before linking
if(TARGET modules)
    target_link_libraries(modules PRIVATE curl)
    
    # Explicitly include nlohmann-json path
    target_include_directories(modules PRIVATE /usr/local/include /usr/local/include/nlohmann)
endif()