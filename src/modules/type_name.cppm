module;


export module ondra.toolbox.type_name;
import <string_view>;
import <source_location>;


/**
@file TypeName.hpp

constexpr replacement for RTTI when you need just name or unique identifier of type

@note Does work well event in MSVC for struct, class or enum. It is not consistent for
template arguments (will generate different names in different compilers)

*/

template<typename __TypeNameArgument__>
struct GenerateTypeName {
    static constexpr std::string_view get_type_name() {
        auto cur = std::source_location::current();
        std::string_view name =  cur.function_name();
        auto pos = name.rfind("__TypeNameArgument__");
        if (pos == name.npos) {
            //msvc
            pos = name.find("GenerateTypeName<");
            if (pos == name.npos) {
                return name;
            }
            auto sn1 = name.substr(pos+17);
            pos = sn1.find(">::get_type_name");
            auto sn2 = sn1.substr(0,pos);
            if (sn1.substr(0,7) == "struct ") sn2 = sn2.substr(7);
            else if (sn1.substr(0,6) == "class ") sn2 = sn2.substr(6);
            else if (sn1.substr(0,5) == "enum ") sn2 = sn2.substr(5);
            return sn2;
            
        } else {
            auto sn1 = name.substr(pos);
            pos = sn1.find(" = ");
            auto sn2 = sn1.substr(pos+3);
            pos = sn2.find_first_of("];");
            return sn2.substr(0,pos);
        }
    }


    static constexpr std::size_t get_hash()  {
         std::string_view s = get_type_name();
        if constexpr(sizeof(std::size_t) == 4) {
            std::size_t h = 2166136261u;
            for (char c : s)
                h = (h ^ static_cast<unsigned char>(c)) * 16777619u;
            return h;
        } else {
            // constexpr FNV-1a 64-bit
            std::size_t h = 1469598103934665603ull;
            for (char c : s)
                h = (h ^ static_cast<unsigned char>(c)) * 1099511628211ull;
            return h;
        }    
    }
    
};

export template<typename T>
constexpr auto type_name = GenerateTypeName<T>::get_type_name();
export template<typename T>
constexpr auto type_name_hash = GenerateTypeName<T>::get_hash();


