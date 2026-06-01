#pragma once
#include "CompoundTag.h"
#include "ListTag.h"
#include "Tag.h"
#include <string>
#include <vector>
#include <stdexcept>

// this is the translation of net.minecraft.server.MojangsonParser (Spigot 1.8)
// from java to c++


class MojangsonParseException : public std::runtime_error {
public:
    explicit MojangsonParseException(const std::string& msg) : std::runtime_error(msg) {}
    explicit MojangsonParseException(const std::wstring& msg)
        : std::runtime_error(std::string(msg.begin(), msg.end())) {}
};

class MojangsonTypeParser {
public:
    std::wstring a; 
    virtual Tag* parse() = 0; 
    virtual ~MojangsonTypeParser() {}
};

class MojangsonParser {
public:
    
    static CompoundTag* parse(const std::wstring& s);
    static int countTopLevel(const std::wstring& s);

    static MojangsonTypeParser* createParser(const std::wstring& name, const std::wstring& value);

    static MojangsonTypeParser* createParser(const std::wstring* arr, int len);

public:

    static MojangsonTypeParser* parseEntry(const std::wstring& s, bool flag);

    static std::wstring getEntry(const std::wstring& s, bool flag);

    static std::wstring getString(const std::wstring& s, int i);

    static std::wstring getKey(const std::wstring& s, bool flag);

    static std::wstring getValue(const std::wstring& s, bool flag);

    static int locateSeparator(const std::wstring& s, wchar_t c0);

    static bool isEscaped(const std::wstring& s, int i);

    static bool matchesSimpleIntArray(const std::wstring& s);
};


class MojangsonPrimitiveParser : public MojangsonTypeParser {
public:
    std::wstring b; 

    MojangsonPrimitiveParser(const std::wstring& name, const std::wstring& value) {
        a = name;
        b = value;
    }

    Tag* parse() override;

private:
    static bool matchesDouble(const std::wstring& s);   
    static bool matchesFloat(const std::wstring& s);    
    static bool matchesByte(const std::wstring& s);     
    static bool matchesLong(const std::wstring& s);     
    static bool matchesShort(const std::wstring& s);    
    static bool matchesInt(const std::wstring& s);      
    static bool matchesDecimal(const std::wstring& s);  
};


class MojangsonListParser : public MojangsonTypeParser {
public:
    std::vector<MojangsonTypeParser*> b;

    MojangsonListParser(const std::wstring& name) { a = name; }

    Tag* parse() override;

    ~MojangsonListParser() {
        for (auto* p : b) delete p;
    }
};


class MojangsonCompoundParser : public MojangsonTypeParser {
public:
    std::vector<MojangsonTypeParser*> b;

    MojangsonCompoundParser(const std::wstring& name) { a = name; }

    Tag* parse() override;

    ~MojangsonCompoundParser() {
        for (auto* p : b) delete p;
    }
};