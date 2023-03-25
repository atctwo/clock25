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

template <typename T>
const char *match_key(std::map<const char*, T> m, const char *target_key)
{
    /*
        tl;dr this function returns the pointer to a string that can be used as a key,
        based on the target key that's passed to it.

        Most of the code in this program originally accessed the settings map using string literals 
        or variables populated from literals as keys. Even though they are different
        string variables, ultimately they all point to the same string in the data section
        of the compiled program.  They all point to the same place.

        When ArduinoJson loads a JSON file from the SD card, it copies it's contents into
        some other buffer in RAM.  When you try to access a setting name as the name of an
        object key, it points to it's own location in memory.

        std::map<const char*, <any>> appears to hash the string's address, rather than it's 
        contents, as the key.  This means that when using a string literal (or variable which
        has been assigned to the string literal) as the key, it's actually hashing a different
        memory location than it would when using the key of a JsonObject.  Even if they have 
        the same contents!

        This can result in (sort of) duplicate keys in the map.  While the keys aren't
        technically duplicates, it looks like they are if you print the map's contents.  They
        also look identical when using them as keys of a JsonObject (so, different map keys
        result in the same JsonObject key).

        My workaround for this is instead of using the JsonObject's key as a key of the map
        directly, first to a linear search to find the matching (already existing) map key,
        and use that instead.  If such a key doesn't exist in the map, then it's ok to go
        ahead and use the JSON key to create it.

        This does mean that a similar "key lookup" will have to be used to access the map
        every single time that isn't via string literal.

        That's what this function does.  It iterates over all keys in a map, and tries to
        find the key that matches the contents of the target key that's passed to it.  If there
        is no match, it creates a new key and returns a pointer to the key's name.
    */

    // check each key in the map
    for (auto it = m.begin(); it != m.end(); ++it)
    {
        // if the target key is actually the same pointer as the key
        if (it->first == target_key) return it->first;

        // if the key has the same contents as the target
        if (strcmp(it->first, target_key) == 0)
        {
            // return the key (a pointer to a string with the key's name)
            return it->first;
        }
    }

    // if we reach this point, the target has no matching key in the map
    // so make one!
    return target_key;
}

void set_setting(const char *screen_name, const char *setting_name, const char *new_value)
{
    // if screen name is passed as nullptr, use the current screen name
    const char *screen_name_2 = (screen_name == nullptr) ? current_screen_name() : screen_name;
    logi(LOG_TAG, "setting setting %s:%s = %s", screen_name_2, setting_name, new_value);

    // match screen and setting names
    const char *matched_screen_name = match_key<std::map<const char*, const char*>>(settings, screen_name_2);
    const char *matched_setting_name = match_key<const char *>(settings[matched_screen_name], setting_name);

    // set the setting
    settings[matched_screen_name][matched_setting_name] = new_value;
    settings.find(screen_name_2)->second.find(setting_name)->second = new_value;

    // call setting reload method of current screen
    // if specified screen is the current screen
    if (strcmp(matched_screen_name, current_screen_name()) == 0)
    {
        // tell the current screen about the update
        current_screen->setting_update(matched_setting_name, new_value);
    }

    // call any callbacks registered for the setting
    for (const std::pair<int, callback_params> param : callbacks) {

        // if screen name and setting name match
        if (strcmp(param.second.screen_name, matched_screen_name) == 0) {
            if (strcmp(param.second.setting_name, matched_setting_name) == 0) {

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

    // match screen and setting names
    const char *matched_screen_name = match_key<std::map<const char*, const char*>>(settings, screen_name_2);
    const char *matched_setting_name = match_key<const char *>(settings[matched_screen_name], setting_name);

    // if there are no settings for screen name, return default value
    if      (!settings.count(matched_screen_name))                       return default_value;

    // if there are settings for screen name, but none of the specified setting name, return default value
    else if (!settings[matched_screen_name].count(matched_setting_name)) return default_value;

    // otherwise, return the setting value
    else return settings[matched_screen_name][matched_setting_name];
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

void serialise_settings(StaticJsonDocument<JSON_BUFFER_SIZE> &doc)
{
    logi(LOG_TAG, "Serialising all settings");

    // create object to store settings
    JsonObject obj_settings = doc.createNestedObject("clock25_settings");

    logi(LOG_TAG, "e %d", settings.size());

    // for each screen with settings
    for (auto screen : settings)
    {
        // create object for screen
        JsonObject obj_screen = obj_settings.createNestedObject(screen.first);

        logi(LOG_TAG, "%s (%d chars) (at 0x%x)", screen.first, strlen(screen.first), screen.first);

        // for each setting
        for (auto setting : screen.second)
        {
            logi(LOG_TAG, "\t%s = %s", setting.first, setting.second);

            // store setting
            obj_screen[setting.first] = setting.second;
        }
    }
}

void deserialise_settings(StaticJsonDocument<JSON_BUFFER_SIZE> &doc)
{
    logi(LOG_TAG, "Deserialising all settings");

    // get settings object
    JsonObject obj_settings = doc["clock25_settings"];

    // for each screen
    if (obj_settings)
    {
        // for each screen
        for (JsonPair screen : obj_settings)
        {
            // get screen name
            const char *screen_name = screen.key().c_str();
            const char *matched_screen_name = match_key<std::map<const char*, const char*>>(settings, screen_name);

            // get screen object
            JsonObject obj_screen = screen.value().as<JsonObject>();

            // for each setting
            for (JsonPair setting : obj_screen)
            {
                // get setting name and value
                const char *setting_name = setting.key().c_str();
                const char *matched_setting_name = match_key<const char *>(settings[matched_screen_name], setting_name);
                const char *setting_value = setting.value().as<const char*>();

                // write setting
                set_setting(matched_screen_name, matched_setting_name, setting_value);
            }
        }
    }
    else loge(LOG_TAG, "Deserialisation error: missing key clock25_settings");
}

void load_settings(FS &fs, const char *filename)
{
    logi(LOG_TAG, "loading settings from fs");

    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    File json = fs.open(filename);
    deserializeJson(doc, json);
    json.close();
    deserialise_settings(doc);
}

void store_settings(FS &fs, const char *filename)
{
    logi(LOG_TAG, "storing settings to fs");

    // serialise settings
    StaticJsonDocument<JSON_BUFFER_SIZE> doc2;
    serialise_settings(doc2);
    std::string output = "";
    serializeJsonPretty(doc2, output);

    // write json file to fs
    File json = fs.open(filename, "w");
    json.print(output.c_str());
    json.close();
    
}