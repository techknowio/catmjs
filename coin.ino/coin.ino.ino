/*
  This program expects pulses of logic high coming from a bill acceptor into a GPIO pin on an arduino leonardo or anything with an Atema32u4 and an arduino compatible boot loader.
  It counts the number of pulses, pretends to be a usb keyboard and types the dollar amount followed by a newline after each bill has been accepted e.g: "5.00\n"
*/

// The pin on the arduino where CREDIT (-) [Common] is connected
#define INPIN (9)

int cents_per_pulse; // how many cents per pulse. for most bill acceptors this is 100 or $1 per pulse, but it can often be configured and coin acceptors will be different
int min_pulse_width; // the minimum pulse width to acccept
int max_pulse_width; // the maximum pulse width to accept
int debounce_speed; // ignore changes in input line state that happen faster than this
int pulse_count; // how many pulses have been received so far in this pulse train
int cents_received; // Counts how many cents have been received
unsigned long pulse_duration; // how long was the last pulse
unsigned long pulse_begin; // when did the last pulse begin
unsigned long pulse_end; // if they pulse was within min and max pulse width, when did it end
unsigned long curtime; // what is the current time
int post_pulse_pause; // how long to wait after last pulse before sending pulse count
int pulse_state; // what is the current input line state (1 for high, 0 for low)
int last_state; // what was the last input line state
int whole_dollars; // how many whole dollars were received
int remaining_cents; // how many remaining cents were received
char out_str[8];

void setup() {
  pinMode(INPIN, INPUT);
  Serial.begin(9600); // You can comment all the Keyboard lines and uncomment all the serial lines to make it print to serial instead (useful for debugging)
  //Keyboard.begin();
  pulse_begin = 0;
  last_state = 0;
  min_pulse_width = 40;
  max_pulse_width = 60;
  debounce_speed = 4;
  post_pulse_pause = 300;
  cents_per_pulse = 100;
  pulse_end = 0;
  pulse_count = 0;
  cents_received = 0;
}

void loop() {
  pulse_state = digitalRead(INPIN);
  curtime = millis();
  if((pulse_state == 1) && (last_state == 0)) { // this means we entered a new pulse
    pulse_begin = curtime; // save the begin time of the pulse
    last_state = 1; // set the previous state
  } else if((pulse_state == 0) && (last_state == 1)) { // this means a pulse just ended
    pulse_duration = curtime - pulse_begin; // calculate pulse duration
    if(pulse_duration > debounce_speed) { // ensure that we don't change state for very short (false) pulses (this is called debouncing)
      last_state = 0;
    }
    if((pulse_duration > min_pulse_width) && (pulse_duration < max_pulse_width)) { // check if the pulse width is between the minimum and maximum
      pulse_end = curtime; // save the end time of the pulse
      pulse_count++; // increment the pulse counter
      Serial.println(1);
    }
  }
  
  if((pulse_end > 0) && (curtime - pulse_end > post_pulse_pause)) { // check if we've waited long enough that we don't expect any further pulses to be forthcoming

    cents_received += pulse_count;
    Serial.println(cents_received);
    cents_received = 0; // reset cents_received so it's ready for next payment

    pulse_end = 0;
    pulse_count = 0;
  }

}
