/* 
    READING 15 MINS
    // Replace with your network credentials -- DEBUG
    //const char* ssid = "New Internet Connection 2G";
    //const char* password = "localhost";

    DHT - 4 
    MQ - 35
*/

#include <Arduino.h>

//AWS FACTORY CERTS
    #include "mycerts.h"

//WIFI and AP
    #include "WiFi.h"
    #include "ESPAsyncWebServer.h"
    #define MAX_WIFI_SCANNER_SIZE 1024

    // Create AsyncWebServer object on port 80
        AsyncWebServer server(80);    


    //Scanned Wifi Networks
        int wifiNetworks = 0;
        int wifi_retries = 0;
        int aws_retries = 0;        

//File System
    #include "LITTLEFS.h"               //LittleFS
    #define FORMAT_LITTLEFS_IF_FAILED true


//JSON
    #include "ArduinoJson.h"            //JSON
    #include <AsyncJson.h>              //JSON

//MQTT
    #include <WiFiClientSecure.h>       //MQTT
    #include <MQTTClient.h>             //MQTT

    //Inits    
    WiFiClientSecure net = WiFiClientSecure(); 
    MQTTClient client = MQTTClient(4096);    
    
//Preferences
    #include <Preferences.h>   
    Preferences pref;
        bool isInternet, isActivated;
        String user_ssid, user_password, user_token;
        String aws_cert, aws_key, device_name; 

//Task Schedular
    #define _TASK_TIMEOUT
    #include <TaskScheduler.h>
        Scheduler ts;    

        //Prototypes

        //Data
            void pushData();

        //Internet
            void connectToInternet();
            void checkConnectivity();
        
        //Connection
            void awsClientLoop();
            void awsConnectForActivation();

        //Sensors
            void calibrateSensors();
            void readSensors();

        //Tasks    
            Task tast_awsConnectForActivation(500 * TASK_MILLISECOND, TASK_FOREVER, &awsConnectForActivation, &ts, false, NULL);
            Task task_AWSClientLoop(100 * TASK_MILLISECOND, TASK_FOREVER, &awsClientLoop, &ts, false, NULL);

            Task task_ConnectWifi(5 * TASK_SECOND, 10, &connectToInternet, &ts, false, NULL);
            Task task_checkConnectivity(10 * TASK_MINUTE, TASK_FOREVER, &checkConnectivity, &ts, false, NULL);

            Task task_CalibrateSensors(1 * TASK_SECOND, TASK_ONCE, &calibrateSensors, &ts, false, NULL);
            Task task_ReadSensors(3 * TASK_SECOND, TASK_FOREVER, &readSensors, &ts, false, NULL);

            Task task_pushData(15 * TASK_MINUTE, TASK_FOREVER, &pushData, &ts, false, NULL);




//SENSORS
    #define MQ_PIN              (34)   
    #define DHT_PIN             4

    #include "DHTesp.h"                 //DHT
    #include <MQUnifiedsensor.h>        //MQ135

    //Configuration
    //MQ135
    #define         Board                   ("ESP-32")             
    #define         Type                    ("MQ-135")     
    #define         Voltage_Resolution      (3.3)          
    #define         ADC_Bit_Resolution      (12)           
    #define         RatioMQ135CleanAir      3.6

    //DH Data Struct
        struct myDH_Data {
            float temp;
            float humidity;
            float heatindex;
            float dewpoint;
        };

    //MQ Data Struct
        struct myMQ_Data {
            float co;
            float co2;
            float alcohol;
            float tolueno;
            float nh4;
            float acetona;
        };      

    //Initialization
    MQUnifiedsensor MQ135(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ_PIN, Type);
    DHTesp dht; //DHT    
    myMQ_Data current_MQ_Data = {0,0,0,0,0,0};
    myDH_Data current_DH_Data = {0,0,0,0};   

    //MQ Calibration Time
    bool isMQCalib = true;
    int mq_Calib_i = 0;      

//Ping
    #include <ESP32Ping.h>
    const char* remote_host = "www.google.com";


