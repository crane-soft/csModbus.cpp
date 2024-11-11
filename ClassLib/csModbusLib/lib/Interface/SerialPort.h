#pragma once
#include <functional>
#include <stdint.h>

class SerialCallBack {
public:
	virtual void DataReceived(int bytesAvailavle) {}
};

class SerialPort {

public:
	using ReadCallback_t = typename std::function<int(int)>;

	enum class Parity { NoParity, Odd, Even } ;
	enum class StopBits { NoStopBits, One, Two, OnePointFive } ;

	SerialPort();
	SerialPort(const void* _ComPort, int _BaudRate);

	void SetComParms(const void* _ComPort, int _BaudRate);
	void SetComParms(const void* _ComPort, int _BaudRate, int _DataBits, SerialPort::Parity _Parity, SerialPort::StopBits _StopBits);
	void setCallback(ReadCallback_t callBack);

	bool Open();
	void SetWriteTimeout(int ms);
	void SetReadTimeout(int ms);
	int ReadByte();
	int SerialByteTime() const;

	virtual bool IsOpen() = 0;
	virtual void Close() = 0;
	virtual void DiscardInOut() = 0;
	virtual int Write(const uint8_t * Data, int count) = 0;
	virtual int Read(uint8_t * Data,  int count) = 0;
	virtual void ReadEv(uint8_t* Data, int count) = 0;

protected:
	const void * ComPort;
	ReadCallback_t ReadCallBack;

	int BaudRate;
	int DataBits;
	SerialPort::Parity mParity;
	SerialPort::StopBits mStopBits;
	uint32_t WriteTimeout;
	uint32_t ReadTimeout;
		
	virtual bool OpenPort() = 0;
	virtual void SetTimeouts() {}
};
