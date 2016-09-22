// #ifndef CONN
// #define CONN
//
// #include "SerialExtractor.h"
//
// typedef void (*msg_callback) (char *, char *);
// typedef void (*callback) ();
//
// class Connector:public SerialExtractor{
// private:
//
//   callback on_connection;
//   callback on_disconnection;
//   msg_callback on_config_received;
//   msg_callback on_command_received;
//
//   void on_msg(int a[], int sz)
//
// public:
//   Connector();
//
//   void init();
//   void run();
//
//   void set_on_connection(callback);
//   void set_on_disconnection(callback);
//   void set_on_config(msg_callback);
//   void set_on_command(msg_callback);
//
// };
//
// #endif
