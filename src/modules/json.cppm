module;
#ifndef module

export module ondra.toolbox.json;

import ondra.toolbox.utf8;
import ondra.toolbox.flatmap;


import <variant>;
import <vector>;
import <unordered_map>;
import <string>;
import <format>; 
import <optional>;
import <exception>;
import <charconv>;
import <algorithm>;
import <format>;
import <limits>;
#endif

export class Json ;

constexpr  bool is_digit(char c){return c>='0' && c <='9';};

export class JsonNumber : public std::string {
public:
    
    JsonNumber() = default;
    JsonNumber(std::string_view text):std::string(is_valid_number(text)?text:std::string_view()) {}

    template<typename T>
    requires(std::is_integral_v<T> && std::is_arithmetic_v<T>)
    JsonNumber(T val):std::string(std::to_string(val)) {};
    template<typename T>
    requires(std::is_floating_point_v<T>)
    JsonNumber(T val):std::string(std::format("{:.12g}", val)) {};
    

    template<typename T>
    requires(std::is_integral_v<T> && std::is_arithmetic_v<T>)
    operator T() const {
        if (empty()) return static_cast<T>(0);
        return static_cast<T>(std::strtoll(this->c_str(), nullptr,10));
    }
    template<typename T>
    requires(std::is_floating_point_v<T>)
    operator T() const {
        if (empty()) return std::numeric_limits<T>::signaling_NaN();
        return static_cast<T>(std::strtod(this->c_str(), nullptr));
    }

    static bool is_valid_number(std::string_view text) {
        if (text.empty()) return false;
        std::size_t pos = 0;
        std::size_t tsz = text.size();

        // Optional minus sign
        if (text[pos] == '-') ++pos;
        if (pos >= tsz) return false;

        // Must have at least one digit before decimal point
        if (!is_digit(text[pos])) return false;

        // Leading zero must be alone
        if (text[pos] == '0' && pos + 1 < tsz && is_digit(text[pos + 1])) return false;

        // Consume digits
        while (pos < tsz && is_digit(text[pos])) ++pos;

        // Optional fractional part
        if (pos < tsz && text[pos] == '.') {
            ++pos;
            if (pos >= tsz || !is_digit(text[pos])) return false;
            while (pos < tsz && is_digit(text[pos])) ++pos;
        }

        // Optional exponent part
        if (pos < tsz && (text[pos] == 'e' || text[pos] == 'E')) {
            ++pos;
            if (pos < tsz && (text[pos] == '+' || text[pos] == '-')) ++pos;
            if (pos >= tsz || !is_digit(text[pos])) return false;
            while (pos < tsz && is_digit(text[pos])) ++pos;
        }

        return pos == tsz;
    }
};


using JsonTypes = std::variant<
    std::nullptr_t,
    std::string,
    JsonNumber,
    bool,
    std::vector<Json>,
    FlatMap<std::string, Json> >;


std::string string_from_u8(std::u8string_view str) {
    std::string out(str.size(),0)    ;
    std::transform(str.begin(), str.end(), out.begin(), [](auto x){return static_cast<char>(x);});
    return out;
}

std::string string_from_w(std::wstring_view str) {
    std::string out;
    Utf8<wchar_t>::to_utf8(str.begin(), str.end(), std::back_inserter(out));
    return out;
}


export class Json: public JsonTypes  {
public:
    using JsonTypes::JsonTypes;
    Json() {};
    Json(std::string_view str):JsonTypes(std::string(str)) {}
    Json(std::u8string_view str):JsonTypes(std::string(string_from_u8(str))) {}
    Json(const std::u8string &str):Json(std::u8string_view(str)) {}
    Json(std::wstring_view str):Json(string_from_w(str)) {};
    Json(const std::wstring &str):Json(string_from_w(str)) {};

    using Object = FlatMap<std::string, Json> ;
    using Array = std::vector<Json>;