//Main Setup
void setup() {
    //0. Serial Port
        Serial.begin(115200);
        delay(200);

        Serial.println("Serial Begin");
    
    //1. Mount File System
        if(!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
            Serial.println("LITTLEFS Mount Failed");
            return;
        }        
        
        //Debug
        listDir(LITTLEFS, "/", 3);    

    //2. Get Preferences
        pref.begin("settings", false);

        //Internet Settings
            isInternet              = pref.getBool("internet", false);
            user_ssid               = pref.getString("ssid", "");
            user_password           = pref.getString("password", "");  

        //AWS Settings
            isActivated             = pref.getBool("activated", false);
            aws_cert                = pref.getString("device_cert", "");
            aws_key                 = pref.getString("device_key", "");
            device_name             = pref.getString("device_name", ""); 

            user_token             = pref.getString("user_token", "");  

        //Generate Random Seed
            if (!user_token) {
                int randNumber = random(999999);
                user_token = String(randNumber);
                pref.putString("user_token", user_token);
            }

        pref.end();                            

        //Serial.println(aws_cert);
        //Serial.println("-------------------------");
        //Serial.println(aws_key);
        //Serial.println("-------------------------");
        //Serial.println(device_name);


    //3. Initialize Sensors
        //MQ135
        MQ135.setRegressionMethod(1);
        MQ135.init();    
        MQ135.setRL(1);  

        //DHT
        dht.setup(DHT_PIN, DHTesp::DHT22);
        Serial.println("DHT initiated");         

        //Check DHT
        if (dht.getStatus() != 0) {
            Serial.println("DHT Sensor Failed");
        }

        //Calibrate Sensor
        task_CalibrateSensors.enable();

    //4. Start Reading Sensors
        task_ReadSensors.enable();

    //5. Check Internet
        if (!isInternet) {
            //Set Access Point
            setAccessPoint();
            return;
        }

    //6. Connect to Internt Normal
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(500);

        wifi_retries = 0;

        task_ConnectWifi.enable();    
}

//Main Loop
void loop() {
    ts.execute();   //Task Schedular loop
}

/* FUNCTIONS */

