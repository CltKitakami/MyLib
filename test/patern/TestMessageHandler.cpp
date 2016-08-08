#include "common/Log.hpp"
#include "pattern/MessageHandler.hpp"


class T1 : public MessageHandler<int>
{
public:
	virtual void handle(int v)
	{
		if (v > 100)
			LOGT("T1 work");
		else
			handleNext(v);
	}
};

class T2 : public MessageHandler<int>
{
public:
	virtual void handle(int v)
	{
		if (v < 50)
			LOGT("T2 work");
		else
			handleNext(v);
	}
};


int main()
{
	MessageHandlerContainer<int> mhc;
	T1 t1;
	T2 t2;
	mhc.attach(&t1);
	mhc.attach(&t2);
	mhc.handle(49);
	mhc.handle(101);
	return 0;
}
