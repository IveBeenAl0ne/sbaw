#include "stdafx.h"
#include "MojangsonParser.h"
#include "ByteTag.h"
#include "ShortTag.h"
#include "IntTag.h"
#include "LongTag.h"
#include "FloatTag.h"
#include "DoubleTag.h"
#include "StringTag.h"
#include "ByteArrayTag.h"
#include "IntArrayTag.h"
#include "ListTag.h"
#include "CompoundTag.h"
#include <cwchar>
#include <cwctype>
#include <algorithm>
#include <sstream>
#include <stack>

using std::wstring;
using std::vector;
using std::stack;

bool MojangsonPrimitiveParser::matchesDouble(const wstring& s) {
    if (s.empty()) return false;
    if (towupper(s.back()) != L'D') return false;
    wstring num = s.substr(0, s.size() - 1);
    if (num.empty()) return false;
    int start = (num[0] == L'-' || num[0] == L'+') ? 1 : 0;
    bool hasDot = false, hasDigit = false;
    for (int i = start; i < (int)num.size(); i++) {
        if (num[i] == L'.') { if (hasDot) return false; hasDot = true; }
        else if (iswdigit(num[i])) hasDigit = true;
        else return false;
    }
    return hasDigit;
}

bool MojangsonPrimitiveParser::matchesFloat(const wstring& s) {
    if (s.empty()) return false;
    if (towupper(s.back()) != L'F') return false;
    wstring num = s.substr(0, s.size() - 1);
    if (num.empty()) return false;
    int start = (num[0] == L'-' || num[0] == L'+') ? 1 : 0;
    bool hasDot = false, hasDigit = false;
    for (int i = start; i < (int)num.size(); i++) {
        if (num[i] == L'.') { if (hasDot) return false; hasDot = true; }
        else if (iswdigit(num[i])) hasDigit = true;
        else return false;
    }
    return hasDigit;
}

bool MojangsonPrimitiveParser::matchesByte(const wstring& s) {
    if (s.empty()) return false;
    if (towupper(s.back()) != L'B') return false;
    wstring num = s.substr(0, s.size() - 1);
    if (num.empty()) return false;
    int start = (num[0] == L'-' || num[0] == L'+') ? 1 : 0;
    if (start == (int)num.size()) return false;
    for (int i = start; i < (int)num.size(); i++)
        if (!iswdigit(num[i])) return false;
    return true;
}

bool MojangsonPrimitiveParser::matchesLong(const wstring& s) {
    if (s.empty()) return false;
    if (towupper(s.back()) != L'L') return false;
    wstring num = s.substr(0, s.size() - 1);
    if (num.empty()) return false;
    int start = (num[0] == L'-' || num[0] == L'+') ? 1 : 0;
    if (start == (int)num.size()) return false;
    for (int i = start; i < (int)num.size(); i++)
        if (!iswdigit(num[i])) return false;
    return true;
}

bool MojangsonPrimitiveParser::matchesShort(const wstring& s) {
    if (s.empty()) return false;
    if (towupper(s.back()) != L'S') return false;
    wstring num = s.substr(0, s.size() - 1);
    if (num.empty()) return false;
    int start = (num[0] == L'-' || num[0] == L'+') ? 1 : 0;
    if (start == (int)num.size()) return false;
    for (int i = start; i < (int)num.size(); i++)
        if (!iswdigit(num[i])) return false;
    return true;
}

bool MojangsonPrimitiveParser::matchesInt(const wstring& s) {
    if (s.empty()) return false;
    int start = (s[0] == L'-' || s[0] == L'+') ? 1 : 0;
    if (start == (int)s.size()) return false;
    for (int i = start; i < (int)s.size(); i++)
        if (!iswdigit(s[i])) return false;
    return true;
}

bool MojangsonPrimitiveParser::matchesDecimal(const wstring& s) {
    if (s.empty()) return false;
    int start = (s[0] == L'-' || s[0] == L'+') ? 1 : 0;
    bool hasDot = false, hasDigit = false;
    for (int i = start; i < (int)s.size(); i++) {
        if (s[i] == L'.') { if (hasDot) return false; hasDot = true; }
        else if (iswdigit(s[i])) hasDigit = true;
        else return false;
    }
    return hasDigit;
}

