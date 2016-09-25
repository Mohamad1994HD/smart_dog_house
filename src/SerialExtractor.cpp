#include "SerialExtractor.h"

SerialExtractor::SerialExtractor(SoftwareSerial &s, char end,char const* deli, call_back function):
serial_obj(s),endIndicator(end), delimeter(deli), callBack(function)
{
 resetCommand();

 this->SERIAL_RESULT = WAITING;
}

void SerialExtractor::init(long baudrate){
  serial_obj.begin(baudrate);
}

void SerialExtractor::TryReadSerial()
{
 char serialInByte;
 bool isData = false;
 if (serial_obj.available())
 {
 isData = true;
   do
   {
     serialInByte = serial_obj.read();
     //this->command += serialInByte;

     this->cmd[siz] = serialInByte;
     siz++;

    // Serial.println(this->cmd);
  } while ((serialInByte != endIndicator) && serial_obj.available());
 }
this->cmd[this->siz] = '\0';

if(isData)
{
   if (serialInByte == endIndicator)
   {
   this->SERIAL_RESULT = SUCCESS;
   }
   else
   {
   this->SERIAL_RESULT = ERROR_MSG_NOT_TERMINATED;
   //Serial.println("ERROR MSG NOT TERMINATED WITH : "+this->endIndicator);
   }
}
else
{
 this->SERIAL_RESULT = WAITING;
}
Serial.flush();
}

void SerialExtractor::resetCommand()
{
 for (int i=0;i<siz;i++)
 {
   this->cmd[i]='\0';
 }
  this->siz = 0;
}


void SerialExtractor::extractInfo(int a[], int &size)
{
 char* token = strtok(cmd, delimeter);

 if(token)
 {
   while(token)
   {
     a[size] = atoi(token);
     size++;
     token = strtok(NULL, delimeter);
     /*if(!token  )
     {
       SERIAL_RESULT = ERROR_IN_DELIMETER;
       break;
     }*/
     //Serial.print(a[size]); Serial.print(" "); Serial.println(size);
   }
 }
 /*else
 {
   SERIAL_RESULT = ERROR_IN_DELIMETER;
 }

 if (SERIAL_RESULT == ERROR_IN_DELIMETER)
 {
   Serial.print("ERROR VALID DELIMETER NOT USED: ");
   Serial.println(this->delimeter);
 }*/

 resetCommand();
}

void SerialExtractor::Run()
{
 if(callBack == NULL)
 {
   return;
 }

 int a[10] = {0};
 int size = 0;

 TryReadSerial();
 if(SERIAL_RESULT == SUCCESS)
 {
   extractInfo(a, size);
   callBack(a, size);
 }
}


void SerialExtractor::DisplayError()
{
 if(SERIAL_RESULT == ERROR_MSG_NOT_TERMINATED)
 {
   Serial.print("ERROR MSG NOT TERMINATED WITH ");
     Serial.println(GetEndIndicator());
 }
}

void SerialExtractor::SetEndIndicator(char c)
{
 this->endIndicator = c;
}

void SerialExtractor::SetDelimeter(char const* c)
{
 this->delimeter = c;
}

char SerialExtractor::GetEndIndicator()
{
 return this->endIndicator;
}
char const* SerialExtractor::GetDelimeter()
{
 return this->delimeter;
}
void SerialExtractor::SetCallBack(call_back func)
{
   this->callBack = func;
}
