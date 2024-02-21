//
// Created by ByteDance on 2024/2/18.
//

#ifndef DATAFRAME_FEATUREVALUE_H
#define DATAFRAME_FEATUREVALUE_H

#include <algorithm>
#include <exception>
#include <string_view>
#include <string>
#include <type_traits>
#include "absl/types/variant.h"

template<typename T>
struct is_int : std::false_type {};

template<>
struct is_int<int> : std::true_type {};

template<typename T>
struct is_string : std::false_type {};

template<>
struct is_string<std::string> : std::true_type {};

template<typename T>
struct is_vector_int : std::false_type {};

template<>
struct is_vector_int<std::vector<int>> : std::true_type {};


template<typename T>
struct is_map_int_string : std::false_type {};

template<>
struct is_map_int_string<std::unordered_map<int, std::string>> : std::true_type {};


class DataFrame;

class FeatureValue {
    friend class DataFrame;
public:
    enum class valType {
        MAP_INT_STRING,
        LIST_INT,
        INVALID,
        INT,
        STRING
    };

    FeatureValue():val_type_(valType::INVALID),df(nullptr) {};

    FeatureValue(FeatureValue&& other) = default;

    explicit FeatureValue(valType valType):val_type_(valType),df(nullptr) {};

    valType ValType() {
        return val_type_;
    };

    std::string getValTypeAsString() {
        switch (val_type_) {
            case valType::MAP_INT_STRING:
                return "MAP_INT_STRING";
            case valType::LIST_INT:
                return "LIST_INT";
            case valType::INT:
                return "INT";
            case valType::STRING:
                return "STRING";
            default:
                return "INVALID";
        }
    }

    void setValType(valType valType) {
        val_type_ = valType;
    }
    virtual ~FeatureValue() = default;
protected:
    valType val_type_;
    absl::variant<int, std::string, std::vector<int>, std::unordered_map<int, std::string>> data_;
    DataFrame* df{};
};


template <typename T>
FeatureValue::valType getValType()
{
    if constexpr(is_vector_int<T>::value)
    {
        return FeatureValue::valType::LIST_INT;
    }
    else if constexpr (is_map_int_string<T>::value)
    {
        return FeatureValue::valType::MAP_INT_STRING;
    }
    else if constexpr (is_int<T>::value) {
        return FeatureValue::valType::INT;
    }
    else if constexpr (is_string<T>::value) {
        return FeatureValue::valType::STRING;
    }
    else {
        return FeatureValue::valType::INVALID;
    }
}



template <typename T>
class ComplexFeatureValue : public FeatureValue {
public:
    ComplexFeatureValue():FeatureValue(getValType<T>()){};
    ComplexFeatureValue(const ComplexFeatureValue& other ) = delete;
    ComplexFeatureValue(ComplexFeatureValue&& other)  noexcept = default;
    ComplexFeatureValue& operator= (const ComplexFeatureValue) = delete;

    T value_;
    T& getValue() {
        return value_;
    }
    void setValue(T&& val) {
//        assert(&value_ != nullptr);
        value_ = val;
    }
};


template<class T>
bool setValue(FeatureValue* fv, T&& val) {
    auto cfv = static_cast<ComplexFeatureValue<T>*>(fv);
    cfv->setValue(std::forward<T>(val));
    return true;
}



template< class T >
T& getValue(FeatureValue* fv) {
    auto cfv = static_cast<ComplexFeatureValue<T>*>(fv);
    return cfv->getValue();
}


#endif //DATAFRAME_FEATUREVALUE_H