/* AWS */
    //AWS Device Message

    //Deivce Messages - Normal
    void aws_Device_Messages(String &topic, String &payload) {
        Serial.println(topic);
        Serial.println(payload);
    }

    //Device Activation - Messages Recevied
    void aws_device_actvation_messages(String &topic, String &payload) {

        //Registeration Token - Step 1
        if (topic.equals(AWS_CERT_REQUEST_ACCEPT)) {
            delay(2000);
            Serial.println("Step 1: Registeration Token");

            //Parse Payload - Deserialize
            DynamicJsonDocument doc(payload.length());  
            DeserializationError error = deserializeJson(doc, payload);

            //Handle Error
            if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return;
            }            

            //Save Data
            //const char* certificateId       = doc["certificateId"];
            const char* certificatePem      = doc["certificatePem"];
            const char* privateKey          = doc["privateKey"];

            //Get Token
            const char* certificateOwnershipToken = doc["certificateOwnershipToken"];          

            //Save Certs
            //String user_token;
            pref.begin("settings", false);
            pref.putString("device_cert", certificatePem);
            pref.putString("device_key", privateKey);
            //pref.getString("token", user_token);
            pref.end();                     

            aws_cert = certificatePem;
            aws_key = privateKey;
            
            //Serial.println(user_token);

            //Serialize JSON - Token Verification
            DynamicJsonDocument doc_token(1024);
            doc_token["certificateOwnershipToken"]  = certificateOwnershipToken;

            JsonObject parameters                   = doc_token.createNestedObject("parameters");
            parameters["SerialNumber"]  = get_Chip_Number();
            parameters["mac_addr"]      = get_Mac_Addr();
            parameters["chip_id"]       = get_Chip_Number();
            parameters["token"]         = user_token;
            String json_string;
            serializeJson(doc_token, json_string);     
            
            Serial.println(json_string);

            //Republish
            delay(2000);
            client.publish(AWS_CERT_REQUEST_PROVISION, json_string);            
        }    

        //Device Registeration Confirmed - Step 2
        if (topic.equals(AWS_CERT_PROVISION_ACCEPT)) {
            delay(2000);
            Serial.println("Step 2: Device Registeration Confirmed");

            //Deserialize
            DynamicJsonDocument doc(200);
            deserializeJson(doc, payload);

            bool device_status = doc["deviceConfiguration"]["success"]; // true
            const char* thingName = doc["thingName"]; // "BRAP_MY_NAME_11"         

            Serial.println("Thing Name");
            Serial.println(thingName);
            device_name = thingName;

            //Handle Error
            if (!device_status) {
                Serial.println("Device Registeration failed");
                return;
            }           

            //Device Ready - Store Settings
            pref.begin("settings", false);          
            pref.putBool("activated", true);
            pref.putString("device_name", thingName);        
            pref.end();          

            //Disconnect Current Session
            client.disconnect();

            //Unsubscribe
            //Step 1
            client.unsubscribe(AWS_CERT_REQUEST_REJECT);
            client.unsubscribe(AWS_CERT_REQUEST_ACCEPT);

            //Step 2
            client.unsubscribe(AWS_CERT_PROVISION_REJECT);
            client.unsubscribe(AWS_CERT_PROVISION_ACCEPT);      

            //Device Activated
            Serial.println("Device Activated");
            delay(5000);
            
            isActivated = true;

            //Disable AWS Loop
            task_AWSClientLoop.disable();

            //Reconnect as Normal Wifi
                WiFi.mode(WIFI_STA);
                WiFi.disconnect();
                delay(500);

                wifi_retries = 0;

                task_ConnectWifi.enable();         
        }    

        //Errors
        if (topic.equals(AWS_CERT_REQUEST_REJECT)) {
            //Request Rejected
            //gen_Error("0x1001");
            Serial.println("Certificate Rejected");
            return;
        }

        if (topic.equals(AWS_CERT_PROVISION_REJECT)) {
            //Request Rejected
            //gen_Error("0x1002"); 
            Serial.println("Privisioning Device Rejected");
            return;
        }                    
    }

    //AWS Activate Device - Connect
    void awsConnectForActivation() {
        
        //Device Name
        String deviceCHIPID = get_Chip_Number();    
        int str_len = deviceCHIPID.length() + 1; 
        char DEV_NAME[str_len];
        deviceCHIPID.toCharArray(DEV_NAME, str_len);   

        //Attempt Connection
        if (!client.connect(DEV_NAME) && aws_retries < AWS_MAX_RECONNECT_TRIES) {
            Serial.println("Attempting AWS Connection");
            aws_retries++;
            return;
        }

        //Timedout
        if(!client.connected()){
            Serial.println(" Timeout!");    
            tast_awsConnectForActivation.disable();  
            return;
        }    

        //Disable Task
        tast_awsConnectForActivation.disable();

        Serial.println("Connected to AWS for Activation");

        //Connected
        if (!isActivated) {
            Serial.println("Connected to AWS!: Activation Mode");

            //Receive Messages
                client.onMessage(&aws_device_actvation_messages);        

            //Subscribe to Channels - For Activation
                //Step 1 - Registeration
                client.subscribe(AWS_CERT_REQUEST_REJECT);
                client.subscribe(AWS_CERT_REQUEST_ACCEPT);

                //Step 2 - Token Verification
                client.subscribe(AWS_CERT_PROVISION_REJECT);
                client.subscribe(AWS_CERT_PROVISION_ACCEPT);         

                //Ping Topic for Token
                StaticJsonDocument<16> doc;
                doc["test_data"] = 1;
                String json_string;
                serializeJson(doc, json_string);         
                
                //Publish to Request for New Certificate
                client.publish(AWS_CERT_REQUEST_CREATE, json_string);            
        }
    }

    //AWS Client loop
    void awsClientLoop() {
        if (WiFi.status() == WL_CONNECTED) {
            client.loop();
        }
    }    


