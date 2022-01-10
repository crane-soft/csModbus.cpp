#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

#include "SerialBoost.h"

using namespace boost;

SerialPort::SerialPort()
	:io(), serial(io)
{
}

void SerialPort::SetComParms(const char* PortName, int BaudRate)
{
	SetComParms(PortName, BaudRate, 8, Parity::None, StopBits::One, Handshake::None);
}

void SerialPort::SetComParms(const char* PortName, int BaudRate, int DataBits, Parity::type Parity, StopBits::type StopBits, Handshake::type Handshake)
{
	_PortName = PortName;
	_BaudRate = BaudRate;
	_DataBits = DataBits;;
	_Parity.Set(Parity);
	_StopBits.Set(StopBits);
	_Handshake.Set(Handshake);
}

void SerialPort::Open()
{
	if (IsOpen) {
		serial.close();
	}
	serial.open(_PortName);
	serial.set_option(serial_port_base::baud_rate(_BaudRate));
	serial.set_option(serial_port_base::character_size(_DataBits));
	serial.set_option(serial_port_base::parity(_Parity.Value));
	serial.set_option(serial_port_base::stop_bits(_StopBits.Value));
	serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
}

int SerialPort::GetCharTime()
{
	int nbits = 1 + _DataBits;
	nbits += _Parity.Value == Parity::None ? 0 : 1;
	switch (_StopBits.Value) {
	case StopBits::One:
		nbits += 1;
		break;

	case StopBits::OnePointFive: // Ceiling
	case StopBits::Two:
		nbits += 2;
		break;
	}

	return int32_t(ceil(1E6 / ((double)_BaudRate / (double)nbits)));
}

bool SerialPort::IsOpen()
{
	return serial.is_open();
}

void SerialPort::Close()
{
	if (IsOpen) {
		serial.close();
	}
}

void SerialPort::Write(const uint8_t * Data, int offs, int count)
{
	asio::write(serial, asio::buffer(&Data[offs], count));
}
