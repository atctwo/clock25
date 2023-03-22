#ifndef CLOCK_SETTINGS_H
#define CLOCK_SETTINGS_H

#include <vector>
#include <string>

/**
 * Write the value of a setting.  If the setting doesn't already exist, it will be created.
*/
void set_setting(const char *screen_name, const char *setting_name, const char *new_value);

/**
 * Get the value of a setting.  If the setting doesn't exist, a default value will be returned
*/
std::string get_setting(const char *screen_name, const char *setting_name, const char *default_value);

/**
 * Returns a list of the names of each setting for a given screen
*/
std::vector<const char*> get_settings(const char *screen_name);


#endif /* CLOCK_SETTINGS_H */