/* CONNECTIVITY */

    //Connect to Internet
    void connectToInternet() {

        //First Run
        if (wifi_retries == 0) {
            WiFi.begin(user_ssid.c_str(), user_password.c_str());
            wifi_retries++;        
            return;
        }

        //Attempt Connection
        if (WiFi.status() != WL_CONNECTED && wifi_retries < 5) {
            Serial.println("Attempting Wifi Connection: " + wifi_retries);
            wifi_retries++;
            return;
        }     

        //Case Where no Internet Available
            //Unable to Connect
            if (WiFi.status() != WL_CONNECTED) {
                Serial.println("Password Failed");        
                wifi_retries = 0;

                //Disable Task
                task_ConnectWifi.disable();

                //Set Back to AP Mode
                setAccessPoint();
                return;
            }

            //Fetch IP Address
            if (WiFi.localIP().toString() == "0.0.0.0" && wifi_retries < 10) {
                Serial.println("Fetching IP");
                wifi_retries++;
                return;
            }

            //Check IP Address
            if (WiFi.localIP().toString() == "0.0.0.0") {
                //Disable Task
                task_ConnectWifi.disable();      
                Serial.println("Got No IP");

                //Set Access Point
                setAccessPoint();
                return;  
            }    

        //Connect to the Internet and Got IP
        Serial.println("Connected to the Internet");
        Serial.println(WiFi.localIP());

        //Disabled Wifi Task
        task_ConnectWifi.disable();

        //Store Internet Settings
        if (!isInternet) {
            pref.begin("settings", false);
            pref.putString("ssid", user_ssid);
            pref.putString("password", user_password);
            pref.putBool("internet", true);
            pref.end();    

            isInternet = true;
        }

        //Check Activation
        if (!isActivated) {
            //Start Activation
                
                //Set Factory Certs
                net.setCACert(AWS_ROOT_CA);
                net.setCertificate(AWS_INIT_CERT);
                net.setPrivateKey(AWS_INIT_KEY);     

                //Enable AWS Loop
                task_AWSClientLoop.enable();    

                //Connect to AWS
                client.begin(AWS_IOT_ENDPOINT, 8883, net);

                Serial.print("Connecting to AWS to Activate");     
                aws_retries = 0;
                tast_awsConnectForActivation.enable();   

            return;
        }

        //Connect to AWS - Normal Operation

            //Get Preferences -- AWS Settings
            pref.begin("settings", false);                                     
            String aws_cert                = pref.getString("device_cert", "");
            String aws_key                 = pref.getString("device_key", "");
            String device_name             = pref.getString("device_name", ""); 
            pref.end();   

            //Cert
            char USER_AWS_CERT[aws_cert.length()];
            aws_cert.toCharArray(USER_AWS_CERT, aws_cert.length()); 

            //Key
            char USER_AWS_KEY[aws_key.length()];
            aws_key.toCharArray(USER_AWS_KEY, aws_key.length());   

            //Device Name
            char USER_DEVICE_NAME[device_name.length() + 1];
            device_name.toCharArray(USER_DEVICE_NAME, device_name.length() + 1);             
        
            // Configure WiFiClientSecure to use the AWS IoT device credentials
            net.setCACert(AWS_ROOT_CA);
            net.setCertificate(USER_AWS_CERT);
            net.setPrivateKey(USER_AWS_KEY);      
        
            //Connect to AWS
            Serial.print("Connecting to AWS");   
            client.begin(AWS_IOT_ENDPOINT, 8883, net);
            aws_retries = 0;

            
            while (!client.connect(USER_DEVICE_NAME) && aws_retries < AWS_MAX_RECONNECT_TRIES) {
                Serial.print(".");
                delay(200);
                aws_retries++;
            }

            if(!client.connected()){
                Serial.println("AWS IoT Timeout!");
                return;
            }   

            Serial.println("AWS IoT Connected!");    

        //Enable AWS Loop
        task_AWSClientLoop.enable();        

        //Connected

            //AWS Message Function
            client.onMessage(&aws_Device_Messages);       

            //Device Shadows
                //Device - Control Topics - Subscribe - Get
                client.subscribe("$aws/things/" + device_name + "/shadow/name/device_control/get/accepted");
                client.subscribe("$aws/things/" + device_name + "/shadow/name/device_control/get/rejected");
                
                //Device - Control Topics - Subscribe - Update
                client.subscribe("$aws/things/" + device_name + "/shadow/name/device_control/update/accepted");
                client.subscribe("$aws/things/" + device_name + "/shadow/name/device_control/update/rejected");            
                    
            //Device Settings Latest

                //Ping Topic for Data
                StaticJsonDocument<16> doc;
                doc["test_data"] = 1;
                String json_string;
                serializeJson(doc, json_string);     

                //Device - Control Topics - Publish - Get
                client.publish("$aws/things/" + device_name + "/shadow/name/device_control/get", json_string);
                

        //Enable Connectivity Check
            task_checkConnectivity.enable();            

        //Task for Pushing Data
            task_pushData.enable();


            //delay(3000);
            //task_awsConnect.enable();

        //Normal AWS Data
    }

    //Push Data
    void pushData() {
        Serial.println("Publishing Device Data");
        if (WiFi.status() != WL_CONNECTED) {
            //Store Data Locally
            Serial.println("No Wifi yet");
            return;
        }

        if (!client.connected()) {
            Serial.println("No MQTT yet");
            //reconnect_AWS();            
            return;
        }        

        DynamicJsonDocument doc(384);
        doc["temp"]         = current_DH_Data.temp;
        doc["humidity"]     = current_DH_Data.humidity;
        doc["heatindex"]    = current_DH_Data.heatindex;
        doc["dewpoint"]     = current_DH_Data.dewpoint;

        doc["co"]           = current_MQ_Data.co;
        doc["co2"]          = current_MQ_Data.co2;
        doc["alcohol"]      = current_MQ_Data.alcohol;
        doc["tolueno"]      = current_MQ_Data.tolueno;
        doc["nh4"]          = current_MQ_Data.nh4;
        doc["acetona"]      = current_MQ_Data.acetona;

        //doc["rpm"]          = rpm_fan;
        //doc["duty_cycle"]   = FAN_SPEED_DUTY;
        //doc["firmware"]     = FRIMWARE_VERSION;

        doc["rssi"]         = WiFi.RSSI();
        doc["device_id"]    = device_name;

        String string_json;
        serializeJson(doc, string_json);  

        client.publish("airquality/data", string_json); 

    }

    //Check Connectivity
    void checkConnectivity() {
        Serial.println("Checking Connectivity");

        //Check Wifi Status
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Got Wifi");
            return;            
        }

        //Check IP Address
        if (WiFi.localIP().toString() != "0.0.0.0") {
            Serial.println("Got IP");
            return;
        }    

        //Ping Device
        if (Ping.ping(remote_host)) {
            Serial.println("Got Ping");
            return;
        }    

        //Connect to WiFi Normal Mode
            WiFi.mode(WIFI_STA);
            WiFi.disconnect();
            delay(500);

            wifi_retries = 0;
            task_ConnectWifi.enable();

        //Disable 
            task_checkConnectivity.disable();

    }

    //Set Access Point
    void setAccessPoint() {

        WiFi.mode(WIFI_AP_STA);
        WiFi.disconnect();
        delay(500);

        //Scan Wifi Networks
            wifiNetworks = WiFi.scanNetworks();
            delay(3000);    

        //SSID BreatheIO        
            Serial.println("Set softAPConfig");
            IPAddress Ip(10, 0, 0, 1);
            IPAddress NMask(255, 255, 255, 0);
            WiFi.softAPConfig(Ip, Ip, NMask);
            WiFi.softAP("BreatheIO");

            IPAddress myIP = WiFi.softAPIP();
            Serial.print("AP IP address: ");
            Serial.println(myIP);    

        //ESP Async Server
            //Check Connectivity
            server.on("/check", HTTP_GET, [](AsyncWebServerRequest *request){
                Serial.println("Checking Internet");

                if (WiFi.status() != WL_CONNECTED) {
                    Serial.println("Wifi Failed: Credentials");
                    DynamicJsonDocument doc(16);
                    doc["success"] = false;
                    String json_string;
                    serializeJson(doc, json_string);
                    request->send(200, "application/json", json_string); 
                    return;
                }

                DynamicJsonDocument doc(16);
                doc["success"] = true;
                String json_string;
                serializeJson(doc, json_string);
                request->send(200, "application/json", json_string);

            });


            //Connect to Internet - Version 2       
            server.on("/connect", HTTP_PUT, [](AsyncWebServerRequest *request){

                //Check SSID
                if (request->hasParam("ssid", true)) {
                    user_ssid = request->getParam("ssid", true)->value();
                } else {
                    DynamicJsonDocument doc(32);
                    doc["success"] = false;
                    doc["error"] = "No SSID Found";
                    String json_string;
                    serializeJson(doc, json_string);
                    request->send(200, "application/json", json_string);
                    return;            
                }         

                if (request->hasParam("password", true)) {
                    user_password = request->getParam("password", true)->value();
                }     

                //Send Response Back
                AsyncJsonResponse* response = new AsyncJsonResponse(false, MAX_WIFI_SCANNER_SIZE);
                JsonObject root = response->getRoot();           
                root["success"] = true;
                response->setLength();
                request->send(response); 

                //Start Wifi Connection   
                wifi_retries = 0;         
                task_ConnectWifi.enable();
            });

            //Scan Wifi Networks
            server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
                Serial.println("Scan Started");
                if (WiFi.scanComplete() != -1) {
                    WiFi.scanDelete();
                    WiFi.scanNetworks(true);
                }

                DynamicJsonDocument doc(16);
                doc["success"] = true;
                String json_string;
                serializeJson(doc, json_string);
                request->send(200, "application/json", json_string);         
            });

            //Send Wifi Results
            server.on("/results", HTTP_GET, [](AsyncWebServerRequest *request){
                Serial.println("Sending Results");
                int numNetworks = WiFi.scanComplete();
                if (numNetworks > -1) {
                    AsyncJsonResponse* response = new AsyncJsonResponse(false, MAX_WIFI_SCANNER_SIZE);
                    JsonObject root = response->getRoot();
                    JsonArray networks = root.createNestedArray("networks");
                    for (int i = 0; i < numNetworks; i++) {
                        JsonObject network = networks.createNestedObject();
                        network["rssi"] = WiFi.RSSI(i);
                        network["ssid"] = WiFi.SSID(i);
                        network["bssid"] = WiFi.BSSIDstr(i);
                        network["channel"] = WiFi.channel(i);
                        network["encryption_type"] = (uint8_t)WiFi.encryptionType(i);
                    }
                    root["success"] = true;
                    root["rescan"] = false;
                    response->setLength();
                    request->send(response);
                } else {
                    DynamicJsonDocument doc(32);
                    doc["success"] = true;
                    doc["rescan"] = true;
                    String json_string;
                    serializeJson(doc, json_string);            
                    request->send(200, "application/json", json_string); 
                }
            });         

            /* SERVE FILES */
                //logo
                server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request){
                    request->send(LITTLEFS, "/logo.png", "image/png");
                });  

                //Stylesheet
                server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
                    request->send(LITTLEFS, "/style.css", "text/css");
                });   

                //Bootstrap
                server.on("/mini-default.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
                    request->send(LITTLEFS, "/mini-default.min.css", "text/css");
                });           

                //Serve QR Code
                server.on("/qrcode.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
                    request->send(LITTLEFS, "/qrcode.min.js", "text/javascript");
                });   

                //Serve Jquery
                server.on("/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
                    request->send(LITTLEFS, "/jquery.min.js", "text/javascript");
                });    

                // Route for root / web page
                server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
                    request->send(LITTLEFS, "/index.html", String(), false);
                });

            server.onNotFound(notFound);
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");

            server.begin();          

    }

    //Web Server - Not Found
    void notFound(AsyncWebServerRequest *request) {
        //request->send(404, "text/plain", "Not found");

        if (request->method() == HTTP_OPTIONS) {   
            request->send(200);
        } else {
            //request->send(404);
            request->send(404, "text/plain", "Not found");
        }        
    }    


