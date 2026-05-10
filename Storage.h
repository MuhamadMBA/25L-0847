#ifndef STORAGE_H
#define STORAGE_H

template <class T>
class Storage
{
private:
    static const int MAX_SIZE = 100;
    T* data;
    int count;
public:

    Storage() : count(0)
    {
        data = new T[MAX_SIZE];
    }


    bool add(T item)
    {
        if (count >= MAX_SIZE)
        {
            return false;
        }

        data[count] = item;
        count++;

        return true;
    }

    bool remove(int index)
    {
        if (index < 0 || index >= count)
        {
            return false;
        }

        for (int i = index; i < count - 1; i++)
        {
            data[i] = data[i + 1];
        }

        count--;

        return true;
    }

    T get(int index) const
    {
        return data[index];
    }

    T& getReference(int index)
    {
        return data[index];
    }

    int size() const
    {
        return count;
    }

    bool isEmpty() const
    {
        return count == 0;
    }

    bool isFull() const
    {
        return count == MAX_SIZE;
    }

    void clear()
    {
        count = 0;
    }

    //returns the index of the element whose getID() matches
    int findByID(int id) const
    {
        for (int i = 0; i < count; i++)
        {
            if (data[i].getID() == id)
            {
                return i;
            }
        }

        return -1;
    }

    // Removes the element whose getID() matches.
    bool removeByID(int id)
    {
        int index = findByID(id);

        if (index == -1)
        {
            return false;
        }

        return remove(index);

    }

    Storage(const Storage& other) : count(other.count)
    {
        data = new T[MAX_SIZE];
        for (int i = 0; i < count; i++) data[i] = other.data[i];
    }
    Storage& operator=(const Storage& other)
    {
        if (this != &other) {
            count = other.count;
            for (int i = 0; i < count; i++) data[i] = other.data[i];
        }
        return *this;
    }

    ~Storage()
    {
        delete[] data;
    }
};

#endif
