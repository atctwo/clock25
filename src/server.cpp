#include "server.h"
#include "wifi.h"
#include "ntp.h"
#include "rtc.h"
#include "screens/screens.h"
#include "settings.h"
#include "utility.h"
#include "pins.h"
#include "log.h"

#include <string>
#include <SD.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>

#define LOG_TAG "server"
#define SERVER_PORT 80

WebServer server(SERVER_PORT);

void server_cb_404()
{
    String msg = "";
    msg += "<h1>404 Resource Not Found</h1>";
    msg += "<p>Sorry, there was nothing at this URL.</p>";
    server.send(404, "text/html", msg);
}

void server_cb_root()
{
    String msg = "";
    msg += "<h1>clock25</h1>";
    msg += "<p>This web interface is still a work in progress!</p><br>";
    msg += "<img src='https://ukyophtho.files.wordpress.com/2018/08/work-in-progress-gif-11.gif'>";
    server.send(404, "text/html", msg);
}


void server_cb_api_screens()
{
    // create json object
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    JsonObject root = doc.to<JsonObject>();

    // create screens array
    JsonArray arr = root.createNestedArray("screens");
    for (ScreenInfo screen : screens)
    {
        arr.add(screen.name);
    }

    // store current screen
    root["current"] = current_screen_id;

    // add error info
    root["error"] = false;
    root["reason"] = "";

    // serialise json
    std::string output = "";
    serializeJsonPretty(doc, output);

    // return object
    server.send(200, "application/json", output.c_str());
}

void server_cb_api_settings()
{
    // create json object
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    JsonObject root = doc.to<JsonObject>();

    // create screens array
    JsonArray arr = root.createNestedArray("screens");
    for (std::string screen : get_screens())
    {
        // exclude wifi settings
        if (screen.compare("<wifi>")) // ie: if string is different to <wifi>
        {
            // create object for screen
            JsonObject screen_obj = arr.createNestedObject();
            screen_obj["name"] = screen;
            
            // add screen settings
            JsonArray settings_obj = screen_obj.createNestedArray("settings");
            for (std::string setting : get_settings(screen.c_str()))
            {
                JsonObject setting_obj = settings_obj.createNestedObject();
                setting_obj["name"] = setting;
                setting_obj["value"] = get_setting(screen.c_str(), setting.c_str(), "");

                // check if setting has values registered to it
                std::vector<std::string> setting_values = get_setting_values(screen.c_str(), setting.c_str());
                if (setting_values.size() > 0) {
                    
                    // create array
                    JsonArray values_obj = setting_obj.createNestedArray("values");
                    for (auto val : setting_values) values_obj.add(val);

                }
            }
        }
    }

    // add error info
    root["error"] = false;
    root["reason"] = "";

    // serialise json
    std::string output = "";
    serializeJsonPretty(doc, output);

    // return object
    server.send(200, "application/json", output.c_str());
}

void server_cb_api_set_screen()
{
    // for some reason, WebServer seems to provide access to POST request bodies
    // through an argument called "plain"

    // check for data in the plain parameter
    if (!server.hasArg("plain")) {
        server.send(400, "application/json", "{\"error\": true, \"reason\": \"Missing parameters\"}");
        return;
    }

    for (uint8_t i = 0; i < server.args(); i++) {
        logi(LOG_TAG, "%s=%s", server.argName(i).c_str(), server.arg(i).c_str());
    }

    // get request parameters
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    deserializeJson(doc, server.arg("plain"));

    // check if correct parameters were sent
    if (!doc.containsKey("screen")) {
        server.send(400, "application/json", "{\"error\": true, \"reason\": \"Missing screen parameter\"}");
        return;
    }

    // check if screen is numeric
    std::string str_screen = doc["screen"].as<std::string>();
    if (!is_number(str_screen)) {
        server.send(400, "application/json", "{\"error\": true, \"reason\": \"Screen parameter is non-numeric\"}");
        return;
    }

    // attempt to switch screen
    int new_screen = std::stoi(str_screen);
    bool success = switch_screen(new_screen);

    // return result
    if (!success) {
        server.send(400, "application/json", "{\"error\": true, \"reason\": \"Invalid screen ID\"}");
        return;
    } else {
        server.send(200, "application/json", "{\"error\": false, \"reason\": \"\"}");
        return;
    }
}

void server_cb_api_set_setting()
{
    // for some reason, WebServer seems to provide access to POST request bodies
    // through an argument called "plain"

    // check for data in the plain parameter
    if (!server.hasArg("plain")) {
        server.send(400, "application/json", "{\"error\": true, \"reason\": \"Missing parameters\"}");
        return;
    }

    // get request parameters
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    deserializeJson(doc, server.arg("plain"));

    for (uint8_t i = 0; i < server.args(); i++) {
        logi(LOG_TAG, "%s=%s", server.argName(i).c_str(), server.arg(i).c_str());
    }

    // check if correct parameters were sent
    if (!doc.containsKey("screen")) {
        server.send(400, "application/json", "{\"error\": true, \"reason\": \"Missing screen parameter\"}");
        return;
    }
    else if (!doc.containsKey("setting")) {
        server.send(400, "application/json", "{\"error\": true, \"reason\": \"Missing setting parameter\"}");
        return;
    }
    else if (!doc.containsKey("value")) {
        server.send(400, "application/json", "{\"error\": true, \"reason\": \"Missing value parameter\"}");
        return;
    }

    // set setting
    set_setting(doc["screen"].as<std::string>().c_str(), doc["setting"].as<std::string>().c_str(), doc["value"].as<std::string>().c_str());
    server.send(200, "application/json", "{\"error\": false, \"reason\": \"\"}");

    // save settings to sd card
    store_settings(FILESYSTEM);
}

