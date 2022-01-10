#pragma once
#include <stdint.h>
#include <boost/asio.hpp>
#include "SerialPort.h"

using namespace boost::asio;

// https://gist.github.com/kaliatech/427d57cb1a8e9a8815894413be337cf9
// https://github.com/fedetft/serial-port

// Serial Communication for WIN32
// https://www.tetraedre.com/advanced/serial2.php
// https ://docs.microsoft.com/en-us/previous-versions/ff802693(v=msdn.10)?redirectedfrom=MSDN


// and for Linux
// https://codereview.stackexchange.com/questions/223731/serial-port-library-for-unix-systems-in-c-using-callback-model


class SerialBoost : public SerialPort {
public:

	SerialBoost();

	bool IsOpen();
	void Close();
	void DiscardInOut();
	void Write(const uint8_t * Data, int offs, int count);
	int Read(uint8_t * Data, int offs, int count);
	int BytesToRead();


	/// <summary>
	/// Get time duration in microseconds  for one received chars
	/// </summary>
	/// <returns>Calculated delay (microseconds)</returns>
	int GetCharTime();

protected:
	bool OpenPort();
	void SetTimeouts();

private:
	boost::asio::io_service io;
	boost::asio::serial_port serial;

	serial_port_base::parity::type boost_parity(SerialPort::Parity Parity)
	{
		switch (Parity) {
		case Parity::Odd: return serial_port_base::parity::odd;
		case Parity::Even: return serial_port_base::parity::even;
		default:
			return serial_port_base::parity::none;
		}
	}

	serial_port_base::stop_bits::type boost_stop_bits(SerialPort::StopBits stopbits)
	{
		switch (stopbits) {
		case StopBits::Two: return serial_port_base::stop_bits::two;
		case StopBits::OnePointFive: return serial_port_base::stop_bits::onepointfive;
		default:
			return serial_port_base::stop_bits::one;
		}
	}
};


