idf_build_get_property(target IDF_TARGET)


set(srcs "src/led_effects_driver.cpp" "src/led_effects_driver_priv.hpp" "src/png_loader.cpp")
set(public_include "include")

set(priv_requires led_strip libpng)


idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES ${priv_requires}
                       EMBED_FILES "png/testpattern.png" "png/wave.png" "png/love.png" "png/connected.png" "png/disconnected.png" "png/pairing.png" "png/startup.png" "png/settingsmenu.png" "png/love_sending.png"
                      )