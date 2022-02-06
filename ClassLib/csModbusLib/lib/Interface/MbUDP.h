#include "MbEthernet.h"
#include "UdpClient.h"

namespace csModbusLib
{
    class MbUDPMaster : public MbETHMaster
    {
	public:
		MbUDPMaster(const char * host_name, int port);

		// overrides MbIterface Functions
		bool Connect();
		void DisConnect();
		void SendFrame(MbRawData TransmitData, int Length);
		void ReceiveBytes(MbRawData *RxData, int count);


		bool RecvDataAvailable();

	private:
		int BytesReaded;
	};

	/*
    class MbUDPSlave : public MbETHSlave
    {
	private:
		class UdpContext : EthContext
        {
		public:
             UdpContext (UdpClient client)  {
                udpc = client;
                Frame_Buffer = new MbRawData();
            }

            IPEndPoint EndPoint;
            UdpClient udpc;
            
			void SendFrame (char * data, int Length)  {
                udpc.Send(data, Length, EndPoint);
            }

			
            bool EndReceive (IAsyncResult ar)
            {
                Frame_Buffer.Data = udpc.EndReceive(ar, ref EndPoint);
                if (Frame_Buffer.Data != null) {
                    // TODO check length
                    Frame_Buffer.EndIdx = Frame_Buffer.Data.Length;
                    return true;
                }
                return false;
            } 
		};

	private:
		UdpContext mUdpContext[2];


	public:
		MbUDPSlave (int port)
			: MbETHSlave(port)
        {
        }

		// overrides MbIterface Functions
		bool Connect();
		void DisConnect();

	private:
		//void OnClientRead(IAsyncResult ar);
	}; */
}
