file(GLOB_RECURSE srcs "main.c" "src/*.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "./" "./include"
                       REQUIRES bt nvs_flash driver esp_wifi)
