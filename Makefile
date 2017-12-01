
################################################################################
## ESP32
ifndef IDF_PATH
	$(error If building for ESP32, you must supply the IDF_PATH variable.)
endif

PROJECT_NAME         := creature-light

# Pull in the esp-idf...
include $(IDF_PATH)/make/project.mk
