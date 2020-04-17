#ifndef DelimitedSerial_h
#define DelimitedSerial_h
#include "Arduino.h"

// Messages must begin with a byte marker of 255
// Followed by two bytes declaring the size (uint16_t) of the original message (big-endian)
// Non-marker bytes between 254-255 must be split into two: (254) and (n-254)

enum class DelimitedSerialState { StartReadSize, FinishReadSize, Processing, Finished };

class DelimitedSerial {
  public:
    // Callback takes pointer to message buffer, and length of message
    DelimitedSerial(void (*callback)(uint8_t*, uint16_t));
    ~DelimitedSerial();
  
    // Processes a single byte
    // Sends buffer to callback function when message is complete
    void update();

  private:
    // Values of delimiting bytes
    static const uint8_t START_MARKER = 255;
    static const uint8_t SPLIT_MARKER = 254;

    // Size of message
    uint16_t messageLength;

    // Maximum size of message
    uint16_t maxMessageLength;

    // Pointer to message buffer
    uint8_t * messageBuffer;

    // Accumulated value for split bytes
    uint8_t currentByte;

    // Current state of message processing
    DelimitedSerialState currentState;

    // Pointer to callback to send complete message to
    void (*msgReadyCallback)(uint8_t*, uint16_t);

    // Member functions
    void resetState();
    void handleStartMarkerState();
    void handleSplitMarkerState();
    void handleStartReadSizeState(uint8_t input);
    void handleFinishReadSizeState(uint8_t input);
    void handleProcessingState(uint8_t input);
};

#endif
