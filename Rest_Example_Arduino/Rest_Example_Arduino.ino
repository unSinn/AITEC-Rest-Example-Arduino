#include <config_rest.h>
#include <rest_server.h>
#include <SPI.h>
#include <Ethernet.h>
#include <TM1637.h>

// Config of NumberDisplay
#define CLK 2 //pins definitions for TM1637 and can be changed to other ports       
#define DIO 3

#define SERVICES_COUNT	2

// Index of services in "service_get_pins" array
#define SERVICE_LIGHTSENSOR 0
#define SERVICE_DISPLAY 1

#define LIGHTPIN A0

#define CRLF "\r\n"

// Enter a MAC address and IP address for your Arduino below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x68, 0xF8 };
byte ip[] = { 192, 168, 2, 200 };
byte gateway[] = { 192, 168, 2, 1 };
byte subnet[] = { 255, 255, 255, 0 };

// Start a TCP server on port 7999
EthernetServer server(80);

// Create instance of the RestServer
RestServer request_server = RestServer(Serial);

TM1637 tm1637(CLK,DIO);

// method that register the resource_descriptions with the request_server
// it is important to define this array in its own method so that it will
// be discarted from the Arduino's RAM after the registration.
void register_rest_server() {
	resource_description_t lightsensor = { "output_light", false, { 0, 1024 } };
	resource_description_t numberdisplay = { "input_numberdisplay", true, { 0, 9999 } };

	resource_description_t resource_description[SERVICES_COUNT];
	resource_description[SERVICE_LIGHTSENSOR] = lightsensor;
	resource_description[SERVICE_DISPLAY] = numberdisplay;
	request_server.register_resources(resource_description, SERVICES_COUNT);
}

void setup() {
	// start the Ethernet connection and the server:
	Ethernet.begin(mac, ip, gateway, subnet);

	// setup NumberDisplay
	tm1637.init();
	tm1637.set(BRIGHT_TYPICAL); //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

	// manually initialize input and output pins, if not done by a library
	pinMode(LIGHTPIN, INPUT);

	server.begin();

	// register resources with resource_server
	register_rest_server();
}

void loop() {
	// listen for incoming clients
	EthernetClient client = server.available();

	// CONNECTED TO CLIENT
	if (client) {
		while (client.connected()) {

			// get request from client, if available
			if (request_server.handle_requests(client)) {
				read_write_data();
				request_server.respond();	// tell RestServer: ready to respond
			}

			// send data to client, when ready	
			if (request_server.handle_response(client))
				break;
		}
		// give the web browser time to receive the data and close connection
		delay(1);
		client.stop();
	}
}

void read_write_data() {
	for (int j = 0; j < SERVICES_COUNT; j++) {
		if (!request_server.resource_post_enabled(j)) {
			if (j == SERVICE_LIGHTSENSOR) {
				request_server.resource_set_state(j, analogRead(LIGHTPIN));
			}
		}
		if (request_server.resource_post_enabled(j)) {
			if (j == SERVICE_DISPLAY) {
				setNumberDisplay(request_server.resource_get_state(j));
			}
			// Or e.g. for analog PINS
			// analogWrite(service_pins[j], request_server.resource_get_state(j));
		}
	}
}

void setNumberDisplay(int value) {
	tm1637.display(0, (int) (value / 1000));
	tm1637.display(1, (int) (value / 100) % 10);
	tm1637.display(2, (int) (value / 10) % 10);
	tm1637.display(3, value % 10);
}
