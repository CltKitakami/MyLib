#ifndef _MESSAGEHANDLER_HPP_
#define _MESSAGEHANDLER_HPP_
/// Chain of Responsibility

#include <vector>


template <typename T>
class MessageHandler
{
protected:
	MessageHandler<T> *next;

public:
	MessageHandler() : next(nullptr) {}
	virtual ~MessageHandler() = default;

	void handleNext(T message)
	{
		if (next != nullptr)
			next->handle(message);
	}

	void setNext(MessageHandler<T> *next) { this->next = next; }

	MessageHandler<T> * getNext() { return this->next; }
	const MessageHandler<T> * getNext() const { return this->next; }

	virtual void handle(T message) = 0;
};


template <typename T>
class MessageHandlerContainer
{
	std::vector<MessageHandler<T> *> v;

public:
	void handle(T message)
	{
		if (v.size() > 0)
			v[0]->handle(message);
	}

	void attach(MessageHandler<T> *handler)
	{
		if (v.size() > 0)
			v.back()->setNext(handler);

		handler->setNext(nullptr);
		v.push_back(handler);
	}

	void dettach(MessageHandler<T> *handler)
	{
		typedef typename std::vector<MessageHandler<T> *>::iterator Iterator;

		for (Iterator i = v.begin(); i != v.end(); ++i)
		{
			if (*i == handler)
			{
				if (i != v.begin())
				{
					(*(i - 1))->setNext(i + 1 == v.end() ?
						nullptr : (*i)->getNext());
				}

				(*i)->setNext(nullptr);
				v.erase(i);

				break;
			}
		}
	}
};

#endif
