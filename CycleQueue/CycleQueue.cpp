
#include "CycleQueue.h"
#include <assert.h>  
#include <memory.h>

CycleQueue::CycleQueue(int size)

{    
    m_bufSize = size;
    m_readPos = 0;
    m_writePos = 0;
    m_pBuf = new char[m_bufSize];
    m_isEmpty = true;
    m_isFull = false;
}    

CycleQueue::~CycleQueue()
{    
	delete[] m_pBuf;    
}   

int CycleQueue::write(const char* pBuf, int size)
{    
    if(size <= 0)
    {
        return 0;
    }
    m_isEmpty = false;
    //队列已满，不能继续写入
    if(m_isFull)
	{    
		return 0;    
	}    
    else if(m_readPos == m_writePos) // 缓冲区为空时
	{    
		/*                          == 内存模型 ==
        (empty)                         m_readPos                (empty)
		|----------------------------------|-----------------------------------------|  
                                        m_writePos                              m_bufSize
        */
        int leftCount = m_bufSize - m_writePos;
        if(leftCount > size)
        {   //插入点位置右侧可以存放下本次数据
            memcpy(m_pBuf + m_writePos, pBuf, size);
            m_writePos += size;
            m_isFull = (m_writePos == m_readPos);
            return size;
		}    
		else    
        {   //插入点后面放不下了, 需要使用前半段数据
            memcpy(m_pBuf + m_writePos, pBuf, leftCount);
            m_writePos = (m_readPos > size - leftCount) ? size - leftCount : m_writePos;
            memcpy(m_pBuf, pBuf + leftCount, m_writePos);
            m_isFull = (m_writePos == m_readPos);
            return leftCount + m_writePos;
		}    
	}     
    else if(m_readPos < m_writePos) // 有剩余空间可写入
	{    
		/*                           == 内存模型 ==
        (empty)                        (data)                     (empty)
		|-------------------|----------------------------|---------------------------|  
                          m_readPos                  m_writePos    (leftCount)
		*/    
		// 剩余缓冲区大小(从写入位置到缓冲区尾)

        int leftCount = m_bufSize - m_writePos;
        if(leftCount > size)   // 有足够的剩余空间存放
		{    
            memcpy(m_pBuf + m_writePos, pBuf, size);
            m_writePos += size;
            m_isFull = (m_readPos == m_writePos);
            assert(m_readPos <= m_bufSize);
            assert(m_writePos <= m_bufSize);
            return size;
		}    
		else       // 剩余空间不足
		{    
			// 先填充满剩余空间，再回头找空间存放
            memcpy(m_pBuf + m_writePos, pBuf, leftCount);
            m_writePos = (m_readPos >= size - leftCount) ? size - leftCount : m_readPos;
            memcpy(m_pBuf, pBuf + leftCount, m_writePos);
            m_isFull = (m_readPos == m_writePos);
            assert(m_readPos <= m_bufSize);
            assert(m_writePos <= m_bufSize);
            return leftCount + m_writePos;
        }
	}    
	else    
	{    
		/*                          == 内存模型 ==
        (unread)                        (read)                     (unread)
		|-------------------|----------------------------|---------------------------|  
                         m_writePos    (leftCount)    m_readPos
        */
        int leftCount = m_readPos - m_writePos;
        if(leftCount > size)
		{    
			// 有足够的剩余空间存放
            memcpy(m_pBuf + m_writePos, pBuf, size);
            m_writePos += size;
            m_isFull = (m_readPos == m_writePos);
            assert(m_readPos <= m_bufSize);
            assert(m_writePos <= m_bufSize);
            return size;
		}    
		else    
		{    
			// 剩余空间不足时要丢弃后面的数据
            memcpy(m_pBuf + m_writePos, pBuf, leftCount);
            m_writePos += leftCount;
            m_isFull = (m_readPos == m_writePos);
            assert(m_isFull);
            assert(m_readPos <= m_bufSize);
            assert(m_writePos <= m_bufSize);
            return leftCount;
		}    
	}    
}    

