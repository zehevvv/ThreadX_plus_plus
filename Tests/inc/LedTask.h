#include "Task.h"
#include "Event.h"
#include "Mutex.h"
#include "Semaphore.h"
#include "Singleton.h"


class LedTask : public Task, public Singleton<LedTask>
{
friend Singleton<LedTask>;
private:
	LedTask();
	void Main_loop();
	void ToggleLed();

	static const uint16_t 	TASK_PRIORITY 	= 10;
	static const uint16_t 	STACK_SIZE 		= 1024;
protected:
	virtual void ReceiveMsg(void* pointer);
public:
	Event event;
	Mutex m_mutex;
	Semaphore m_semahore;
};