bool MojangsonParser::matchesSimpleIntArray(const wstring& s) {
    if (s.empty() || s.front() != L'[' || s.back() != L']') return false;
    for (int i = 1; i < (int)s.size() - 1; i++) {
        wchar_t c = s[i];
        if (!iswdigit(c) && c != L'-' && c != L'+' && c != L',' && c != L'|' && !iswspace(c))
            return false;
    }
    return true;
}

Tag* MojangsonPrimitiveParser::parse() {
    wstring val = b;
    wstring result;

    try {
        if (matchesDouble(val))
            return new DoubleTag(L"", stod(val.substr(0, val.size() - 1)));

        if (matchesFloat(val))
            return new FloatTag(L"", stof(val.substr(0, val.size() - 1)));

        if (matchesByte(val))
            return new ByteTag(L"", (byte)(stoi(val.substr(0, val.size() - 1)) & 0xFF));

        if (matchesLong(val))
            return new LongTag(L"", (int64_t)stoll(val.substr(0, val.size() - 1)));

        if (matchesShort(val))
            return new ShortTag(L"", (short)stoi(val.substr(0, val.size() - 1)));

        if (matchesInt(val))
            return new IntTag(L"", stoi(val));

        if (matchesDecimal(val))
            return new DoubleTag(L"", stod(val));

        wstring lower = val;
        transform(lower.begin(), lower.end(), lower.begin(), towlower);
        if (lower == L"true")  return new ByteTag(L"", 1);
        if (lower == L"false") return new ByteTag(L"", 0);
    }
    catch (...) {
        for (int j = 0; j < (int)val.size(); j++) {
            if (j < (int)val.size() - 1 && val[j] == L'\\' && val[j+1] == L'\\') {
                result += L'\\'; j++;
            } else {
                result += val[j];
            }
        }
        return new StringTag(L"", result);
    }

    if (!val.empty() && val.front() == L'[' && val.back() == L']'
        && MojangsonParser::matchesSimpleIntArray(val))
    {
        wstring inner = val.substr(1, val.size() - 2);
        vector<int> ints;
        std::wistringstream ss(inner);
        wstring tok;
        bool failed = false;
        while (getline(ss, tok, L',')) {
            while (!tok.empty() && iswspace(tok.front())) tok.erase(tok.begin());
            while (!tok.empty() && iswspace(tok.back()))  tok.pop_back();
            if (!tok.empty()) {
                try { ints.push_back(stoi(tok)); }
                catch (...) { failed = true; break; }
            }
        }
        if (!failed) {
            intArray ia((int)ints.size());
            for (int i = 0; i < (int)ints.size(); i++) ia.data[i] = ints[i];
            return new IntArrayTag(L"", ia);
        }
    }

    if (val.size() >= 2 && val.front() == L'"' && val.back() == L'"')
        val = val.substr(1, val.size() - 2);

    result.clear();
    for (int j = 0; j < (int)val.size(); j++) {
        if (j < (int)val.size() - 1 && val[j] == L'\\' && val[j+1] == L'\\') {
            result += L'\\'; j++;
        } else {
            result += val[j];
        }
    }

    wstring final_result;
    for (int j = 0; j < (int)result.size(); j++) {
        if (j < (int)result.size() - 1 && result[j] == L'\\' && result[j+1] == L'"') {
            final_result += L'"'; j++;
        } else {
            final_result += result[j];
        }
    }
    return new StringTag(L"", final_result);
}

Tag* MojangsonListParser::parse() {
    ListTag<Tag>* list = new ListTag<Tag>(L"");
    for (auto* parser : b)
        list->add(parser->parse());
    return list;
}

Tag* MojangsonCompoundParser::parse() {
    CompoundTag* compound = new CompoundTag(L"");
    for (auto* parser : b) {
        Tag* tag = parser->parse();
        tag->setName(parser->a);
        compound->put(parser->a, tag);
        delete tag;
    }
    return compound;
}

bool MojangsonParser::isEscaped(const wstring& s, int i) {
    return i > 0 && s[i-1] == L'\\' && !isEscaped(s, i-1);
}