    static const Json &empty_json() {
        static Json e;
        return e;
    }

    bool is_null() const {return std::holds_alternative<std::nullptr_t>(*this);}
    bool is_bool() const {return std::holds_alternative<bool>(*this);}
    bool is_number() const {return std::holds_alternative<JsonNumber>(*this);}
    bool is_string() const {return std::holds_alternative<std::string>(*this);}    
    bool is_array() const {return std::holds_alternative<Array>(*this);}
    bool is_object() const {return std::holds_alternative<Object>(*this);}

    template<typename T>
    T as() const {
        if constexpr(std::is_same_v<T, bool>) {
            if (is_bool()) {
                return std::get<bool>(*this);
            } else if (is_number()) {
                auto v = static_cast<int>(std::get<JsonNumber>(*this));
                return v != 0;            
            } else if (is_string()) {
                return std::get<std::string>(*this) == "true";
            } else {
                return false;
            }

        } else if constexpr(std::is_arithmetic_v<T>) {
            if (is_bool()) {
                return static_cast<T>(std::get<bool>(*this)?1:0);
            } else if (is_number()) {
                return static_cast<T>(std::get<JsonNumber>(*this));                
            } else if (is_string()) {
                return static_cast<T>(JsonNumber(std::get<std::string>(*this)));
            } 
        } else if constexpr(std::is_convertible_v<std::string_view, T>) {
            if (is_bool()) {
                return T(std::string_view(std::get<bool>(*this)?"true":"false"));
            } else if (is_number()) {
                return T(std::get<JsonNumber>(*this));             
            } else if (is_string()) {
                return std::get<std::string>(*this);
            }
        } else if constexpr(std::is_same_v<T, std::wstring>) {
            auto s = this->as<std::string_view>();
            std::wstring out;
            Utf8<wchar_t>::from_utf8(s.begin(), s.end(),std::back_inserter(out));
            return out;
        } else if constexpr(std::is_same_v<T, std::u8string_view>) {
            auto s = this->as<std::string_view>();
            return std::u8string_view(reinterpret_cast<const char8_t *>(s.data()), s.size());
        } 
        return T();    
    }

    auto as_bool() const {return as<bool>();}
    auto as_int() const {return as<int>();}
    auto as_unsigned_int() const {return as<unsigned int>();}
    auto as_long() const {return as<long>();}
    auto as_unsigned_long() const {return as<unsigned long>();}
    auto as_float() const {return as<float>();}
    auto as_double() const {return as<double>();}
    auto as_text() const {return as<std::string_view>();}
    auto as_wtext() const {return as<std::wstring>();}
    auto as_utf8() const {return as<std::u8string_view>();}
    const JsonNumber &as_number() const {
        static JsonNumber empty;
        if (is_number()) return get<JsonNumber>(*this);
        else return empty;
    }

    const Array &as_array() const {
        if (is_array()) return std::get<Array>(*this);
        else {
            static Array empty;
            return empty;
        }
    }
    const Object &as_object() const {
        if (is_object()) return std::get<Object>(*this);
        else {
            static Object empty;
            return empty;
        }
    }

    const Json &operator[](std::size_t index) const {
        auto &x = as_array();
        if (index >= x.size()) return empty_json();
        return x[index];
    }

    const Json &operator[](std::string_view key) const {
        auto &x = as_object();
        auto iter = x.find(key);   //todo
        if (iter == x.end()) return empty_json();
        return iter->second;
    }
    
    template<std::invocable<Array &> Fn>
    auto update(Fn &&fn) {
        if (!is_array()) *this = Array();
        return fn(std::get<Array>(*this));
    }
    template<std::invocable<Object &> Fn>
    auto update(Fn &&fn) {
        if (!is_array()) *this = Object();
        return fn(std::get<Object>(*this));
    }

    auto push_back(Json &&val) {
        return update([&](Array &x){
            return x.push_back(std::move(val));
        });
    }

