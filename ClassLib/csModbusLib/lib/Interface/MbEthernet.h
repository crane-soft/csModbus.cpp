#include "MbInterface.h"
#include "MbFrame.h"
#include <stdint.h>
#include <mutex>

namespace csModbusLib {
	class MbEthernet : public MbInterface {
	public:
		enum ModbusEthernetType {
			TCP = 0,
			UDP = 1
		};

		MbEthernet() {
			TransactionIdentifier = 0;
		}



	protected:
		void SetPort(int port)
		{
			remote_port = port;
		}

		void FillMBAPHeader(MbRawData *TransmitData, int Length)
		{
			++TransactionIdentifier;
			TransmitData->PutUInt16(0, TransactionIdentifier);
			TransmitData->PutUInt16(2, 0);
			TransmitData->PutUInt16(4, (uint16_t)Length);
		}

		void CheckTransactionIdentifier(MbRawData *ReceivMessage)
		{
			uint16_t RxIdentifier = ReceivMessage->GetUInt16(0);
			if (RxIdentifier != TransactionIdentifier) {
				throw  ErrorCodes::WRONG_IDENTIFIER;
			}
		}

	protected:
		const int MBAP_Header_Size = 6;
		int remote_port;
		//UdpClient mUdpClient;

	private:
		uint16_t TransactionIdentifier;

	};

	class MbETHMaster : public MbEthernet {
	protected:
		char *remote_host;
		//NetworkStream nwStream;

		virtual bool RecvDataAvailable() = 0;

	public:
		MbETHMaster(char * host_name, int port)
		{
			remote_host = host_name;
			SetPort(port);
		}

		// overrides Interface::ReceiveHeader
		bool ReceiveHeader(MbRawData *MbData)
		{
			if (RecvDataAvailable()) {
				MbData->EndIdx = 0;
				ReceiveBytes(MbData, 8);
				CheckTransactionIdentifier(MbData);
				return true;
			}
			return false;
		}

	};

	class MbETHSlave : public MbEthernet {

	public:
		MbETHSlave(int port)  {
			SetPort(port);
			RequestAvail = false;
		}

		// overrides Interface::ReceiveHeader
		bool ReceiveHeader(MbRawData *MbData)
		{
			if (RequestAvail) {
				MbData->CopyFrom(&CurrentRxContext.Frame_Buffer);
				return true;
			}
			return false;

		}

		// overrides Interface::SendFrame
		void SendFrame(MbRawData TransmitData, int Length)
		{
			TransmitData.PutUInt16(4, (uint16_t)Length);
			CurrentRxContext.SendFrame(TransmitData.Data, Length + MBAP_Header_Size);
			FreeMessage();
		}

	protected:
		class EthContext {
		public:
			MbRawData Frame_Buffer;
			virtual void SendFrame(uint8_t *data, int Length);
		};

		void RequestReceived(EthContext newContext)	{

			smReqest.lock();
			CurrentRxContext = newContext;
			RequestAvail = true;
		}

	private:
		void FreeMessage()
		{
			RequestAvail = false;
			smReqest.unlock();
		}

		EthContext CurrentRxContext;  // Zeiger auf einen gerade neu empfangenen Frame
		std::mutex smReqest;
		bool RequestAvail;
	};
}