int MojangsonParser::locateSeparator(const wstring& s, wchar_t c0) {
    bool inQuote = true;
    for (int i = 0; i < (int)s.size(); i++) {
        wchar_t c1 = s[i];
        if (c1 == L'"') {
            if (!isEscaped(s, i))
                inQuote = !inQuote;
        } else if (inQuote) {
            if (c1 == c0) return i;
            if (c1 == L'{' || c1 == L'[') return -1;
        }
    }
    return -1;
}

wstring MojangsonParser::getString(const wstring& s, int i) {
    stack<wchar_t> st;
    int j = i + 1;
    bool inQuote = false;
    bool hadQuote = false;
    bool hadContent = false;
    int lastQuoteEnd = -1;

    for (; j < (int)s.size(); j++) {
        wchar_t c0 = s[j];
        if (c0 == L'"') {
            if (isEscaped(s, j)) {
                if (!inQuote)
                    throw MojangsonParseException(L"Illegal use of \\\": " + s);
            } else {
                inQuote = !inQuote;
                if (inQuote && !hadContent) hadQuote = true;
                if (!inQuote) lastQuoteEnd = j;
            }
        } else if (!inQuote) {
            if (c0 == L'{' || c0 == L'[') {
                st.push(c0);
            } else if (c0 == L'}') {
                if (st.empty() || st.top() != L'{')
                    throw MojangsonParseException(L"Unbalanced curly brackets {}: " + s);
                st.pop();
            } else if (c0 == L']') {
                if (st.empty() || st.top() != L'[')
                    throw MojangsonParseException(L"Unbalanced square brackets []: " + s);
                st.pop();
            } else if (c0 == L',' && st.empty()) {
                return s.substr(0, j);
            }
        }

        if (!iswspace(c0)) {
            if (!inQuote && hadQuote && lastQuoteEnd != j)
                return s.substr(0, lastQuoteEnd + 1);
            hadContent = true;
        }
    }
    return s.substr(0, j);
}

wstring MojangsonParser::getKey(const wstring& s, bool flag) {
    wstring trimmed = s;
    while (!trimmed.empty() && iswspace(trimmed.front())) trimmed.erase(trimmed.begin());

    if (flag && (!trimmed.empty() && (trimmed[0] == L'{' || trimmed[0] == L'[')))
        return L"";

    int i = locateSeparator(s, L':');
    if (i == -1) {
        if (flag) return L"";
        throw MojangsonParseException(L"Unable to locate name/value separator for string: " + s);
    }

    wstring key = s.substr(0, i);
    while (!key.empty() && iswspace(key.front())) key.erase(key.begin());
    while (!key.empty() && iswspace(key.back()))  key.pop_back();
    if (key.size() >= 2 && key.front() == L'"' && key.back() == L'"')
        key = key.substr(1, key.size() - 2);
    return key;
}

wstring MojangsonParser::getValue(const wstring& s, bool flag) {
    wstring trimmed = s;
    while (!trimmed.empty() && iswspace(trimmed.front())) trimmed.erase(trimmed.begin());

    if (flag && !trimmed.empty() && (trimmed[0] == L'{' || trimmed[0] == L'['))
        return trimmed;

    int i = locateSeparator(s, L':');
    if (i == -1) {
        if (flag) return s;
        throw MojangsonParseException(L"Unable to locate name/value separator for string: " + s);
    }

    wstring val = s.substr(i + 1);
    while (!val.empty() && iswspace(val.front())) val.erase(val.begin());
    while (!val.empty() && iswspace(val.back()))  val.pop_back();
    return val;
}

wstring MojangsonParser::getEntry(const wstring& s, bool flag) {
    int i = locateSeparator(s, L':');
    int j = locateSeparator(s, L',');

    if (flag) {
        if (i == -1)
            throw MojangsonParseException(L"Unable to locate name/value separator for string: " + s);
        if (j != -1 && j < i)
            throw MojangsonParseException(L"Name error at: " + s);
    } else if (i == -1 || (j != -1 && i > j)) {
        i = -1;
    }

    return getString(s, i);
}

MojangsonTypeParser* MojangsonParser::parseEntry(const wstring& s, bool flag) {
    wstring key   = getKey(s, flag);
    wstring value = getValue(s, flag);
    wstring arr[2] = { key, value };
    return createParser(arr, 2);
}

