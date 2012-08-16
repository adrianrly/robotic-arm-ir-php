#include <IRremote.h>
#include <Servo.h>

Servo umarRot, umar, cot, manaVert, manaRot, gripper;

signed int servo,pos; 

byte umarRotPos; 
byte umarPos;
byte cotPos;
byte manaVertPos;
byte manaRotPos;
byte gripperPos;

boolean standby = false;
byte function = 3; // mod de functionare 1 - IR, 2 - PHP, 3 - IR+PHP
byte multiplicator = 3;

IRrecv irrecv(7); //pinul 7 - Receptorul IR
decode_results valIR, valIRRaw;

int userInput[3];    // bufferul de intrare, 3 biti
int startbyte;       // bitul de inceput

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void StreamPrint_progmem(Print &out,PGM_P format,...)
{
  //http://www.utopiamechanicus.com/article/low-memory-serial-print/
  // program memory version of printf - copy of format string and result share a buffer
  // so as to avoid too much memory use
  char formatString[128], *ptr;
  strncpy_P( formatString, format, sizeof(formatString) ); // copy in from program mem
  // null terminate - leave last char since we might need it in worst case for result's \0
  formatString[ sizeof(formatString)-2 ]='\0';
  ptr=&formatString[ strlen(formatString)+1 ]; // our result buffer...
  va_list args;
  va_start (args,format);
  vsnprintf(ptr, sizeof(formatString)-1-strlen(formatString), formatString, args );
  va_end (args);
  formatString[ sizeof(formatString)-1 ]='\0';
  out.print(ptr);
}
 
#define Serialprint(format, ...) StreamPrint_progmem(Serial,PSTR(format),##__VA_ARGS__)
#define Streamprint(stream,format, ...) StreamPrint_progmem(stream,PSTR(format),##__VA_ARGS__)
 

void setup()
{
  Serial.begin(9600);
  
  umarRot.attach(13); //pin 13 - Rotatie umar
  umar.attach(12); //pin 12 - Umar
  cot.attach(11);//pin 11 - Cot
  manaVert.attach(10); //pin 10 - Mana vertical
  manaRot.attach(9); //pin 9 - Rotatie umar
  gripper.attach(8); //pin 8 - Gripper
    
  umarRot.write(0); //calibram servo-ul la 0.
  cot.write(20);
  umar.write(30);  //calibram umarul(2 servouri) la 30
  manaVert.write(0);
  manaRot.write(0);
  gripper.write(0);
  
  irrecv.enableIRIn(); // Start the receiver
  Serialprint("Memorie ram disponibila: %d bytes\n",freeRam());
  Serialprint("  ===  Begin debugger  ===  \n");
}

void loop() {
  if (irrecv.decode(&valIRRaw)) { //citim tasta
    if(valIRRaw.value != 0xFFFFFFFF) valIR = valIRRaw; //daca nu este tasta tinuta apasata, stocam ultima valoare
    if(valIRRaw.value == 0x8B7F807) standbySwitch();
    decodare(valIR);
    irrecv.resume(); // absolut necesar pentru primirea valorilor ulterioare
  }
  php2Serial();
}

void decodare(decode_results valIR)
{
    if(!standby)
        switch(valIR.value){
            case 0: Serial.println("Eroare de comunicatie"); break;
            case 0x8B7D22D: multiDown(); 
							break;
            case 0x8B752AD: multiUp(); 
							break;
            //Servo-uri
            //1 umarRot
            case 0x8B7E01F: umarRotPos = posUp(umarRotPos); 
							servoMod(umarRot,umarRotPos,1); 
							break;
            case 0x8B7A25D: umarRotPos = posDown(umarRotPos); 
							servoMod(umarRot,umarRotPos,1); 
							break;
            //2 umar
            case 0x8B70AF5: umarPos = posUp(umarPos); 
							servoMod(umar,umarPos,2); 
							break;
            case 0x8B7C837: umarPos = posDown(umarPos); 
							servoMod(umar,umarPos,2); 
							break;
            //3 cot
            case 0x8B702FD: cotPos = posUp(cotPos); 
							servoMod(cot,cotPos,3); 
							break;
            case 0x8B7827D: cotPos = posDown(cotPos); 
							servoMod(cot,cotPos,3); 
							break;
            //4 manaVert
            case 0x8B73AC5: manaVertPos = posUp(manaVertPos); 
							servoMod(manaVert,manaVertPos,4); 
							break;
            case 0x8B77887: manaVertPos = posDown(manaVertPos); 
							servoMod(manaVert,manaVertPos,4); 
							break;
            //5 manaRot
            case 0x8B71AE5: manaRotPos = posUp(manaRotPos); 
							servoMod(manaRot,manaRotPos,5); 
							break;
            case 0x8B758A7: manaRotPos = posDown(manaRotPos); 
							servoMod(manaRot,manaRotPos,5); 
							break;
            //6 gripper
            case 0x8B7D827: gripperPos = posUp(gripperPos); 
							servoMod(gripper,gripperPos,6); 
							break;
            case 0x8B79A65: gripperPos = posDown(gripperPos); 
							servoMod(gripper,gripperPos,6); 
							break;
            //Standby
            case 0x8B7926D: status(); 
							break;
         }
        else Serial.println("Sistemul este in standby");
}
void miscaServo(decode_results valIR)
{
}

