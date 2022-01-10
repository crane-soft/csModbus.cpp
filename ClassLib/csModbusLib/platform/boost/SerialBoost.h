#pragma once
#include <stdint.h>
#include <boost/asio.hpp>
using namespace boost::asio;

// https://gist.github.com/kaliatech/427d57cb1a8e9a8815894413be337cf9
// https://github.com/fedetft/serial-port

// Serial Communication for WIN32
// https://www.tetraedre.com/advanced/serial2.php
// https ://docs.microsoft.com/en-us/previous-versions/ff802693(v=msdn.10)?redirectedfrom=MSDN

// and for Linux
// https://codereview.stackexchange.com/questions/223731/serial-port-library-for-unix-systems-in-c-using-callback-model

// Some Conversions from My Types to boost::asio
class Parity {
public:	
	enum type { None, Odd, Even };
	void Set(type value) {
		switch (value)
		{
		case Parity::Odd:
			Value = serial_port_base::parity::odd;
			break;
		case Parity::Even:
			Value = serial_port_base::parity::even;
			break;
		default:
			Value = serial_port_base::parity::none;
			break;
		}
	}
	serial_port_base::parity::type Value;
};

class StopBits {
public:
	enum type { None, One,Two,	OnePointFive };
	serial_port_base::stop_bits::type Value;
	void Set(type value) {
		switch (value) {
		case StopBits::Two:
			Value = serial_port_base::stop_bits::two;
			break;
		case StopBits::OnePointFive:
			Value = serial_port_base::stop_bits::onepointfive;
			break;
		default:
			Value = serial_port_base::stop_bits::one;
		}
	}
};


class Handshake {
public:
	enum type { None, XOnXOff, RequestToSend, RequestToSendXOnXOff };
	void Set(type value) {
		switch (value)	{
		case Handshake::XOnXOff:
			Value = serial_port_base::flow_control::software;
			break;
		case Handshake::RequestToSend:
			Value = serial_port_base::flow_control::hardware;
			break;
		default:
			Value = serial_port_base::flow_control::none;
			break;
		}
	}
	serial_port_base::flow_control::type Value;

};

class SerialPort {
public:

	SerialPort();
	void SetComParms(const char* PortName, int BaudRate);
	void SetComParms(const char* PortName, int BaudRate, int DataBits, Parity::type Parity, StopBits::type StopBits, Handshake::type Handshake);
	void Open();
	bool IsOpen();
	void Close();
	void Write(const uint8_t * Data, int offs, int count);

	/// <summary>
	/// Get time duration in microseconds  for one received chars
	/// </summary>
	/// <returns>Calculated delay (microseconds)</returns>
	int GetCharTime();

private:
	int BytesToRead;

	const char * _PortName;
	int _BaudRate;
	int _DataBits;
	Parity _Parity;
	StopBits _StopBits;
	Handshake  _Handshake;

	long WriteTimeout ;

private:
	boost::asio::io_service io;
	boost::asio::serial_port serial;

};