/* SENSORS */
    //Calibrate Sensors
    void calibrateSensors() {
        Serial.print("Calibrating please wait.");
        float calcR0 = 0;
        for(int i = 1; i<=50; i ++) {
            MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
            calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
            Serial.print(".");          
        }

        MQ135.setR0(calcR0/10);
        Serial.println("  done!.");           
    }

    //Read Sensors - Version 2.0
    void readSensors() {
        //DHT
            TempAndHumidity newValues = dht.getTempAndHumidity();       

            if (dht.getStatus() != 0) {
                //Handle Error
                Serial.println("DHT11 error status: " + String(dht.getStatusString()));
                
            } else {
                float heatIndex = dht.computeHeatIndex(newValues.temperature, newValues.humidity);
                float dewPoint = dht.computeDewPoint(newValues.temperature, newValues.humidity);

                //Update Current Data
                current_DH_Data.temp = newValues.temperature;
                current_DH_Data.humidity = newValues.humidity;
                current_DH_Data.heatindex = heatIndex;
                current_DH_Data.dewpoint = dewPoint;  

                //Serial.print("Temp: ");
                //Serial.println(current_DH_Data.temp);
                //Serial.print("Humidity: ");
                //Serial.println(current_DH_Data.humidity);

            }              

        //MQ Data    
            MQ135.update();

            MQ135.setA(605.18); MQ135.setB(-3.937); // Configurate the ecuation values to get CO concentration
            float CO = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup

            MQ135.setA(77.255); MQ135.setB(-3.18); // Configurate the ecuation values to get Alcohol concentration
            float Alcohol = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup

            MQ135.setA(110.47); MQ135.setB(-2.862); // Configurate the ecuation values to get CO2 concentration
            float CO2 = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup

            MQ135.setA(44.947); MQ135.setB(-3.445); // Configurate the ecuation values to get Tolueno concentration
            float Tolueno = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup

            MQ135.setA(102.2); MQ135.setB(-2.473); // Configurate the ecuation values to get NH4 concentration
            float NH4 = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup

            MQ135.setA(34.668); MQ135.setB(-3.369); // Configurate the ecuation values to get Acetona concentration
            float Acetona = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup

            current_MQ_Data.co          = CO;
            current_MQ_Data.alcohol     = Alcohol;
            current_MQ_Data.co2         = CO2;
            current_MQ_Data.tolueno     = Tolueno;
            current_MQ_Data.nh4         = NH4;
            current_MQ_Data.acetona     = Acetona;  

            //Serial.print("CO: ");
            //Serial.println(current_MQ_Data.co);
            //Serial.print("Alcohol: ");
            //Serial.println(current_MQ_Data.alcohol);
            //Serial.print("CO2: ");
            //Serial.println(current_MQ_Data.co2);
            //Serial.print("Tolueno: ");
            //Serial.println(current_MQ_Data.tolueno);
            //Serial.print("NH4: ");
            //Serial.println(current_MQ_Data.nh4);
            //Serial.print("Acetona: ");
            //Serial.println(current_MQ_Data.acetona);

            
    }


