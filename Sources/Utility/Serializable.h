#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <fstream>
#include <vector>


enum class SerializeMode
{
    Read, Write
};

class Serializable
{
public:

    Serializable() = default;
    virtual ~Serializable() = default;

    virtual void Serialize(std::ifstream& stream) = 0;
    virtual void Serialize(std::ofstream& stream) = 0;

    template<class T>
    void Field(std::ifstream& stream, T& field)
    {
        stream.read((char*)&field, sizeof(T));
    }
    template<class T>
    void Field(std::ofstream& stream, T& field)
    {
        stream.write((char*)&field, sizeof(T));
    }

    template<class T>
    void Field(std::ifstream& stream, std::vector<T>& field)
    {
        size_t size;
        stream.read((char*)&size, sizeof(size));
        field.resize(size);
        stream.read((char*)&field, sizeof(T) * size);
    }
    template<class T>
    void Field(std::ofstream& stream, std::vector<T>& field)
    {
        size_t size = field.size();
        stream.write((char*)&size, sizeof(size));
        stream.write((char*)&field, sizeof(T) * size);
    }
};

#endif // SERIALIZABLE_H
