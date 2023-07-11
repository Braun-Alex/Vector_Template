#ifndef CODEREVIEWTASK_MYVECTOR_HPP
#define CODEREVIEWTASK_MYVECTOR_HPP
#include <cstdint>
#include <string>
#include <map>
#include <tuple>
#include <algorithm>
#include <stdexcept>
template<typename Type>
class MyVector//Class std::vector<Type> in no case can not be base class for any inherited class
{
public:
    MyVector() noexcept:ID(COW_identifiers.size()+1)
    {
        ++COW_identifiers[ID];
    }//It touches all the STL containers
    explicit MyVector(size_t entered_size) noexcept:the_pointer(new
    std::pair<Type,std::string>[entered_size]),//So, I decided to rewrite MyVector template absolutely
    the_capacity(entered_size),
    the_size(entered_size),ID(COW_identifiers.size()+1)
    {
        ++COW_identifiers[ID];
    }
    MyVector(const MyVector<Type> &another_vector) noexcept:the_size(another_vector.the_size),
    the_capacity(another_vector.the_capacity)
    {
        if (the_capacity==0)
        {
            ID=COW_identifiers.size()+1;
            ++COW_identifiers[ID];
            return;
        }
        ID=another_vector.ID;
        ++COW_identifiers[ID];
        the_pointer=another_vector.the_pointer;
    }
    MyVector(MyVector<Type> &&another_vector) noexcept:the_size(another_vector.the_size),
    the_capacity(another_vector.the_capacity),the_pointer(another_vector.the_pointer),
    ID(COW_identifiers.size()+1)
    {
        ++COW_identifiers[ID];
        another_vector.the_size=0;
        another_vector.the_capacity=0;
        another_vector.the_pointer=nullptr;
    }
    ~MyVector() noexcept
    {
        --COW_identifiers[ID];
        if (COW_identifiers[ID]==0)
            delete[] the_pointer;
    }
    void operator=(const MyVector<Type> &another_vector) noexcept
    {
        if (the_pointer==another_vector.the_pointer) return;
        the_size=another_vector.the_size;
        the_capacity=another_vector.the_capacity;
        if (the_capacity==0)
        {
            ID=COW_identifiers.size()+1;
            ++COW_identifiers[ID];
            return;
        }
        ID=another_vector.ID;
        ++COW_identifiers[ID];
        the_pointer=another_vector.the_pointer;
    }
    void operator=(MyVector<Type> &&another_vector) noexcept
    {
        if (the_pointer==another_vector.the_pointer) return;
        the_size=another_vector.the_size;
        the_capacity=another_vector.the_capacity;
        the_pointer=another_vector.the_pointer;
        ID=COW_identifiers.size()+1;
        ++COW_identifiers[ID];
        another_vector.the_size=0;
        another_vector.the_capacity=0;
        another_vector.the_pointer=nullptr;
    }
    std::pair<Type,std::string> &operator[](int the_index) noexcept
    {
        if (the_index<0||the_index>=the_size) return the_wrong_index;
        COW_check();
        return the_pointer[the_index];
    }
    const std::pair<Type,std::string> &operator[](int the_index) const noexcept
    {
        if (the_index<0||the_index>=the_size) return the_wrong_index;
        return the_pointer[the_index];
    }
    Type &operator[](const std::string &the_name) noexcept
    {
        auto the_iterator=std::find_if(begin(),end(),[&](std::pair<Type,std::string> &the_pair)
        {
        return std::tie(the_pair.second)==std::tie(the_name);
        });
        if (the_iterator==end()) return the_wrong_iterator;
        COW_check();
        return the_iterator->first;
    }
    const Type &operator[](const std::string &the_name) const noexcept
    {
        auto the_iterator=std::find_if(cbegin(),cend(),[&](const std::pair<Type,std::string> &the_pair)
        {
            return std::tie(the_pair.second)==std::tie(the_name);
        });
        if (the_iterator==cend()) return the_wrong_iterator;
        return the_iterator->first;
    }
    std::pair<Type,std::string>* begin() noexcept
    {
        COW_check();
        return the_pointer;
    }
    std::pair<Type,std::string>* end() noexcept
    {
        COW_check();
        return the_pointer+the_size;
    }
    const std::pair<Type,std::string>* cbegin() const noexcept
    {
        return the_pointer;
    }
    const std::pair<Type,std::string>* cend() const noexcept
    {
        return the_pointer+the_size;
    }
    size_t size() const noexcept
    {
        return the_size;
    }
    bool empty() const noexcept
    {
        return the_size==0;
    }
    void reserve(size_t entered_capacity) noexcept
    {
        if (entered_capacity<=the_capacity) return;
        COW_check();
        the_capacity=entered_capacity;
    }
    void clear() noexcept
    {
        COW_check();
        the_size=0;
    }
    void COW_check()
    {
        if (COW_identifiers[ID]>=2)
        {
            auto the_previous_pointer=the_pointer;
            the_pointer=new std::pair<Type,std::string>[the_capacity];
            for (size_t i=0; i<the_size; ++i)
                the_pointer[i]=the_previous_pointer[i];
            --COW_identifiers[ID];
            ID=COW_identifiers.size()+1;
            ++COW_identifiers[ID];
        }
    }
    void push_back(const Type &the_object,const std::string &the_name) noexcept
    {
        COW_check();
        const auto &the_value=std::make_pair(the_object,the_name);
        if (the_capacity==0)
        {
            the_pointer=new std::pair<Type,std::string>[1];
            the_capacity=1;
            the_size=the_capacity;
            the_pointer[0]=the_value;
        }
        else
        {
            if (the_capacity==the_size)
            {
                the_capacity*=2;
                std::pair<Type,std::string>* the_new_pointer=new std::pair<Type,
                std::string>[the_capacity];
                for (size_t i=0; i<the_size; ++i) the_new_pointer[i]=the_pointer[i];
                delete [] the_pointer;
                the_pointer=the_new_pointer;
            }
            the_pointer[the_size++]=the_value;
        }
    }
private:
    std::pair<Type,std::string>* the_pointer=nullptr;
    inline static std::pair<Type,std::string> the_wrong_index=std::pair<Type,std::string>();
    inline static Type the_wrong_iterator=Type();
    inline static std::map<size_t,size_t> COW_identifiers;
    size_t the_capacity=0,the_size=0,ID=0;
};
#endif