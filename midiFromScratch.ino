#include <CapacitiveSensor.h>

#include <RGBLed.h>
#include <MIDI.h>

int capSend = 4;
int capRec = 38;
CapacitiveSensor cap = CapacitiveSensor(4,38); 

int button1 = 52;
int button2 = 50;
int button3 = 48;
int button4 = 46;
int button5 = 44;
int potPin = A0;
int joyX = A8;
int joyY = A7;
int switchPin = 22;
boolean pastbt1 = false;
boolean pastbt2 = false;
boolean pastbt3 = false;
boolean bt1= false;
boolean bt2= false;
boolean bt3= false;
boolean bt4= false;
boolean bt5= false;

RGBLed led(9, 10, 11, RGBLed::COMMON_CATHODE); 
int color = 4;
int palette[][3] = {{252, 44, 3},{252, 152, 3},{252, 252, 3},{148, 252, 3},{3, 252, 144},{3, 240, 252},{3, 78, 252},{128, 3, 252},{252, 3, 235},{252, 3, 132},{252, 3, 3}};

MIDI_CREATE_DEFAULT_INSTANCE();
int note1= 60;
int note2= 61;
int note3= 62;
int note4= 63;
int delay_ms= 50;
int PotVal = analogRead(potPin);
int velocity = 127;
int modifier =0;
boolean useModifier = false;
int pastModifier = 0;
boolean useChord = false;
boolean pastUseModifier = false;
boolean autoPlay = false;
int state = 0;

void setup() {
  // put your setup code here, to run once:
  MIDI.begin(MIDI_CHANNEL_OFF);
  Serial.begin(115200);
  led.brightness(palette[color],10);
}

void loop() {
  // put your main code here, to run repeatedly:
  pastbt1 = bt1;
  pastbt2 = bt2;
  pastbt3 = bt3;

  bt1= digitalRead(button1);
  bt2= digitalRead(button2);
  bt3= digitalRead(button3);
  bt4= digitalRead(button4);
  bt5= digitalRead(button5);
  int PotVal = analogRead(potPin);
  int joyXval = analogRead(joyX);
  int joyYval = analogRead(joyY);
  // long touch = cap.capacitiveSensor(1);
  boolean switchRead = !digitalRead(switchPin);

  if ((joyYval > 700 || joyYval < 400) && ((bt1 && !pastbt1)||(bt2 && !pastbt2)||(bt3 && !pastbt3))){
    modulate(joyYval);
  } else {
    useModifier = false;
  }
  if (switchRead == true){
    changeState();
  }


  if(bt5 && !bt4){
    octave(true);
  }
  else if(bt4 && !bt5){
    octave(false);
  }
  else if (bt4 && bt5){
    delaySwitch(PotVal);
  } else{
    velocity = map(PotVal, 0, 1023, 0, 127);
  }


  if (bt1){
    sendNote(note1, velocity, 1,delay_ms,pastbt1);
  } else if(!bt1 && pastbt1){
    stopHolding(note1,velocity,1,delay_ms,pastbt1);
  }
  if (bt2){
    sendNote(note2,velocity,1,delay_ms,pastbt2);
  } else if (!bt2 && pastbt2){
    stopHolding(note2,velocity,1,delay_ms,pastbt2);
  }
  if (bt3){
    sendNote(note3,velocity,1,delay_ms,pastbt3);
  } else if(!bt3 && pastbt3){
    stopHolding(note3,velocity,1,delay_ms,pastbt3);
  }
  // if (touch> 30){
  //   if (touch > 300){
  //     sendNote(note4+1,velocity,1,delay_ms);
  //   }else{
  //     sendNote(note4,velocity,1,delay_ms);
  //   }
}

void sendNote(int note, int vel, int channel, int note_delay, boolean past){
  if (useModifier){
    note = note + modifier;
    pastUseModifier = true;
    pastModifier = modifier;
  }
  if (past){
    // MIDI.sendNoteOn(note, vel, channel);
    // delay(note_delay);
  } else{

  MIDI.sendNoteOff(note, vel, channel);

  if (useChord){
    MIDI.sendNoteOn(note, vel, channel);
    MIDI.sendNoteOn(note+4, vel, channel+1);
    MIDI.sendNoteOn(note+7, vel, channel+2);
    // delay(note_delay);
    // MIDI.sendNoteOff(note, vel, channel);
    // MIDI.sendNoteOff(note+4, vel, channel+1);
    // MIDI.sendNoteOff(note+7, vel, channel+2);

    // delay(note_delay);
  } else{
    MIDI.sendNoteOn(note, vel, channel);
    // delay(note_delay);
    // MIDI.sendNoteOff(note, vel, channel);
    // delay(note_delay);
  }
  }
}

void stopHolding (int note, int vel, int channel, int note_delay, boolean past){
  if (useModifier){
    note = note + modifier;
  } else if (pastUseModifier){
    note= note + pastModifier;
    pastUseModifier = false;
  }
  MIDI.sendNoteOff(note, vel, channel);
  if (useChord){
    MIDI.sendNoteOff(note+4, vel, channel+1);
    MIDI.sendNoteOff(note+7, vel, channel+2);
  }
}

void octave(boolean up){
  //true if up octave, false if down
  if (up){
    note1 = note1+ 12;
    note2 = note2+12;
    note3 = note3+12;
    note4 = note4+12;
    if (note3 > 127){
      note1 = 0;
      note2 = 1;
      note3 = 2;
      note4 = 3;
    }
    color = color + 1;
    if (color>10){
      color = 0;}
    led.brightness(palette[color],20);
    delay(200);
  }
  else{
    note1 = note1- 12;
    note2 = note2-12;
    note3 = note3-12;
    note4 = note4-12;
    if (note1 < 0){
      note1 = 120;
      note2 = 121;
      note3 = 122;
      note4 = 123;
    }
    color = color - 1;
    if (color<0){
      color = 10;}
    led.brightness(palette[color],20);
    delay(200);
  }
}

void modulate (int y){
  modifier = map(y, 0, 1023, 4, -4);
  useModifier = true;
}

void delaySwitch(int PotVal){
    delay_ms = map(PotVal, 0, 1023, 500, 10);
    led.off();
    delay(delay_ms);
    led.brightness(palette[color],20);
    delay(delay_ms);
}
void changeState(){
  state = state + 1;
    if(state > 3){
      state = 0;
    }
    if (state == 0){
      useChord = false;
      autoPlay = false;
    } else if (state == 1){
      useChord = true;
      autoPlay = false;
    } else if(state == 2){
      useChord = false;
      autoPlay = true;
    } else{
      useChord = true;
      autoPlay = true;
    }
    led.brightness(palette[0],20);
    delay(200);
    led.brightness(palette[color],20);
}
