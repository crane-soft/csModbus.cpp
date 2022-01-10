#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

#include "SerialBoost.h"

using namespace boost;

SerialBoost::SerialBoost()
	:io(), serial(io)
{
}

bool SerialBoost::IsOpen()
{
	return serial.is_open();
}

void SerialBoost::Close()
{
	serial.close();
}

void SerialBoost::DiscardInOut()
{
	serial.cancel();
}

void SerialBoost::Write(const uint8_t * Data, int offs, int count)
{
	asio::write(serial, asio::buffer(&Data[offs], count));
}

int SerialBoost::Read(uint8_t * Data, int offs, int count)
{
	asio::read(serial, asio::buffer(&Data[offs], count));
	return 0;
}

int SerialBoost::BytesToRead()
{
	return 0;
}

bool SerialBoost::OpenPort()
{
	serial.open(PortName);
	serial.set_option(serial_port_base::baud_rate(BaudRate));
	serial.set_option(serial_port_base::character_size(DataBits));
	serial.set_option(serial_port_base::parity(boost_parity(mParity)));
	serial.set_option(serial_port_base::stop_bits(boost_stop_bits(mStopBits)));
	serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
	return serial.is_open();
}

int SerialBoost::GetCharTime()
{
	int nbits = 1 + DataBits;
	nbits += mParity == SerialPort::Parity::NoParity ? 0 : 1;
	switch (mStopBits) {
	case SerialPort::StopBits::One:
		nbits += 1;
		break;

	case SerialPort::StopBits::OnePointFive: // Ceiling
	case SerialPort::StopBits::Two:
		nbits += 2;
		break;
	}

	return int32_t(ceil(1E6 / ((double) BaudRate / (double)nbits)));
}

