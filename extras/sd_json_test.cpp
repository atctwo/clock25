
void test_sd_json()
{
    // deserialise settings test
    if (sd_available())
    {
        StaticJsonDocument<JSON_BUFFER_SIZE> doc;
        File json = SD.open(SETTINGS_FILE);
        deserializeJson(doc, json);
        json.close();
        deserialise_settings(doc);
    }
    else logi(LOG_TAG, "Deserialisation test: failed because SD isn't available");

    // change setting
    set_setting("<system>", "Cycle Screens", "0");
    set_setting("<system>", "end of line", "0");
    logi(LOG_TAG, "aaa %s", get_setting("<system>", "Cycle Screens", "0").c_str());

    // serialise settings test
    StaticJsonDocument<JSON_BUFFER_SIZE> doc2;
    serialise_settings(doc2);
    std::string output = "";
    serializeJsonPretty(doc2, output);

    if (sd_available())
    {
        File json = SD.open(SETTINGS_FILE, "w");
        json.print(output.c_str());
        json.close();
    }
    else logi(LOG_TAG, "Serialisation test (write): failed because SD isn't available");

    logi(LOG_TAG, "Serialised Settings: \n%s", output.c_str());
}