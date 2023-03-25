#ifndef CLOCK_SETTINGS_H
#define CLOCK_SETTINGS_H

#include <vector>
#include <string>
#include <FS.h>
#include <ArduinoJson.h>
#include "pins.h"

/**
 * Write the value of a setting.  If the setting doesn't already exist, it will be created.
*/
void set_setting(const char *screen_name, const char *setting_name, const char *new_value);

/**
 * Get the value of a setting.  If the setting doesn't exist, a default value will be returned
*/
std::string get_setting(const char *screen_name, const char *setting_name, const char *default_value);

/**
 * Register a callback function to be called when a function changes
*/
int register_setting_callback(const char *screen_name, const char *setting_name, void (*callback)(const char*));

/**
 * Deregister a previously registered callback
 * @param callback_id The id of the callback returned by `register_callback`
*/
void remove_setting_callback(int callback_id);

/**
 * Returns a list of the names of each setting for a given screen
*/
std::vector<const char*> get_settings(const char *screen_name);

/**
 * Writes every setting into a JSON document
 * @param doc an existing JSON document to write settings to
*/
void serialise_settings(StaticJsonDocument<JSON_BUFFER_SIZE> &doc);

/**
 * Reads a JSON file and loads it's contents into the settings subsystem
*/
void deserialise_settings(StaticJsonDocument<JSON_BUFFER_SIZE> &doc);

/**
 * Loads settings from a JSON file on a file system
 * @param fs the file system to load from
 * @param filename the name of the settings json file
*/
void load_settings(FS &fs, const char *filename=SETTINGS_FILE);

/**
 * Stores settings to a JSON file on a file system
 * @param fs the file system to store to
 * @param filename the name of the settings json file
*/
void store_settings(FS &fs, const char *filename=SETTINGS_FILE);




#endif /* CLOCK_SETTINGS_H */