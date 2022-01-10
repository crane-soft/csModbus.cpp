#include "MbEthernet.h"


namespace csModbusLib
{
    class MbUDPMaster : public MbETHMaster
    {
	public:
		MbUDPMaster(char * host_name, int port);

		// overrides MbIterface Functions
		bool Connect();
		void DisConnect();
		void SendFrame(MbRawData TransmitData, int Length);
		void ReceiveBytes(MbRawData *RxData, int count);

		// overides MbETHMaster::RecvDataAvailable()
		bool RecvDataAvailable();

	private:
		int BytesReaded;
	};


    class MbUDPSlave : public MbETHSlave
    {
	private:
		class UdpContext : EthContext
        {
            public UdpContext (UdpClient client)
            {
                udpc = client;
                Frame_Buffer = new MbRawData();
            }
            public IPEndPoint EndPoint;
            public UdpClient udpc;
            public override void SendFrame (byte[] data, int Length)
            {
                udpc.Send(data, Length, EndPoint);
            }

            public bool EndReceive (IAsyncResult ar)
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
		void OnClientRead(IAsyncResult ar);
	};
}
