#include "MbSlaveStateMachine.h"
namespace csModbusLib {
	void MbSlaveStateMachine::StartListener()
	{
		SerialInterface = (MbSerial*)gInterface;
		sp = SerialInterface->getSerialPort();

		InitTimeoutTimer();
		RxState = enRxStates::Idle;
		//sp->DataReceived += SerialInterface_DataReceivedEvent;
		WaitForFrameStart();
	}

	void MbSlaveStateMachine::StopListener()
	{
		//sp->DataReceived -= SerialInterface_DataReceivedEvent;
		//TimeoutTimer.Stop()
	}

	void MbSlaveStateMachine::WaitForFrameStart()
	{
		DataBytesNeeded = 0;
		RxState = enRxStates::StartOfFrame;
		SetTimeOut(0);
		SerialInterface_DataReceivedEvent();  // for sure if DataReceived has not yet fired but data allready avaiable
												// the event funcktuin is called firs

	}

	void MbSlaveStateMachine::WaitFrameEnd()
	{
		DataBytesNeeded = 0;
		serialBytesNeeded = SerialInterface->EndOffFrameLenthth();
		InitReceiveDataEvent(enRxStates::RcvEndOfFrame);
	}
	void MbSlaveStateMachine::WaitFrameData(enRxStates NextState, int DataLen)
	{
		DataBytesNeeded = DataLen;
		serialBytesNeeded = SerialInterface->NumOfSerialBytes(DataLen);
		InitReceiveDataEvent(NextState);

	}
	void MbSlaveStateMachine::InitReceiveDataEvent(enRxStates NextState)
	{
		SetTimeOut(SerialInterface->GetTimeOut_ms(serialBytesNeeded));
		RxState = NextState;

		//sp->ReceivedBytesThreshold = serialBytesNeeded;  // all should be initialized now, because if alldata allready available,
														// changing the tiemout property will raise the DataReceived event now
		if (RxState == NextState) {
			SerialInterface_DataReceivedEvent();  // for sure if DataReceived has not yet fired but some allready avaiable
															// the event funcion ist called once
		}
	}
	void MbSlaveStateMachine::InitTimeoutTimer()
	{
		//TimeoutTimer = new System.Timers.Timer();
		//TimeoutTimer.AutoReset = false;
		//TimeoutTimer.Elapsed += TimeoutTimer_Elapsed;
	}
	void MbSlaveStateMachine::SetTimeOut(int timeOut)
	{
		/* TimeoutTimer.Stop();
		if (timeOut > 0) {
			TimeoutTimer.Enabled = false;
			TimeoutTimer.Interval = timeOut;
			TimeoutTimer.Start();
		} */
	}
	void MbSlaveStateMachine::TimeoutTimer_Elapsed()
	{
		// Delay and frame Start?
		//WaitFrameStart();
	}
	void MbSlaveStateMachine::SerialInterface_DataReceivedEvent()
	{
		int DataLen;

		if (RxState == enRxStates::StartOfFrame) {
			if (SerialInterface->StartOfFrameDetected()) {
				Frame.RawData.Clear();
				WaitFrameData(enRxStates::ReceiveHeader, 2);
			}
		} else {

			if (DataBytesNeeded > 0) {
				if (sp->BytesToRead() < serialBytesNeeded) {
					return;
				}

				if (DataBytesNeeded > 0) {
					try {
						SerialInterface->ReceiveBytes(DataBytesNeeded);
					}
					catch (ErrorCodes errCode) {
						WaitForFrameStart();
					}
				}
			}

			switch (RxState) {
			case enRxStates::ReceiveHeader:
				DataLen = Frame.ParseMasterRequest();
				WaitFrameData(enRxStates::RcvMessage, DataLen);
				break;
			case enRxStates::RcvMessage:
				DataLen = Frame.ParseDataCount();
				if (DataLen != 0) {
					WaitFrameData(enRxStates::RcvAdditionalData, DataLen);
				} else {
					WaitFrameEnd();
				}
				break;
			case enRxStates::RcvAdditionalData:
				WaitFrameEnd();
				break;
			case enRxStates::RcvEndOfFrame:
				MasterRequestReceived();
				break;
			default:
				break;
			}
		}

	}
	void MbSlaveStateMachine::MasterRequestReceived()
	{

	}

}