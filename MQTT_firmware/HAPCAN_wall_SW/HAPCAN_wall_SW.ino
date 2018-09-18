#include "Arduino.h"
#include <SPI.h>
#include <EEPROM.h>
#include <mcp_can.h>
#include <OnixartsIO.h>
#include <OnixartsTaskManager.h>
#include "HapcanDevice.h"
#include "HapcanRelay.h"
#include "HapcanButton.h"

using namespace Onixarts::HomeAutomationCore;
using namespace Onixarts::Tools;

Hapcan::HapcanDevice hapcanDevice;


// Configure Your Hapcan device here
const byte Hapcan::Config::MCP::InterruptPin = 2;        // CAN module interrupt is connected to this pin (see https://www.arduino.cc/en/Reference/AttachInterrupt)
const byte Hapcan::Config::MCP::CSPin = 10;           // SPI CS pin
const byte Hapcan::Config::MCP::OscillatorFrequency = MCP_8MHZ; // MCP oscillator frequency on MCP CAN module (or MCP_16MHz)

const byte Hapcan::Config::Hardware::DeviceId1 = 0x03;      // unique device identifier 1, change it
const byte Hapcan::Config::Hardware::DeviceId2 = 0x00;      // unique device identifier 2, change it



const byte Hapcan::Config::Node::SerialNumber0 = 0;       // ID0 serial number MSB
const byte Hapcan::Config::Node::SerialNumber1 = 0;
const byte Hapcan::Config::Node::SerialNumber2 = 0x12;      // this is also a default node
const byte Hapcan::Config::Node::SerialNumber3 = 0x14;       // this is also a default group

const byte Hapcan::Config::Firmware::ApplicationType = 2;    // application (hardware) type (such as button, relay, dimmer) 1-10 Hapcan modules, 102 - ethernet, 51 - Hapcanuino Hellow World 1 LED device
const byte Hapcan::Config::Firmware::ApplicationVersion = 1;  // application (hardware) version, change it with device hardware changes
const byte Hapcan::Config::Firmware::FirmwareVersion = 0;   // firmware version
const int  Hapcan::Config::Firmware::FirmwareRevision = 2;    // firmware revision
// Configuration end

void ExecuteInstruction(byte instruction, byte param1, byte param2, byte param3, Hapcan::HapcanMessage& message);
void OnStatusRequest(byte requestType, bool isAnswer);

Hapcan::SubModule::HapcanRelay::Module out1(hapcanDevice, 1, PIN7, 0x00);
Hapcan::SubModule::HapcanRelay::Module out2(hapcanDevice, 2, PIN6, 0x00);
Hapcan::SubModule::HapcanRelay::Module out3(hapcanDevice, 3, PIN5, 0x00);
Hapcan::SubModule::HapcanRelay::Module out4(hapcanDevice, 4, PIN4, 0x00);


//  Module(Hapcan::HapcanDevice& device, byte channel, byte outputPin, byte instructionShift)
//              : Hapcan::SubModuleBase(instructionShift)
//              , IO::DigitalOutput(outputPin)
//              , m_channel(channel)
//, m_device(device) 

Hapcan::SubModule::HapcanButton::Module  button1(hapcanDevice, 5, 9, 0x00);
//Hapcan::SubModule::HapcanButton::EventNotification

//Module(Hapcan::HapcanDevice& device, byte channel, byte inputPin, bool enablePullUpResistor = false, byte inputActiveLevel = HIGH/*, byte instructionShift*/)
//              : Hapcan::SubModuleBase(0)
//              , IO::DigitalInput(inputPin, enablePullUpResistor, inputActiveLevel)
//              , m_channel(channel)
//              , m_device(device) 
//              , m_standardEventNotification(0xFF)
//, m_extendedEventNotification(0xFF)




void setup()
{
    Serial1.begin(115200);
    Serial1.println("Hapcanuino device starting...");

    hapcanDevice.Begin();

    hapcanDevice.SetExecuteInstructionDelegate(ExecuteInstruction);
    hapcanDevice.SetStatusRequestDelegate(OnStatusRequest);

    // submodules
    out1.Init();
    out2.Init();
    out3.Init();
    out4.Init();
    button1.Init();
}


void loop()
{
    hapcanDevice.Update();

    out1.Update();
    out2.Update();
    out3.Update();
    out4.Update();
    button1.Update();
}

void ExecuteInstruction(byte instruction, byte param1, byte param2, byte param3, Hapcan::HapcanMessage& message)
{
    out1.ExecuteInstruction(instruction, param1, param2, param3, message);
    out2.ExecuteInstruction(instruction, param1, param2, param3, message);
    out3.ExecuteInstruction(instruction, param1, param2, param3, message);
    out4.ExecuteInstruction(instruction, param1, param2, param3, message);
}

void OnStatusRequest(byte requestType, bool isAnswer)
{
    out1.SendStatus(isAnswer);
    out2.SendStatus(isAnswer);
    out3.SendStatus(isAnswer);
    out4.SendStatus(isAnswer);
}


class My4OutputRelayDevice : public Hapcan::HapcanDeviceSubModuleHost<4>
{
  // submodules declaration
  Hapcan::SubModule::HapcanRelay::Module out1;
  Hapcan::SubModule::HapcanRelay::Module out2;
  Hapcan::SubModule::HapcanRelay::Module out3;
    Hapcan::SubModule::HapcanRelay::Module out4;

public:
  My4OutputRelayDevice()
    : out1(*this, 1, PIN7, 0x00)
    , out2(*this, 2, PIN6, 0x00)
    , out3(*this, 3, PIN5, 0x00)
        , out4(*this, 4, PIN4, 0x00)
  {
        // add SubModules to the host
    m_subModules[0] = &out1;
    m_subModules[1] = &out2;
    m_subModules[2] = &out3;
    m_subModules[3] = &out4;
  }
};



