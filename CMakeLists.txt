idf_build_get_property(target IDF_TARGET)

set(srcs "src/led_effects_driver.cpp" "src/led_effects_queue.cpp" "src/led_effects_driver_priv.hpp")
set(public_include "include")

set(priv_requires led_strip)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES ${priv_requires}
                      )
