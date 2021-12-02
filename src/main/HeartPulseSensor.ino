///*
//   Every Sketch that uses the PulseSensor Playground must
//   define USE_ARDUINO_INTERRUPTS before including PulseSensorPlayground.h.
//   Here, #define USE_ARDUINO_INTERRUPTS false tells the library to
//   not use interrupts to read data from the PulseSensor.
//
//   If you want to use interrupts, simply change the line below
//   to read:
//     #define USE_ARDUINO_INTERRUPTS true
//
//   Set US_PS_INTERRUPTS to false if either
//   1) Your Arduino platform's interrupts aren't yet supported
//   by PulseSensor Playground, or
//   2) You don't wish to use interrupts because of the side effects.
//
//   NOTE: if US_PS_INTERRUPTS is false, your Sketch must
//   call pulse.sawNewSample() at least once every 2 milliseconds
//   to accurately read the PulseSensor signal.
//*/
//
///*
//   samplesUntilReport = the number of samples remaining to read
//   until we want to report a sample over the serial connection.
//
//   We want to report a sample value over the serial port
//   only once every 20 milliseconds (10 samples) to avoid
//   doing Serial output faster than the Arduino can send.
//*/
//
//void initPulseSensor() {
//  
//}
