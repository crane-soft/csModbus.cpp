#pragma once

/* Dieser Fifo kann ohne zusätzliche Maßnahmen von unterschiedlichen Threads geschrieben und gelesen werden.
   Nicht aber gleichzeitig von mehreren Threads geschrieben bzw mehreren Threads gelesen werden
   Er dient in erster Linie dazu Daten von einem Thread zu einem anderen zu übertragen
   Ein Thread schreibt ein anderer liest.
*/

class csFifo
{
public:

	csFifo(int aSize) {
		Size = aSize;
		Clear();
	}

	void  Clear(void) {
		WrIdx = 0;
		RdIdx = 0;
		PutCnt = 0;
		GetCnt = 0;
	}

	int FillLevel(void) {
		return (PutCnt - GetCnt);
	}

	int  FreeLeft(void) {
		return (Size - (PutCnt - GetCnt));
	}


protected:
	void PutIncr(void)	{
		if (++WrIdx == Size)
			WrIdx = 0;
		++PutCnt;
	}

	void GetIncr(void) {
		if (++RdIdx == Size)
			RdIdx = 0;
		++GetCnt;
	}

	unsigned	WrIdx;
	unsigned	RdIdx;
	unsigned	Size;

private:
	int			PutCnt;
	int			GetCnt;
};

template <typename DataTyp> class csTypFifo : public csFifo
{
public:
	csTypFifo(DataTyp *BuffPtr, int aSize) 	: csFifo(aSize) {
		Buff = BuffPtr;
	}

	void	Write(DataTyp* Data) {
		if (Buff == 0) return;
		Buff[WrIdx] = *Data;
		PutIncr();
	}

	DataTyp Read() {
		if (Buff == 0) return 0;
		DataTyp RdData = Buff[RdIdx];
		GetIncr();
		return RdData;
	}

	void Read(DataTyp* Data) {
		if (Buff == 0) return;
		if (Data != 0) {
			*Data = Buff[RdIdx];
		}
		GetIncr();
	}


private:
	DataTyp * Buff;

};


template <typename DataTyp, int const BuffSize>
class csFifoBuff : public csTypFifo <DataTyp> {
public:
	csFifoBuff() : csTypFifo <DataTyp>(Buffer, BuffSize){}
private:
	DataTyp Buffer[BuffSize];
};
