#ifndef CYCLE_QUEUE_H
#define CYCLE_QUEUE_H

/*
循环队列工具类
*/

//循环队列默认的大小
#define CYCLE_QUEUE_DEFAULT_SIZE (2*1024*1024)

class CycleQueue
{  

public:  

    CycleQueue(int size = CYCLE_QUEUE_DEFAULT_SIZE);
    virtual ~CycleQueue();
    //写入, 读取数据, 返回值为实际写入/读取的数据量
    int write(const char* pBuf, int size);
    int read(char* pBuf, int size);

    //队列是否已满
	bool isFull();  
    //队列是否为空
	bool isEmpty();  
    //清空队列中所有数据
    void clear();
    //获取已经使用的空间
	int getUsedSize();  
    //获取队列中剩余的空间
	int getFreeSize();    
    //获取队列总共的空间大小
    int getQueueSize() { return m_bufSize; }

private:  
	//空标示符  
    bool m_isEmpty;
	//满标示符  
    bool m_isFull;
	//缓冲区指针  
	char * m_pBuf;  
	//缓冲区大小  
    int m_bufSize;
    //当前读位置, 也是有效数据开始的位置
    int m_readPos;
    //当前写位置, 也是有效数据结束的位置
    int m_writePos;
};  
#endif // CYCLE_QUEUE_H
