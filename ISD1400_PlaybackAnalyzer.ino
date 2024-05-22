const int AD0 = 4;  // Specify address pins.
const int AD1 = 5;
const int AD2 = 6;
const int AD3 = 7;
const int AD4 = 8;
const int AD5 = 9;
const int AD6 = 10;
const int AD7 = 11;

const int LEVEL = 2;  // Specify playback pins.
const int EDGE = 3;

const int RECLED = 14;  // Specify simulated RECLED pin.

int levelstate = 0;  // For code operation
int edgestate = 0;

int triggerededge = 0;  // for RECLED simulation
unsigned long ledval = 0;
int ledsimmed = 0;

int address = 0;  // For checkAddressPins()

int statechanged = 0;  // For code operation

void setup() {
  Serial.begin(9600);  // Initialize serial communication

  digitalWrite(RECLED, HIGH);
  pinMode(RECLED, OUTPUT);  // Initialize simulated RECLED output

  pinMode(LEVEL, INPUT_PULLUP);  // Initialize control input pins
  pinMode(EDGE, INPUT_PULLUP);   // Note:  "INPUT_PULLUP" was used for testing using pushbuttons without pull-up resistors, it can be changed to "INPUT" for actual use.

  pinMode(AD0, INPUT);  // Initialize address pin inputs
  pinMode(AD1, INPUT);
  pinMode(AD2, INPUT);
  pinMode(AD3, INPUT);
  pinMode(AD4, INPUT);
  pinMode(AD5, INPUT);
  pinMode(AD6, INPUT);
  pinMode(AD7, INPUT);

  edgestate = digitalRead(EDGE);  // initialize pin states
  levelstate = digitalRead(LEVEL);
}

void loop() {
  if (digitalRead(LEVEL) != levelstate) {  // Check control pins
    levelstate = digitalRead(LEVEL);
    statechanged = 1;
  }
  if (digitalRead(EDGE) != edgestate) {
    edgestate = digitalRead(EDGE);
    statechanged = 1;
  }

  if (statechanged == 1) {
    statechanged = 0;
    checkAddressPins();  // Print message, prepare RECLED simulation
    Serial.print("Address: ");
    Serial.print(address);
    Serial.print(", Pin type: ");
    if (levelstate == 0) {
      Serial.println("LEVEL");
      ledval = millis();
    } else if (edgestate == 0) {
      Serial.println("EDGE");
      if (address <= 160) {
        triggerededge = 1;
      }
      ledval = millis();
    } else {
      Serial.println("RELEASE");
    }
    if (address <= 160) {
      Serial.print("Playing sound starting at address ");
      Serial.print(address);
      Serial.println(".");
    } else if (address >= 192) {
      Serial.print("Special function: ");
      if (address == 193) {
        Serial.println("Message cueing");
      } else if (address == 194) {
        Serial.println("Remove EOM marker");
      } else if (address == 200) {
        Serial.print("Message looping");
        if (levelstate == 0) {
          Serial.println(" off.");
        } else if (edgestate == 0) {
          Serial.println(" on.");
        } else {
          Serial.println(".");
        }
      } else if (address == 208) {
        Serial.println("Consecutive addressing.");
      } else {
        Serial.println("Unused function.");
      }
    } else {
      Serial.println("Does nothing.");
    }
  }
  if ((levelstate == 1) and (triggerededge == false)) {
    ledsimmed = 0;
  }
  unsigned long currentval = millis();  // RECLED simulation
  if ((currentval - 2000 >= ledval) and ((triggerededge == 1) or (levelstate == 0)) and (ledsimmed == 0)) {
    triggerededge = 0;
    ledsimmed = 1;
    digitalWrite(RECLED, LOW);
    delay(15);
    digitalWrite(RECLED, HIGH);
  }
}

void checkAddressPins() {
  address = 0;                  // Set the address number to 0 before calculating
  int ad0v = digitalRead(AD0);  // Read the input values
  int ad1v = digitalRead(AD1);
  int ad2v = digitalRead(AD2);
  int ad3v = digitalRead(AD3);
  int ad4v = digitalRead(AD4);
  int ad5v = digitalRead(AD5);
  int ad6v = digitalRead(AD6);
  int ad7v = digitalRead(AD7);
  address = address + ad0v;      // Add up the address values
  address = address + ad1v * 2;  // This probably isn't the best way to do this
  address = address + ad2v * 4;
  address = address + ad3v * 8;
  address = address + ad4v * 16;
  address = address + ad5v * 32;
  address = address + ad6v * 64;
  address = address + ad7v * 128;
}
