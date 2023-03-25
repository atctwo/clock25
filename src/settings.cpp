#include "settings.h"
#include "screens/screens.h"  // cyclic dependancy
#include "log.h"

#include <map>
#include <string>
#include <vector>

#define LOG_TAG "settings"

// map to hold settings
std::map< const char*, std::map< const char*, const char* > > settings;

// struct for callback parameters
struct callback_params {
    const char *screen_name;
    const char *setting_name;
    void (*callback)(const char*);
};

// map to store callbacks
// I used a map so callbacks could have a numeric ID and also be removed without the indexes
// of all the other callbacks changing
std::map<int, callback_params> callbacks;
int next_callback_index = 0;

void set_setting(const char *screen_name, const char *setting_name, const char *new_value)
{
    // if screen name is passed as nullptr, use the current screen name
    const char *screen_name_2 = (screen_name == nullptr) ? current_screen_name() : screen_name;

    logi(LOG_TAG, "setting setting %s:%s = %s", screen_name_2, setting_name, new_value);

    // set the setting
    settings[screen_name_2][setting_name] = new_value;

    // call setting reload method of current screen
    // if specified screen is the current screen
    if (strcmp(screen_name_2, current_screen_name()) == 0)
    {
        // tell the current screen about the update
        current_screen->setting_update(setting_name, new_value);
    }

    // call any callbacks registered for the setting
    for (const std::pair<int, callback_params> param : callbacks) {

        // if screen name and setting name match
        if (strcmp(param.second.screen_name, screen_name_2) == 0) {
            if (strcmp(param.second.setting_name, setting_name) == 0) {

                // if callback isn't null
                if (param.second.callback) param.second.callback(new_value);
                else loge(LOG_TAG, "callback is null");

            }
        }

    }

}

std::string get_setting(const char *screen_name, const char *setting_name, const char *default_value)
{
    // if screen name is passed as nullptr, use the current screen name
    const char *screen_name_2 = (screen_name == nullptr) ? current_screen_name() : screen_name;

    logi(LOG_TAG, "getting setting %s:%s, default %s", screen_name_2, setting_name, default_value);

    // if there are no settings for screen name, return default value
    if      (!settings.count(screen_name_2))               return default_value;

    // if there are settings for screen name, but none of the specified setting name, return default value
    else if (!settings[screen_name_2].count(setting_name)) return default_value;

    // otherwise, return the setting value
    else return settings[screen_name_2][setting_name];
}

int register_setting_callback(const char *screen_name, const char *setting_name, void (*callback)(const char*))
{
    // if screen name is passed as nullptr, use the current screen name
    const char *screen_name_2 = (screen_name == nullptr) ? current_screen_name() : screen_name;

    logi(LOG_TAG, "registering callback for %d:%d (id=%d)", screen_name_2, setting_name, next_callback_index);

    callback_params params = {screen_name_2, setting_name, callback};
    callbacks[next_callback_index] = params;
    
    return next_callback_index++;
}

void remove_setting_callback(int callback_id)
{
    logi(LOG_TAG, "deregistering callback %d", callback_id);
    callbacks.erase(callback_id);
}

std::vector<const char*> get_settings(const char *screen_name)
{
    std::vector<const char*> setting_names;

    // for each setting of the screen
    for (auto setting : settings[screen_name])
    {
        // add key name to vector
        setting_names.push_back(setting.first);
    }

    return setting_names;
}