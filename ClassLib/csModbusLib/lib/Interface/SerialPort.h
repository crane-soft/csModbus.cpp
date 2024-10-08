#pragma once
#include <stdint.h>

class SerialCallBack {
public:
	virtual void DataReceived(int bytesAvailavle) {}
};

class SerialPort {

public:
	enum class Parity { NoParity, Odd, Even } ;
	enum class StopBits { NoStopBits, One, Two, OnePointFive } ;

	SerialPort();
	SerialPort(const void* _ComPort, int _BaudRate);

	void SetComParms(const void* _ComPort, int _BaudRate);
	void SetComParms(const void* _ComPort, int _BaudRate, int _DataBits, SerialPort::Parity _Parity, SerialPort::StopBits _StopBits);
	void setCallback(SerialCallBack* callBack);

	void Open();
	void SetWriteTimeout(int ms);
	void SetReadTimeout(int ms);
	int ReadByte();
	int SerialByteTime();

	virtual bool IsOpen() = 0;
	virtual void Close() = 0;
	virtual void DiscardInOut() = 0;
	virtual void Write(const uint8_t * Data, int offs, int count) = 0;
	virtual int Read(uint8_t * Data, int offs, int count) = 0;
	virtual int BytesToRead() = 0;

protected:
	const void * ComPort;
	SerialCallBack* serialCallBack;
	virtual bool StartEventHandler() { return false; }
	int BaudRate;
	int DataBits;
	SerialPort::Parity mParity;
	SerialPort::StopBits mStopBits;
	uint32_t WriteTimeout;
	uint32_t ReadTimeout;
		
	virtual bool OpenPort() = 0;
	virtual void SetTimeouts() = 0;
};