MojangsonTypeParser* MojangsonParser::createParser(const wstring& name, const wstring& value) {
    wstring s1 = value;
    while (!s1.empty() && iswspace(s1.front())) s1.erase(s1.begin());
    while (!s1.empty() && iswspace(s1.back()))  s1.pop_back();

    if (!s1.empty() && s1.front() == L'{') {
        wstring inner = s1.substr(1, s1.size() - 1);
        if (!inner.empty() && inner.back() == L'}')
            inner = inner.substr(0, inner.size() - 1);

        MojangsonCompoundParser* cp = new MojangsonCompoundParser(name);
        while (!inner.empty()) {
            wstring entry = getEntry(inner, true);
            if (!entry.empty())
                cp->b.push_back(parseEntry(entry, false));
            if ((int)inner.size() < (int)entry.size() + 1) break;
            wchar_t sep = inner[entry.size()];
            if (sep != L',' && sep != L'{' && sep != L'}' && sep != L'[' && sep != L']')
                throw MojangsonParseException(wstring(L"Unexpected token '") + sep + L"'");
            inner = inner.substr(entry.size() + 1);
        }
        return cp;
    }

    if (!s1.empty() && s1.front() == L'[' && !matchesSimpleIntArray(s1)) {
        wstring inner = s1.substr(1, s1.size() - 1);
        if (!inner.empty() && inner.back() == L']')
            inner = inner.substr(0, inner.size() - 1);

        MojangsonListParser* lp = new MojangsonListParser(name);
        while (!inner.empty()) {
            wstring entry = getEntry(inner, false);
            if (!entry.empty())
                lp->b.push_back(parseEntry(entry, true));
            if ((int)inner.size() < (int)entry.size() + 1) break;
            wchar_t sep = inner[entry.size()];
            if (sep != L',' && sep != L'{' && sep != L'}' && sep != L'[' && sep != L']')
                throw MojangsonParseException(wstring(L"Unexpected token '") + sep + L"'");
            inner = inner.substr(entry.size() + 1);
        }
        return lp;
    }

    return new MojangsonPrimitiveParser(name, s1);
}

MojangsonTypeParser* MojangsonParser::createParser(const wstring* arr, int len) {
    return createParser(arr[0], arr[1]);
}

int MojangsonParser::countTopLevel(const wstring& s) {
    int count = 0;
    bool inQuote = false;
    stack<wchar_t> st;

    for (int j = 0; j < (int)s.size(); j++) {
        wchar_t c0 = s[j];
        if (c0 == L'"') {
            if (isEscaped(s, j)) {
                if (!inQuote)
                    throw MojangsonParseException(L"Illegal use of \\\": " + s);
            } else {
                inQuote = !inQuote;
            }
        } else if (!inQuote) {
            if (c0 == L'{' || c0 == L'[') {
                if (st.empty()) count++;
                st.push(c0);
            } else if (c0 == L'}') {
                if (st.empty() || st.top() != L'{')
                    throw MojangsonParseException(L"Unbalanced curly brackets {}: " + s);
                st.pop();
            } else if (c0 == L']') {
                if (st.empty() || st.top() != L'[')
                    throw MojangsonParseException(L"Unbalanced square brackets []: " + s);
                st.pop();
            }
        }
    }

    if (inQuote)
        throw MojangsonParseException(L"Unbalanced quotation: " + s);
    if (!st.empty())
        throw MojangsonParseException(L"Unbalanced brackets: " + s);

    if (count == 0 && !s.empty()) count = 1;
    return count;
}

CompoundTag* MojangsonParser::parse(const wstring& input) {
    wstring s = input;
    while (!s.empty() && iswspace(s.front())) s.erase(s.begin());
    while (!s.empty() && iswspace(s.back()))  s.pop_back();

    if (s.empty() || s.front() != L'{')
        throw MojangsonParseException(L"Invalid tag encountered, expected '{' as first char.");

    if (countTopLevel(s) != 1)
        throw MojangsonParseException(L"Encountered multiple top tags, only one expected");

    MojangsonTypeParser* parser = createParser(L"tag", s);
    Tag* tag = parser->parse();
    delete parser;

    if (tag->getId() != Tag::TAG_Compound) {
        delete tag;
        throw MojangsonParseException(L"Root tag must be a compound tag");
    }

    return static_cast<CompoundTag*>(tag);
}