void multiDown()
{
  if(multiplicator > 1) multiplicator--;
  Serialprint("Am setat multiplicatorul la %d\n",multiplicator);
}
void multiUp()
{
  if(multiplicator < 20) multiplicator++;
  Serialprint("Am setat multiplicatorul la %d\n",multiplicator);
}

void servoMod(Servo servoIR, int posIR, int id)
{
  servoIR.write(posIR);
  Serialprint("Pozitia servo-ului #%d este la %d grade\n",id,posIR);
  //Serial.println(gripper.read());
}


int posUp(int posIR){
  posIR += multiplicator*3;
  if(posIR > 180) posIR = 180;
  return posIR;
}
int posDown(int posIR){
  posIR -= multiplicator*3;
  if(posIR < 0) posIR = 0;
  return posIR;
}

void status(){
Serial.println(" ===  BEGIN FEEDBACK ===  ");
Serial.print("Servo #1 - Rotatie umar se afla la "); Serial.println(umarRot.read());
Serial.print("Servo #2 - Umar se afla la "); Serial.println(umar.read());
Serial.print("Servo #3 - Cot se afla la "); Serial.println(cot.read());
Serial.print("Servo #4 - Miscare mana verticala se afla la "); Serial.println(manaVert.read());
Serial.print("Servo #5 - Rotatie mana se afla la "); Serial.println(manaRot.read());
Serial.print("Servo #6 - Gripper se afla la "); Serial.println(gripper.read());
Serial.println(" ===  END FEEDBACK  ===  ");
}

void standbySwitch()
{
  if(!standby) {standby = true; Serialprint("Mod standby activ. Apasat Power pentru deblocare.\n");}
    else {standby = false; Serialprint("Sistemul a fost deblocat.\n");}
}

void php2Serial()
{
	//Sistemul este in standby?
	if(!standby)
	  // Asteptam intrarea seriala (minim 3 octeti in buffer) 
	  if (Serial.available() > 2) {
		// citim primul octet
		startbyte = Serial.read();
		// verificam daca octetul de inceput este cel care ne intereseaza
		if (startbyte == 255) 	
		  // ... si urmatorii 2 octeti
		  for (int i=0;i<2;i++) {
			userInput[i] = Serial.read();
		  }
		  // primul octet - selectia servo-ului
		  servo = userInput[0];
		  // al 2-lea octet - pozitia servo-ului
		  pos = userInput[1];
		  // verificam daca nu cumva exista o problema de comunicatie
		  if (pos == 255) { servo = 255; }

		  // in functie de servo, schimbam pozitia
		  switch (servo) {
			case 0:
			  break;
			case 1:
			  umarRot.write(pos);
			  umarRotPos = umarRot.read();
			  break;
			case 2:
			  umar.write(pos);
			  umarPos = umar.read();
			  break;
			case 3:
			  cot.write(pos);
			  cotPos = cot.read();
			  break;
			case 4:
			  manaVert.write(pos);
			  manaVertPos = manaVert.read();
			  break;
			case 5:
			  manaRot.write(pos);
			  manaRotPos = manaRot.read();
			  break;
			case 6:
			  gripper.write(pos);
			  gripperPos = gripper.read();
			  break;
		  }
		}
	  }
	else Serial.println("Sistemul este in standby");
}
