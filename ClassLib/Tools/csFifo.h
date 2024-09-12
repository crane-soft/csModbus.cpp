#pragma once

/* This FIFO can be written and read by different threads without any additional measures.
   But it cannot be written or read by multiple threads at the same time.
   It is primarily used to transfer data from one thread to another.
   One thread writes, another reads.
*/

class csFifo
{
public:

	csFifo(int aSize)
	{
		Size = aSize;
		Clear();
	}

	void Clear(void)
	{
		PutCnt = 0;
		GetCnt = 0;
		WrIdx = 0;
		RdIdx = 0;
	}
		
	int	Available(void)
	{
		return (PutCnt - GetCnt);
	}
	
	int FreeLeft(void)
	{
		return (Size - (PutCnt - GetCnt));
	}
		

protected:
	void		PutIncr(void)
	{
		if (++WrIdx == Size)
			WrIdx = 0;
		++PutCnt;
	}
		
	void		GetIncr(void)
	{
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

template <typename FifoTyp_t> 
class csTypFifo : public csFifo 
{
public:
	csTypFifo(FifoTyp_t *BuffPtr, int aSize) : csFifo(aSize) 
	{
		Buff = BuffPtr;
	}

	void Write(FifoTyp_t Data) {
		Buff[WrIdx] = Data;
		PutIncr();
	}
	
	FifoTyp_t Read()
	{
		FifoTyp_t data = Buff[RdIdx];
		GetIncr();
		return data;
	}

private:
	FifoTyp_t * Buff;
};

template <typename FifoTyp_t, int const BuffSize> 
class csFifoBuff : public csTypFifo<FifoTyp_t> 
{
public:
	csFifoBuff() : csTypFifo<FifoTyp_t> (Buffer, BuffSize){}
private:
	FifoTyp_t Buffer[BuffSize];		
};


