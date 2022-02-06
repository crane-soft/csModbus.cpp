﻿#include "MbEthSlave.h"
#include "MbFrame.h"
#include "UdpClient.h"
#include "TcpClient.h"

namespace csModbusLib {

	// -------------------------------------------------------------------
	// Ethernet Slave
	// -------------------------------------------------------------------
	MbETHSlave::MbETHSlave(int port) {
		SetPort(port);
	}

	void MbETHSlave::SendFrame(MbRawData *TransmitData, int Length)
	{
		TransmitData->PutUInt16(4, (uint16_t)Length);
		try {
			SendFrameData(TransmitData->Data, Length + MBAP_Header_Size);
			FreeMessage();
		} catch (int errCode) {
			throw errCode;
		}
	}

	// -------------------------------------------------------------------
	// Ethernet UDP-Slave
	// -------------------------------------------------------------------
	MbUDPSlave::MbUDPSlave(int port) : MbETHSlave(port) { }

	bool MbUDPSlave::Connect()
	{
		try {
			mUdpClient = new UdpClient(0, remote_port);
			IsConnected = true;

		} catch (int errCode) {
			throw errCode;
		}
		return IsConnected;
	}

	void MbUDPSlave::DisConnect()
	{
		if (IsConnected) {
			IsConnected = false;
			if (mUdpClient != NULL) {
				//mUdpClient->Shutdown(); ??
				mUdpClient->Close();
			}
		}
	}

	void MbUDPSlave::ReceiveHeader(int timeOut, MbRawData *RxData)
	{
		UdpReceiveHeaderData(timeOut, RxData);
	}

	void MbUDPSlave::SendFrameData(uint8_t *data, int Length)
	{
		mUdpClient->SendResponse(data, Length);
	}

	// -------------------------------------------------------------------
	// Ethernet TCP-Slave
	// -------------------------------------------------------------------
	MbTCPSlave::MbTCPSlave(int port) : MbETHSlave(port)
	{
	}

	bool MbTCPSlave::Connect()
	{
		smRxProcess->TryRelease();

		try {
			TcpListener::StartListen(remote_port);
			IsConnected = true;
		} catch (int) {
			IsConnected = false;

		}
		return IsConnected;
	}

	void MbTCPSlave::DisConnect()
	{
		if (IsConnected) {
			IsConnected = false;
			TcpListener::Stop();
			smRxProcess->TryRelease();
			smRxDataAvail->TryRelease();
		}
	}

	void MbTCPSlave::ReceiveHeader(int timeOut, MbRawData *RxData)
	{
		smRxDataAvail->Wait();
		if (IsConnected == false)
			throw ErrorCodes::CONNECTION_CLOSED;
		ConnectionContext->CopyRxData(RxData);
	}

	void MbTCPSlave::SendFrameData(uint8_t *data, int Length)
	{
		ConnectionContext->SendFrame(data, Length);

	}

	void MbTCPSlave::FreeMessage()
	{
		smRxProcess->Release();
	}

	void MbTCPSlave::OnClientAccepted(NetStream *stream)
	{
		TcpContext *context = new TcpContext(stream);
		context->BeginNewFrame();
		context->BeginReadFrameData();
	}

	void MbTCPSlave::OnClientRead(void * context)
	{
		TcpContext *rxContext = (TcpContext*)context;
		if ((rxContext == NULL) || (IsConnected == false)) {
			// maybe the listener was closed
			return;
		}
		if (rxContext->EndReceive()) {
			RequestReceived(rxContext);
			rxContext->BeginNewFrame();
		}
		rxContext->BeginReadFrameData();
	}

	void MbTCPSlave::RequestReceived(TcpContext *newContext)
	{
		smRxProcess->Wait();
		ConnectionContext = newContext;
		smRxDataAvail->Release();
	}

	// -------------------------------------------------------------------
	// Ethernet TCP-Slave Context
	// -------------------------------------------------------------------
	TcpContext::TcpContext(NetStream *aStream)
	{
		Stream = aStream;
		closed = false;
	}
	void TcpContext::SendFrame(uint8_t *data, int Length)
	{
		Stream->Send(data, Length);
	}
	void TcpContext::BeginNewFrame()
	{
		Bytes2Read = MbEthernet::MBAP_Header_Size;
		ReadIndex = 0;
		NewFrame = true;

	}

	void TcpContext::BeginReadFrameData()
	{
		if (closed == false)
			Stream->BeginRcv(&FrameBuffer.Data[ReadIndex], Bytes2Read,this);
	}

	bool TcpContext::EndReceive()
	{
		int readed = Stream->EndRead();
		if (readed == 0) {
			// Connection closed
			Stream = NULL;
			closed = true;
			return false;
		}
		ReadIndex += readed;
		Bytes2Read -= readed;
		if (Bytes2Read > 0) {
			return false;
		}

		FrameBuffer.EndIdx = ReadIndex;
		if (NewFrame) {
			Bytes2Read = FrameBuffer.CheckEthFrameLength();
			NewFrame = false;
			return false;
		}
		return true;
	}

	void TcpContext::CopyRxData(MbRawData *RxData)
	{
		RxData->CopyFrom(&FrameBuffer);
	}
}
