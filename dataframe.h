//
// Created by ByteDance on 2024/2/2.
//

#ifndef DATAFRAME_DATAFRAME_H
#define DATAFRAME_DATAFRAME_H

//
// Created by ByteDance on 2024/2/2.
//



#include <algorithm>
#include <exception>
#include "slice.h"

class Arena;

class DataFrame {
    class FeatureValue;
public:
    DataFrame() = default;
    ~DataFrame() {
        delete arena_;
    }
public:
private:
    Arena* arena_;
};



class FeatureValue {
public:
    enum class fvType {
        COMPLEX,
        TRIVIAL
    };
    enum class valType {
        MAP_INT_STRING,
        LIST_INT,
        INT,
        STRING,
        INVALID,
    };

    FeatureValue(): fv_type_(fvType::TRIVIAL), val_type_(valType::INVALID), df(nullptr) {};

    FeatureValue(FeatureValue&& other) = default;

    explicit FeatureValue(valType valType):val_type_(valType),df(nullptr) {
        // static assert
        switch (val_type_) {
            case valType::MAP_INT_STRING:
            case valType::LIST_INT:
                fv_type_ = fvType::COMPLEX;
            default:
                fv_type_ = fvType::TRIVIAL;
        }
    };

    explicit FeatureValue(fvType fvType):fv_type_(fvType), val_type_(valType::INVALID), df(nullptr) {};

    fvType getFvType() {
        return  fv_type_;
    }
    valType getType() {
        return val_type_;
    };

    void setFvType(fvType fvType) {
        fv_type_ = fvType;
    }

    void setValType(valType valType) {
        val_type_ = valType;
    }

    virtual ~FeatureValue() = default;
protected:
    fvType fv_type_;
    valType val_type_;
    DataFrame* df{};
};


template <typename T>
class ComplexFeatureValue : public FeatureValue {
public:

    T value_;

    T& getValue() {
        return value_;
    }

    void setValue(T& val) {
        value_ = std::move(val);
    }

    // move only
    ComplexFeatureValue(): FeatureValue(fvType::COMPLEX) {};

    explicit ComplexFeatureValue(valType valType): FeatureValue(fvType::COMPLEX) {
        switch (valType) {
            case valType::MAP_INT_STRING:
            case valType::LIST_INT:
                val_type_ = valType;
                break;
            default:
                throw std::logic_error("non complex value");
        }
    };

    ComplexFeatureValue(const ComplexFeatureValue& other ) = delete;
    ComplexFeatureValue(ComplexFeatureValue&& other)  noexcept = default;
    ComplexFeatureValue& operator= (const ComplexFeatureValue) = delete;


};

// trivial type mem management is covered by dataframe's private arena
class TrivialFeatureValue : public FeatureValue {
public:

    TrivialFeatureValue(): FeatureValue(fvType::TRIVIAL) {};
    explicit TrivialFeatureValue(valType valType): FeatureValue(fvType::TRIVIAL) {
        switch (valType) {
            case valType::MAP_INT_STRING:
            case valType::LIST_INT:
                throw std::logic_error("non trivial value");
            default:
                val_type_ = valType;
        }
    }
    // move only
    TrivialFeatureValue(const TrivialFeatureValue& other) = delete;
    TrivialFeatureValue&  operator=(const TrivialFeatureValue) = delete;
    TrivialFeatureValue(TrivialFeatureValue&& other) noexcept {
        data_ = other.data_;
    }

    void setValue(std::string_view data) {
        data_ = data;
    }

    template<class T>
    void setValue(const T* val) {
        setValue(std::string_view (reinterpret_cast<const char *>(val), sizeof (T)));
    }


    template<>
    void setValue<std::string_view >(const std::string_view * val) {
        setValue(*val);
    }

    [[nodiscard]]
    std::string_view getValue() const {
        return data_;
    }

    template<class T>
    void getValue(T*& val) const {
        assert(data_.size() == sizeof(T));
        val = reinterpret_cast<const T*>(data_.data());
    }

    template<>
    void getValue<std::string_view >(std::string_view *& val) const {
        *val = data_;
    }

    // data_ is managed by arena
    ~TrivialFeatureValue() override = default;


private:
    std::string_view data_;
};


// setValue: a unified function signature to set FeatureValue

template<class T, std::enable_if_t<!std::is_pointer<T>::value, int> = 0>
bool setValue(FeatureValue* fv, T&& val) {
    assert(fv->getFvType() == FeatureValue::fvType::COMPLEX);
    auto cfv = dynamic_cast<ComplexFeatureValue<T>*>(fv);
    if(cfv != nullptr) {
        cfv->setValue(val);
        return true;
    }
    return false;
}


template<class T>
bool setValue(FeatureValue* fv, const T* val) {
    assert(fv->getFvType() == FeatureValue::fvType::TRIVIAL);
    auto tfv = dynamic_cast<TrivialFeatureValue*>(fv);
    if(tfv != nullptr) {
        tfv->template setValue<T>(val);
        return true;
    }
    return false;
}

bool setValue(FeatureValue* fv, const char* val, size_t len) {
    assert(fv->getFvType() == FeatureValue::fvType::TRIVIAL);
    auto tfv = dynamic_cast<TrivialFeatureValue*>(fv);
    if(tfv != nullptr) {
        tfv->setValue(std::string_view(val, len));
        return true;
    }
    return false;
}



template< class T, std::enable_if_t<!std::is_pointer<T>::value, int> = 0>
bool getValue(FeatureValue* fv, T& val) {
    auto cfv = dynamic_cast<ComplexFeatureValue<T>*>(fv);
    if(cfv != nullptr) {
        val = cfv->getValue();
        return true;
    }
    return false;
}


template< class T, std::enable_if_t<!std::is_pointer<T>::value, int> = 0>
bool getValue(FeatureValue* fv, T* val) {
    auto tfv = dynamic_cast<TrivialFeatureValue*>(fv);
    if(tfv != nullptr) {
        tfv->template getValue<T>(val);
    }
    return false;
}





#endif //DATAFRAME_DATAFRAME_H