/* DEVICE INFO */
    //Get Chip ID
    String get_Chip_Number() {
        uint32_t chipId = 0;
        for(int i=0; i<17; i=i+8) {
            chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff0) << i;
        }  
        return String(chipId);
    }

    //Get Mac Address
    String get_Mac_Addr() {
        return WiFi.macAddress();
    }

/* EXTRA */
    //List Directory
    void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
        Serial.printf("Listing directory: %s\r\n", dirname);

        File root = fs.open(dirname);
        if(!root){
            Serial.println("- failed to open directory");
            return;
        }
        if(!root.isDirectory()){
            Serial.println(" - not a directory");
            return;
        }

        File file = root.openNextFile();
        while(file){
            if(file.isDirectory()){
                Serial.print("  DIR : ");
                Serial.println(file.name());
                if(levels){
                    listDir(fs, file.name(), levels -1);
                }
            } else {
                Serial.print("  FILE: ");
                Serial.print(file.name());
                Serial.print("\tSIZE: ");
                Serial.println(file.size());
            }
            file = root.openNextFile();
        }
    }


/* REFERENCE */

    /* 
    //4. Scan Wifis
        WiFi.mode(WIFI_AP_STA);
        WiFi.disconnect();
        delay(500);

        wifiNetworks = WiFi.scanNetworks();
        delay(5000);


    //Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP());    
    */

    //Captive Portal
    //#include <DNSServer.h>
    //DNSServer dnsServer;

    //dnsServer.processNextRequest();

        //DNS Server
        //dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        //dnsServer.start(53, "*", WiFi.softAPIP());
   
        //server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP

    /* 
        class CaptiveRequestHandler : public AsyncWebHandler {
        public:
        CaptiveRequestHandler() {}
        virtual ~CaptiveRequestHandler() {}

        bool canHandle(AsyncWebServerRequest *request){
            //request->addInterestingHeader("ANY");
            return true;
        }

        void handleRequest(AsyncWebServerRequest *request) {
            //request->send(LITTLEFS, "/index.html", String(), false);

            AsyncResponseStream *response = request->beginResponseStream("text/html");
            response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
            response->print("<p>This is out captive portal front page.</p>");
            response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
            response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
            response->print("</body></html>");
            request->send(response);

        }
        };
    */