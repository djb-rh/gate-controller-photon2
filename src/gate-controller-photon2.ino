/*
 * Project gate-controller-photon2
 * Description: control the gate!
 * Author: Donnie Barnes <djb@donniebarnes.com>
 * Date: December 24, 2023
 */

#include <stdlib.h>

SYSTEM_MODE(AUTOMATIC);

int RELAY = D2;
bool relay_state = 0;

int triggerRelay(String command);
int changeAntenna(String command);

bool tripped[6];

int debugTrips[6];

int minTrips = 5;
int skipCounter = 0;

String eventName;

char dev_name[32] = "";
char pubString[40] = "";
char restarted_msg[11] = "_restarted";
bool publishName = false;

SerialLogHandler logHandler;

void handler(const char *topic, const char *data){
    strncpy(dev_name, data, sizeof(dev_name)-1);
    publishName = true;
}

void pubState(const char *topic, const char *data){
	char pub[40] = "";
	strcat(pub, pubString);
	strcat(pub, "_1");
        char skips[sizeof(int)*8+1] = "";
        itoa(skipCounter, skips, 10);
        char stats[sizeof(int)*8+5] = "";
	if (relay_state) {
            strcat(stats, "ON ");
            strcat(stats, skips);
            Particle.publish(pub, stats, PRIVATE);
        }
	else {
            strcat(stats, "OFF ");
            strcat(stats, skips);
            Particle.publish(pub, stats, PRIVATE);
        }

}

void skipCommand(const char *topic, const char *data){
        skipCounter = atoi(data);
        pubState(NULL, NULL);
}

/* This function is called once at start up ----------------------------------*/
void setup()
{
	
        pinMode(RELAY, OUTPUT);
        digitalWrite(RELAY, LOW); // close gate by default at power up
        relay_state = 0;
	Particle.function("Relay: 1on, 1off", triggerRelay);
	Particle.function("Antenna: 0 int, 1 ext", changeAntenna);
	Particle.subscribe("particle/device/name", handler);
    	Particle.publish("particle/device/name");
    	// allow for up to 3 sec and maximum time to attend to cloud tasks
    	for (uint32_t ms = millis(); millis() - ms < 3000 && !publishName; Particle.process());
	Serial.begin(115200);
	Serial.println(dev_name);

	strcat(pubString, dev_name);
	strcat(dev_name, "_relay_1");
	Particle.subscribe(dev_name, triggerRelayDos, MY_DEVICES);

	// listen for monitors to ask for state and if they do, report it (this report is the same
	// as what gets reported when the gate state is changed)
	Particle.subscribe("getstate", pubState, MY_DEVICES);

        // listen for "skipcommand" if then increment the skipCounter if given
	Particle.subscribe("skipcommand", skipCommand, MY_DEVICES);

	// publish state on boot up, too
	pubState(NULL, NULL);

	Log.info("I'm awake.");



}

/* This function loops forever --------------------------------------------*/
void loop()
{
}

int triggerRelayDos(const char *event, const char *data){
    if(skipCounter==0){
        Serial.println(data);
        triggerRelay(data);
    }
    else {
        skipCounter--;
        pubState(NULL, NULL);
    }
    return 1;
}

int triggerRelay(String command){
	//Relay Specific Command
	eventName = pubString;
	eventName+="_";
	int relayNumber = command.substring(0,1).toInt();
	Serial.print("relayNumber: ");
	Serial.println(relayNumber);
	String relayCommand = command.substring(1);
	Serial.print("relayCommand:");
	Serial.print(relayCommand);
	Serial.println(".");
	if(relayCommand.equalsIgnoreCase("on")){
		Serial.println("Turning on relay");
                relay_state = 1;
                digitalWrite(RELAY, HIGH);
		eventName+=(relayNumber);
		Particle.publish(eventName, "ON", PRIVATE);
		Serial.println("returning");
		WiFiSignal sig = WiFi.RSSI();
		Log.info("WiFi signal strength: %.02f%%", sig.getQuality());
		return 1;
	}
	if(relayCommand.equalsIgnoreCase("off")){
                relay_state = 0;
                digitalWrite(RELAY, LOW);
		eventName+=(relayNumber);
		Particle.publish(eventName, "OFF", PRIVATE);
		WiFiSignal sig = WiFi.RSSI();
		Log.info("WiFi signal strength: %.02f%%", sig.getQuality());

		return 1;
	}
	return 0;
}

int changeAntenna(String command){
// These settings are persistent (saved to FLASH) across power settings and
// supposedly even firmware updates.
	int antennaSetting = command.substring(0,1).toInt();
	if (antennaSetting) {
		// set external if 1
		BLE.selectAntenna(BleAntennaType::EXTERNAL);
		Log.info("ExternalAnt");
		return 1;
	} else {
		// set internal if zero
		BLE.selectAntenna(BleAntennaType::INTERNAL);
		Log.info("InternalAnt");
		return 0;
	}
}


