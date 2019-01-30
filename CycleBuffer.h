#ifndef CYCLEBUFFER_H
#define CYCLEBUFFER_H

#include <iostream>

template <class T>
class CycleBuffer
{
public:
    CycleBuffer(unsigned int size = 10 * 1024)
    {
        m_size = size;
        m_front = 0;
        m_rear = 0;
        m_pData = new T[size];
    }
    ~CycleBuffer()
    {
        if (m_pData != NULL)
        {
            delete[] m_pData;
            m_pData = NULL;
        }
    }

    bool isEmpty()
    {
        return (m_front == m_rear);
    }

    bool isFull()
    {
        return (m_front == (m_rear + 1) % m_size);
    }

    //获取还有多少个空位
    unsigned int getEmptySize()
    {
        unsigned int emptySize = 0;
        if (m_front <= m_rear)
        {
            emptySize = m_size - (m_rear - m_front);
        }
        else
        {
            emptySize = m_front - m_rear - 1;
        }
        return emptySize;
    }

    //获取循环缓存中已经有多少个数据
    int getDataSize()
    {
        return m_size - getEmptySize();
    }

    bool push(T data)
    {
        if (isFull())
        {
            return false;
        }
        m_pData[m_rear] = data;
        m_rear = (m_rear + 1) % m_size;
        return true;
    }

    bool push(T* pData, unsigned int size)
    {
        int emptySize = getEmptySize();
        if (emptySize >= size)
        {   //可以存入此循环缓冲区中
            if (m_front <= m_rear)
            {   //可用空间在线性存储空间的两侧, 可能需要两次拷贝进去
                int rearEmptySize = m_size - m_rear - 1;
                if (rearEmptySize >= size)
                {   //在线性存储空间尾部可以放下当前输入数据
                    memcpy((char*)(m_pData + m_rear + 1), (char*)pData, sizeof(T) * size);
                    m_rear += size;
                }
                else
                {   //线性存储空间尾部空间不足, 需要转一次头
                    //在尾部存储数据
                    memcpy((char*)(m_pData + m_rear + 1), (char*)pData, sizeof(T) * rearEmptySize);
                    //在开头存储数据
                    memcpy((char*)(m_pData), (char*)(pData + rearEmptySize), size - rearEmptySize);
                    m_rear = (m_rear + size) % m_size;
                }
            }
            else
            {   //可用空间在线性存储空间中间, 可以一次拷贝进去
                memcpy((char*)(m_pData + m_rear), (char*)pData, sizeof(T) * size);
                m_rear += size;
            }
            return true;
        }
        else
        {   //剩余空间不足以放下输入数据, 直接退出
            return false;
        }
    }

    bool pop(T& data)
    {
        if (isEmpty())
        {
            return false;
        }
        data = m_pData[m_front];
        m_front = (m_front + 1) % m_size;
        return true;
    }

    bool pop(T* pData, unsigned int size)
    {
        int dataSize = getDataSize();
        if (dataSize >= size)
        {   //此循环缓冲区中数据足够取出
            if (m_front <= m_rear)
            {
                //数据在线性存储空间中间, 可以一次拷贝进去
                memcpy((char*)(pData), (char*)(m_pData + m_front), sizeof(T) * size);
                m_front += size;
            }
            else
            {   //可用空间在线性存储空间两侧
                int rearDataSize = m_size - m_front;    //在线性存储空间尾部的数据数量
                if (rearDataSize >= size)
                {   //在线性存储空间尾部数据足够取出
                    memcpy((char*)pData, (char*)(m_pData + m_front), sizeof(T) * size);
                    m_front += size;
                }
                else
                {   //线性存储空间尾部数据不足, 需要转一次头再取额外的数据
                    //在尾部读取数据
                    memcpy((char*)pData, (char*)(m_pData + m_front), sizeof(T) * rearDataSize);
                    //在开头存储数据
                    memcpy((char*)(pData + rearDataSize), (char*)(m_pData), size - rearDataSize);
                    m_front = (m_front + size) % m_size;
                }
            }
            return true;
        }
        else
        {   //存储空间中的数据不足, 退出
            return false;
        }

    }

private:
    unsigned int m_size;
    int m_front;
    int m_rear;
    T* m_pData;

};

#endif // CYCLEBUFFER_H