void server_cb_api_set_options()
{
    // CORS is so complicated!!!!  who came up with it?
    // this api route is just to deal with CORS preflights that chrome sends
    server.send(200, "text/plain", "");
}

void server_cb_api_ntp()
{
    get_time_from_ntp();
    server.send(200, "application/json", "{\"error\": false, \"reason\": \"\"}");
}

void server_cb_api_get_time()
{
    // create json object
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    JsonObject root = doc.to<JsonObject>();

    // add time
    DateTime now = get_rtc_time();
    root["day"] = now.day();
    root["month"] = now.month();
    root["year"] = now.year();
    root["hours"] = now.hour();
    root["minutes"] = now.minute();
    root["seconds"] = now.second();

    // add error info
    root["error"] = false;
    root["reason"] = "";

    // serialise json
    std::string output = "";
    serializeJsonPretty(doc, output);

    // return object
    server.send(200, "application/json", output.c_str());
}

void server_cb_api_set_time()
{
    // check for data in the plain parameter
    if (!server.hasArg("plain")) {
        server.send(400, "application/json", "{\"error\": true, \"reason\": \"Missing parameters\"}");
        return;
    }

    // get request parameters
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    deserializeJson(doc, server.arg("plain"));

    for (uint8_t i = 0; i < server.args(); i++) {
        logi(LOG_TAG, "%s=%s", server.argName(i).c_str(), server.arg(i).c_str());
    }

    // get current time
    DateTime now = get_rtc_time();
    int current_hours = now.hour();
    int current_minutes = now.minute();
    int current_seconds = now.second();
    int current_day = now.day();
    int current_month = now.month();
    int current_year = now.year();

    // get time from parameters
    if (doc.containsKey("hours")) {
        std::string str_hours = doc["hours"].as<std::string>();
        if (is_number(str_hours)) {
            int hours = std::stoi(str_hours);
            if (hours > -1) current_hours = hours;
        }
    }
    if (doc.containsKey("minutes")) {
        std::string str_minutes = doc["minutes"].as<std::string>();
        if (is_number(str_minutes)) {
            int minutes = std::stoi(str_minutes);
            if (minutes > -1) current_minutes = minutes;
        }
    }
    if (doc.containsKey("seconds")) {
        std::string str_seconds = doc["seconds"].as<std::string>();
        if (is_number(str_seconds)) {
            int seconds = std::stoi(str_seconds);
            if (seconds > -1) current_seconds = seconds;
        }
    }
    if (doc.containsKey("day")) {
        std::string str_day = doc["day"].as<std::string>();
        if (is_number(str_day)) {
            int day = std::stoi(str_day);
            if (day > -1) current_day = day;
        }
    }
    if (doc.containsKey("month")) {
        std::string str_month = doc["month"].as<std::string>();
        if (is_number(str_month)) {
            int month = std::stoi(str_month);
            if (month > -1) current_month = month;
        }
    }
    if (doc.containsKey("year")) {
        std::string str_year = doc["year"].as<std::string>();
        if (is_number(str_year)) {
            int year = std::stoi(str_year);
            if (year > -1) current_year = year;
        }
    }

    // set time
    DateTime thingy(current_year, current_month, current_day, current_hours, current_minutes, current_seconds);
    set_rtc_time(thingy);

    server.send(200, "application/json", "{\"error\": false, \"reason\": \"\"}");
}


void setup_http_server()
{
    logi(LOG_TAG, "starting http server at port %d", SERVER_PORT);

    server.begin(SERVER_PORT);
    server.enableCORS();
    MDNS.addService("http", "tcp", SERVER_PORT);

    // register main routes
    // server.on("/", server_cb_root);
    server.onNotFound(server_cb_404);

    // static file hosting for web interface
    server.serveStatic("/", FILESYSTEM, "/web/index.html");
    server.serveStatic("/index.html", FILESYSTEM, "/web/index.html");
    server.serveStatic("/index.js", FILESYSTEM, "/web/index.js");
    server.serveStatic("/index.css", FILESYSTEM, "/web/index.css");
    server.serveStatic("/ball.css", FILESYSTEM, "/web/ball.css");

    // register api routes
    server.on("/api/get/screen/", server_cb_api_screens);
    server.on("/api/get/setting/", server_cb_api_settings);
    server.on("/api/set/screen/", HTTP_POST, server_cb_api_set_screen);
    server.on("/api/set/screen/", HTTP_OPTIONS, server_cb_api_set_options);
    server.on("/api/set/setting/", HTTP_POST, server_cb_api_set_setting);
    server.on("/api/set/setting/", HTTP_OPTIONS, server_cb_api_set_options);
    server.on("/api/ntp/", HTTP_POST, server_cb_api_ntp);
    server.on("/api/ntp/", HTTP_OPTIONS, server_cb_api_set_options);
    server.on("/api/get/time/", HTTP_GET, server_cb_api_get_time);
    server.on("/api/set/time/", HTTP_POST, server_cb_api_set_time);
    server.on("/api/set/time/", HTTP_OPTIONS, server_cb_api_set_options);
}

void server_handle_client()
{
    server.handleClient();
}