    auto push_back(const Json &val) {
        return update([&](Array &x){
            return x.push_back(val);
        });
    }

    auto set(std::string key, Json &&value) {
        return update([&](Object &x){
            return x[std::move(key)] = std::move(value);
        });
    }

    auto set(std::string key, const Json &value) {
        return update([&](Object &x){
            return x[std::move(key)] = value;
        });
    }

    auto set(std::initializer_list<std::pair<std::string_view, Json> > items) {
        return update([&](Object &x){
            for (auto x: items) {
                set(std::string(x.first), x.second);
            };            
        });
    }

    template<std::invocable<char> Fn>
    void serialize(Fn &&fn) const  {
        if (is_null()) write_token(fn,"null");
        else if (is_bool()) write_token(fn, as_bool()?"true":"false");
        else if (is_string()) write_string(fn, as_text());
        else if (is_number()) {
            char buff[50];
            auto n = as_number();
            write_token(fn, n);
        } else if (is_array()) {
            auto &a = as_array();
            fn('[');
            auto iter = a.begin();
            auto end = a.end();
            if (iter != end) {
                iter->serialize(fn);
                ++iter;
                while (iter != end) {
                    fn(',');
                    iter->serialize(fn);
                    ++iter;
                }
            }
            fn(']');
        } else if (is_object()) {
            auto &o = as_object();
            fn('{');
            auto iter = o.begin();
            auto end = o.end();
            if (iter != end) {
                write_string(fn, iter->first);
                fn(':');
                iter->second.serialize(fn);                
                ++iter;
                while (iter != end) {
                    fn(',');
                    write_string(fn, iter->first);
                    fn(':');
                    iter->second.serialize(fn);                
                    ++iter;
                }
            }
            fn('}');            
        }
    }

    class ParseError: public std::exception {
    public:
        virtual const char *what() const noexcept {return "json parse error";}
    };


    template<std::invocable<> Fn>
    requires(std::is_invocable_r_v<std::optional<char>, Fn>)
    static Json parse(Fn &&fn) {     
        char c= read_skip_ws(fn);
        return parse_first_chr(c, fn);
    }

    Json(std::initializer_list<Json> list) {
        bool isobj = std::all_of(list.begin(), list.end(), [](const Json &x){
            if (!x.is_array()) return false;
            auto &arr = x.as_array();            
            return arr.size() == 2 && arr[0].is_string();
        });
        if (isobj) {
            Json::Object obj;
            for (const auto &x: list) {
                auto &arr = x.as_array();
                obj.emplace(arr[0].as_text(), arr[1]);
            }
            *this = std::move(obj);
        } else {
            *this = Json::Array(list.begin(), list.end());
        }
    }


protected:

    using ReadChr = std::optional<char>;

