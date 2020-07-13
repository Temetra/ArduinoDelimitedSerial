#include "DelimitedSerial.h"

DelimitedSerial::DelimitedSerial() {
  resetState();
  currentState = DelimitedSerialState::Finished;
}

DelimitedSerial::~DelimitedSerial() {
  delete[] messageBuffer;
  messageBuffer = nullptr;
}

void DelimitedSerial::update(void (*callback)(uint8_t*, uint16_t, void*), void *callback_data) {
  // Check serial port
  if(Serial.available() > 0) {
    
    // Get single byte
    uint8_t input = Serial.read();

    // Process
    if (input == START_MARKER) {
      handleStartMarkerState();
    }
    else if (input == SPLIT_MARKER) {
      handleSplitMarkerState();
    }
    else if (currentState == DelimitedSerialState::StartReadSize) {
      handleStartReadSizeState(input);
    }
    else if (currentState == DelimitedSerialState::FinishReadSize) {
      handleFinishReadSizeState(input);
    }
    else if (currentState == DelimitedSerialState::Processing) {
      handleProcessingState(input, callback, callback_data);
    }
  }
}

void DelimitedSerial::handleStartMarkerState() {
  // Reset state
  resetState();

  // Wait for buffer size bytes
  currentState = DelimitedSerialState::StartReadSize;
}

void DelimitedSerial::handleSplitMarkerState() {
  // Store partial byte
  currentByte = SPLIT_MARKER;
}

void DelimitedSerial::handleStartReadSizeState(uint8_t input) {
  // Store first part of buffer size
  maxMessageLength = 256U * (input + currentByte);
  currentByte = 0;

  // Wait for second half
  currentState = DelimitedSerialState::FinishReadSize;
}

void DelimitedSerial::handleFinishReadSizeState(uint8_t input) {
  // Create buffer
  maxMessageLength += (input + currentByte);
  currentByte = 0;
  messageBuffer = new uint8_t[maxMessageLength];

  // Continue processing
  currentState = DelimitedSerialState::Processing;
}

void DelimitedSerial::handleProcessingState(uint8_t input, void (*callback)(uint8_t*, uint16_t, void*), void *callback_data) {
  // Append byte
  if (messageLength < maxMessageLength) {
    messageBuffer[messageLength] = input + currentByte;
    messageLength += 1;
  }

  // Check if message is complete
  if (messageLength == maxMessageLength) {
    // Send message to callback function
    if (callback) callback(messageBuffer, messageLength, callback_data);

    // Reset state
    resetState();

    // Stop processing
    currentState = DelimitedSerialState::Finished;
  }
  else {
    // Reset current byte
    currentByte = 0;
  }
}

void DelimitedSerial::resetState() {
  currentByte = 0;
  messageLength = 0;
  maxMessageLength = 0;
  delete[] messageBuffer;
  messageBuffer = nullptr;
}
