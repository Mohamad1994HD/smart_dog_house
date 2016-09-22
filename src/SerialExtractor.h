#ifndef SERIAL_H
#define SERIAL_H


/*
20 February 2016
Simple Serial extractor for extracting numbers sent to serial port in a sentence
ex. 2:3# where : is the delimeter and # is the end indicator
Use:
  int a[10]={0};
  int siz = 0;

  ser.TryReadSerial();

    if (ser.SERIAL_RESULT == SUCCESS){
      ser.extractInfo(a, siz);
      for (int i=0;i<siz; i++)
      {
        Serial.println(a[i]);
      }

    }

CAUTION: Displaying Error via DisplayError has a problem unknown till now
*/
#include "Arduino.h"
enum MsgState {SUCCESS, ERROR_MSG_NOT_TERMINATED, WAITING, ERROR_IN_DELIMETER};
typedef void (*call_back) (int a[], int size);

class SerialExtractor{
  private:
	//char delimeter;
	char endIndicator;
	char const*  delimeter;
    char cmd[100];
    int siz;
	call_back callBack;

  public :
	SerialExtractor(char end='#', char const* deli=":", call_back func=NULL);

    MsgState SERIAL_RESULT;

    void TryReadSerial();
    void extractInfo(int *,int &);
	void Run();

	void resetCommand();
	void DisplayError();

	void SetCallBack(call_back);
	void SetEndIndicator(char );
	char GetEndIndicator();
	void SetDelimeter(char const* );
	char const* GetDelimeter();



};

#endif
