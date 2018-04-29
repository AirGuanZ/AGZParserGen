#include <fstream>
#include <iterator>
#include <sstream>

NS_BEGIN(AGZ)

template<typename T, typename...Args>
inline Ptr<T> MakePtr(Args&&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

inline bool ReadTxt(const String &filename, String &output)
{
    std::ifstream fin(filename, std::ifstream::in);
    if(!fin)
        return false;
    output = String(std::istreambuf_iterator<char>(fin),
                    std::istreambuf_iterator<char>());
    return true;
}

inline String StrJoin(const Vec<String> &strs, const String &join)
{
    if(strs.empty())
        return "";
    std::stringstream sst;
    sst << strs[0];
    for(size_t i = 1; i != strs.size(); ++i)
        sst << join << strs[i];
    return sst.str();
}

template<typename T>
inline bool WriteBinary(std::ofstream &fout, const T &data)
{
    fout.write((const char*)&data, sizeof(data));
    return !fout.fail();
}

inline bool WriteString(std::ofstream &fout, const String &str)
{
    for(size_t i = 0;i <= str.size(); ++i)
    {
        if(!WriteBinary(fout, str[i]))
            return false;
    }
    return true;
}

template<typename T>
inline bool ReadBinary(std::ifstream &fin, T &data)
{
    fin.read((char*)&data, sizeof(data));
    return !fin.fail();
}

inline bool ReadString(std::ifstream &fin, String &str)
{
    str = "";
    std::stringstream sst;
    while(true)
    {
        char ch;
        if(!ReadBinary(fin, ch))
            return false;
        if(ch == '\0')
        {
            str = sst.str();
            return true;
        }
        sst << ch;
    }
    return false;
}

NS_END(AGZ)