int CycleQueue::read(char* pBuf, int size)
{
    if(size <= 0)
    {
        return 0;
    }
    m_isFull = false;
    if(m_isEmpty)       // 缓冲区空，不能继续读取数据
	{    
		return 0;    
	}    
    else if(m_readPos == m_writePos)   // 缓冲区满时
	{    
		/*                          == 内存模型 ==
                     (data)          m_readPos                (data)
		|--------------------------------|--------------------------------------------|  
                                     m_writePos                                   m_bufSize
		*/    
        int leftcount = m_bufSize - m_readPos;
        if(leftcount > size)
		{    
            memcpy(pBuf, m_pBuf + m_readPos, size);
            m_readPos += size;
            m_isEmpty = (m_readPos == m_writePos);
            return size;
		}    
		else    
		{    
            memcpy(pBuf, m_pBuf + m_readPos, leftcount);
            m_readPos = (m_writePos > size - leftcount) ? size - leftcount : m_writePos;
            memcpy(pBuf + leftcount, m_pBuf, m_readPos);
            m_isEmpty = (m_readPos == m_writePos);
            return leftcount + m_readPos;
		}    
	}    
    else if(m_readPos < m_writePos)   // 写指针在前(未读数据是连接的)
	{    
		/*                          == 内存模型 ==
        (read)                      (unread)                      (read)
		|-------------------|----------------------------|---------------------------|  
                          m_nReadPos                m_nWritePos                 m_nBufSize
		*/    
        int leftcount = m_writePos - m_readPos;
        int c = (leftcount > size) ? size : leftcount;
        memcpy(pBuf, m_pBuf + m_readPos, c);
        m_readPos += c;
        m_isEmpty = (m_readPos == m_writePos);
        assert(m_readPos <= m_bufSize);
        assert(m_writePos <= m_bufSize);
		return c;    
	}    
	else          // 读指针在前(未读数据可能是不连接的)
	{    
		/*                          == 内存模型 ==
             (unread)                   (read)                      (unread)
		|-------------------|----------------------------|---------------------------|  
                        m_nWritePos                  m_nReadPos                  m_nBufSize
		*/    
        int leftcount = m_bufSize - m_readPos;
        if(leftcount > size)   // 未读缓冲区够大，直接读取数据
		{    
            memcpy(pBuf, m_pBuf + m_readPos, size);
            m_readPos += size;
            m_isEmpty = (m_readPos == m_writePos);
            assert(m_readPos <= m_bufSize);
            assert(m_writePos <= m_bufSize);
            return size;
		}    
		else       // 未读缓冲区不足，需回到缓冲区头开始读
		{    
            memcpy(pBuf, m_pBuf + m_readPos, leftcount);
            m_readPos = (m_writePos >= size - leftcount) ? size - leftcount : m_writePos;
            memcpy(pBuf + leftcount, m_pBuf, m_readPos);
            m_isEmpty = (m_readPos == m_writePos);
            assert(m_readPos <= m_bufSize);
            assert(m_writePos <= m_bufSize);
            return leftcount + m_readPos;
        }
    }
}

bool CycleQueue::isFull()
{    
    return m_isFull;
}

bool CycleQueue::isEmpty()
{    
    return m_isEmpty;
}    

void CycleQueue::clear()
{    
    m_readPos = 0;
    m_writePos = 0;
    m_isEmpty = true;
    m_isFull = false;
}    

int CycleQueue::getUsedSize()
{    
    if(m_isEmpty)
	{    
		return 0;    
	}    
    else if(m_isFull)
	{    
        return m_bufSize;
	}    
    else if(m_readPos < m_writePos)
	{    
        return m_writePos - m_readPos;
	}    
	else    
	{    
        return m_bufSize - m_readPos + m_writePos;
	}    
}  

int CycleQueue::getFreeSize()
{    
    if(m_isEmpty)
	{    
        return m_bufSize;
	}    
    else if(m_isFull)
	{    
		return 0;    
	}    
    else if(m_readPos > m_writePos)
	{    
        return m_readPos - m_writePos;
	}    
	else    
	{    
        return m_bufSize - m_writePos + m_readPos;
	}    
}  

