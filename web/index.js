
function load_screens()
{
    console.log("Loading Screens");

    fetch("http://clock25.local/api/get/screen/").
    then(res => res.json()).
    then(data => {
        console.log(data);

        // get select
        let screens = document.getElementById("select-screens");

        // clear select
        while (screens.childElementCount > 0) screens.remove(0);

        // add screens
        data.screens.forEach((screen, i) => {
            
            // create new option element
            let opt = document.createElement("option");
            opt.innerText = screen;
            opt.value = i;

            // add option to select
            screens.add(opt);

        });

        // set selected element to current screen
        screens.value = data.current;

    }).
    catch(err => console.log(err));
}

function set_screen()
{
    console.log("Setting screen");

    // get select
    let screens = document.getElementById("select-screens");

    // get value
    let screen_id = screens.value;

    // send new screen id to server
    fetch("http://clock25.local/api/set/screen/", {
        method: "POST",
        mode: "cors",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({
            screen: screen_id
        })
    }).
    then(res => res.json()).
    then(data => {
        if (data.error) {
            console.error("Error setting screen:", data.reason)
            show_error(data.reason);
        }
    }).
    catch(err => {
        console.log(err);
        show_error(err);
    });
}

function load_settings()
{
    console.log("Loading settings");

    // get settings from clock
    fetch("http://clock25.local/api/get/setting/").
    then(res => res.json()).
    then(data => {

        if (data.error) {
            console.error("Error getting settings:", data.reason)
            show_error(data.reason);
        } else {

            // first, remove all existing settings sections
            let existing_sections = document.getElementsByClassName("setting-section generated");
            for (var i = 0; i < existing_sections.length; i++)
            {
                existing_sections[i].parentElement.removeChild(existing_sections[i]);
            }

            console.log(data);

            // create a new section for each setting group
            let main_container = document.getElementById("main-container");
            data.screens.forEach((screen, i) => {

                // create new details and summary elements
                let details = document.createElement("details");
                let summary = document.createElement("summary");
                let contents = document.createElement("div");
                let table = document.createElement("table");

                // set classes
                details.classList.add("setting-section");
                details.classList.add("generated");
                // details.open = true;

                // set section title
                switch(screen.name)
                {
                    case "<ntp>":
                        summary.innerText = "NTP (Network Time Protocol)"
                        break;

                    case "<system>":
                        summary.innerText = "System"
                        break;

                    case "<weather>":
                        summary.innerText = "Weather Settings"
                        break;

                    default:
                        summary.innerText = screen.name;
                }

                // add NTP button
                if (screen.name == "<ntp>")
                {
                    // create button
                    let ntp_button = document.createElement("button");
                    ntp_button.innerText = "Get time using NTP";
                    ntp_button.onclick = get_time_from_ntp;
                    contents.appendChild(ntp_button);
                }

                // add settings to table
                screen.settings.forEach((setting, j) => {

                    // create new table row
                    let row = table.insertRow(-1);

                    // create cell for setting name
                    let cell_name = row.insertCell(-1);
                    cell_name.innerText = setting.name;

                    // create cell for setting value
                    let cell_value = row.insertCell(-1);
                    let setting_input;

                    // if there are no pre-determined values, use a regular input
                    if (!("values" in setting))
                    {
                        setting_input = document.createElement("input");
                        setting_input.value = setting.value;
                        if (!isNaN(setting.value)) setting_input.type = "number";   // if value is numeric, set the input type to be number
                        cell_value.appendChild(setting_input);
                    }
                    else // otherwise, use a <select> tag
                    {
                        // create select wrapper
                        let select_wrapper = document.createElement("div");
                        select_wrapper.classList.add("select-wrapper");

                        // create select
                        setting_input = document.createElement("select");

                        // add options
                        setting.values.forEach(val => {

                            // create option
                            var opt = document.createElement("option");

                            // set opt name and value
                            opt.innerText = val;
                            opt.value = val;

                            // add option to select
                            setting_input.appendChild(opt);

                        });

                        // select current value
                        setting_input.value = setting.value;

                        // add to document
                        select_wrapper.appendChild(setting_input);
                        cell_value.appendChild(select_wrapper);
                    }

                    // input element oninput
                    setting_input.addEventListener("change", e => {
                        console.log(`New value for ${screen.name}:${setting.name}: ${setting_input.value}`);
                        set_setting(screen.name, setting.name, setting_input.value, setting_input)
                    });

                });

                // add summary and contents to details
                contents.appendChild(table);
                details.appendChild(summary);
                details.appendChild(contents);

                main_container.appendChild(details);

            });

        }

    }).
    catch(err => console.log(err));
}

function set_setting(screen_name, setting_name, value, input)
{
    console.log("Setting setting", screen_name, setting_name, "to", value);

    // get select
    let screens = document.getElementById("select-screens");

    // get value
    let screen_id = screens.value;

    // send new screen id to server
    fetch("http://clock25.local/api/set/setting/", {
        method: "POST",
        mode: "cors",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({
            screen: screen_name,
            setting: setting_name,
            value: value
        })
    }).
    then(res => res.json()).
    then(data => {
        if (data.error) {
            console.error("Error setting setting:", data.reason)
            show_error(data.reason);
        } else {

            // set input field border
            if (input) input.style.borderColor = "lawngreen";

        }
    }).
    catch(err => {
        console.log(err);
        show_error(err);
    });
}

function get_time_from_ntp()
{
    // send request to get time from ntp
    fetch("http://clock25.local/api/ntp/", {
        method: "POST",
        mode: "cors"
    }).
    then(res => res.json()).
    then(data => {
        if (data.error) {
            console.error("Error getting time from NTP:", data.reason)
            show_error(data.reason);
        }
    }).
    catch(err => {
        console.log(err);
        show_error(err);
    });
}


function show_error(msg)
{
    // get error element
    let err = document.getElementById("error-box");
    let eee = document.getElementById("error-msg");

    // set error message
    eee.innerText = msg;

    // show error message
    err.style.display = "block";
}



function title_easter_egg()
{
    document.getElementById("title").style.color = (function(){var c="#";for(var i=0;i<3;i++){c+=Math.round(Math.random()*255).toString(16).padStart(2, "0")};return c})();
}


// on document ready
console.log("Loading thingfy :)");
load_screens();
load_settings();













// fetch("http://clock25.local/api/get/setting/").
//     then(res => res.json()).
//     then(data => {
//         console.log(data);
//         let speed = data.screens[3].settings[0].value;
//         document.getElementById("rainbow-slider").value = speed;
//         document.getElementById("rainbow-speed").innerText = speed;
//     }).
//     catch(err => console.log(err));

// function thingy()
// {
//     let slider = document.getElementById("rainbow-slider");
//     let counter = document.getElementById("rainbow-speed");

//     counter.innerText = slider.value;

//     fetch("http://clock25.local/api/set/setting/", {
//         method: "POST",
//         mode: "no-cors", 
//         headers: {
//             "Content-Type": "application/json"
//         },
//         body: JSON.stringify({
//             screen: "Digital Clock",
//             setting: "Rainbow Speed",
//             value: slider.value
//         })
//     }).
//         then(res => res.json()).
//         then(data => console.log(data)).
//         catch(err => console.log(err));
// }