    template<typename Fn>
    static void write_token(Fn &&fn, std::string_view s) {
        for (auto x: s) fn(x);
    }
    template<typename Fn>
    static void write_string(Fn &&fn, std::string_view s) {
        fn('"');
        for (auto x: s) {
            switch (x) {
                case '\n': write_token(fn,"\\n");break;
                case '\r': write_token(fn,"\\r");break;
                case '\t': write_token(fn,"\\t");break;
                case '\f': write_token(fn,"\\f");break;
                case '\b': write_token(fn,"\\b");break;
                case '\\': write_token(fn,"\\\\");break;
                case '\"': write_token(fn,"\\\"");break;
                default: 
                    if (x >= 0 && x < 32) {
                        char buff[6] = "\\u";
                        auto iter = std::format_to_n(buff+2,4,"{:04x}", x);
                        write_token(fn,{buff,iter.out});
                    } else {
                        fn(x);
                    }
            }
        }
        fn('"');
    }
    template<typename Fn>
    static char read_skip_ws(Fn &&fn) {
        ReadChr c = fn();
        while (c && std::isspace(*c)) {
            c = fn();
        }
        if (!c) throw ParseError();
        return *c;
    }
    template<typename Fn>
    static Json parse_first_chr(char &c, Fn &&fn) {
        switch (c) {
            case 't': check(c, fn, "true"); c = 0; return Json(true);
            case 'f': check(c, fn, "false"); c = 0; return Json(false);
            case 'n': check(c, fn, "null"); c = 0; return Json(nullptr);
            case '"': c =0; return Json(parse_string(fn));
            case '[':  {
                Array arr;
                c = read_skip_ws(fn);
                if (c != ']') {
                    arr.push_back(parse_first_chr(c,fn));
                    if (!c) c = read_skip_ws(fn);
                    while (c != ']') {
                        if (c != ',') throw ParseError();
                        c = read_skip_ws(fn);
                        arr.push_back(parse_first_chr(c, fn));
                        if (!c) c = read_skip_ws(fn);
                    }
                }
                c = 0;
                return Json(std::move(arr));                
            }
            case '{': {
                Object obj;
                c = read_skip_ws(fn);
                if (c != '}') {
                    if (c!='"') throw ParseError();
                    while (true) {
                        std::string k = parse_string(fn);
                        c = read_skip_ws(fn);
                        if (c!=':') throw ParseError();
                        c = read_skip_ws(fn);
                        auto v = parse_first_chr(c, fn);
                        if (!c) c = read_skip_ws(fn);
                        if (!obj.emplace(std::move(k), std::move(v)).second) throw ParseError();
                        if (c == ',') {
                            c = read_skip_ws(fn);
                            continue;
                        } else if (c != '}') {
                            throw ParseError();
                        } else {
                            break;
                        }
                    }
                }
                c = 0;
                return Json(std::move(obj));
            }
            default:
                return Json(parse_number(c, fn));
        }
    }

    template<typename Fn>
    static void check(char c, Fn &&fn, std::string_view token) {
        if (c != token[0]) throw ParseError();
        for (auto &x: token.substr(1)) {
            auto cc = fn();
            if (!cc || *cc != x) throw ParseError();            
        }
    }
    template<typename Fn>
    static double parse_number(char &c, Fn &&fn) {
        std::string buff;
        while (is_digit(c) || c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E') {
            buff.push_back(c);
            auto cc = fn();
            c = !cc?' ': *cc;            
        }
        double v;
        auto st = std::from_chars(buff.data(), buff.data()+buff.size(), v);
        if (st.ec != std::errc{} || st.ptr != buff.data()+buff.size()) throw ParseError();
        if (std::isspace(c)) c = 0;
        return v;
    }
    template<typename Fn>
    static std::string parse_string(Fn &&fn) {        
        std::basic_string<char16_t> data;
        char hexbuf[4];
        int m = 0;
        auto cc = fn();
        int surg = 0;
        while (cc && (*cc!='"' || m)) {
            char c = *cc;
            if (m == 0) {
                if (c == '\\') m = -1;
                else data.push_back(c);
            } else if (m == -1) {
                m = 0;
                switch (c) {
                    case 'n': data.push_back('\n');break;
                    case 'r': data.push_back('\r');break;
                    case 't': data.push_back('\t');break;
                    case 'f': data.push_back('\f');break;
                    case 'a': data.push_back('\a');break;
                    case 'u': m = 1; break;
                    default: data.push_back(c);
                }
            } else if (m > 0) {
                if (!std::isxdigit(c)) throw ParseError();
                hexbuf[m-1] = c;
                if (m == 4) {
                    std::uint16_t codepoint;
                    std::from_chars(hexbuf,hexbuf+4, codepoint, 16);                    
                    data.push_back(codepoint);
                    m = 0;
                } else {
                    ++m;
                }                
            }
             cc = fn();
        }
        std::string out;
        Utf8<char16_t>::to_utf8(data.begin(), data.end(), std::back_inserter(out));
        return out;
    }
};