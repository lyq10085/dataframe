////
//// Created by ByteDance on 2024/2/2.
////
//
//
//
//// type eros
//#include <algorithm>
//#include <exception>
//
//class Arena;
//
//class DataFrame {
//    class FeatureValue;
//public:
//    DataFrame() = default;
//    ~DataFrame() {
//        delete arena_;
//    }
//public:
//private:
// Arena* arena_;
//};
//
//
//
//class FeatureValue {
//public:
//    enum class fvType {
//        COMPLEX,
//        TRIVIAL
//    };
//    enum class valType {
//        MAP_INT_STRING,
//        LIST_INT,
//        INT,
//        STRING,
//        INVALID,
//    };
//
//    FeatureValue(): fv_type_(fvType::TRIVIAL), val_type_(valType::INVALID), df(nullptr) {};
//
//    FeatureValue(FeatureValue&& other) = default;
//
//    explicit FeatureValue(valType valType):val_type_(valType),df(nullptr) {
//        // static assert
//        switch (val_type_) {
//            case valType::MAP_INT_STRING:
//            case valType::LIST_INT:
//                fv_type_ = fvType::COMPLEX;
//            default:
//                fv_type_ = fvType::TRIVIAL;
//        }
//    };
//
//    explicit FeatureValue(fvType fvType):fv_type_(fvType), val_type_(valType::INVALID), df(nullptr) {};
//
//    fvType getFvType() {
//        return  fv_type_;
//    }
//    valType getType() {
//        return val_type_;
//    };
//
//    void setFvType(fvType fvType) {
//        fv_type_ = fvType;
//    }
//
//    void setValType(valType valType) {
//        val_type_ = valType;
//    }
//
//    virtual ~FeatureValue() = default;
//protected:
//    fvType fv_type_;
//    valType val_type_;
//    DataFrame* df{};
//};
//
//
//template <typename T>
//class ComplexFeatureValue : FeatureValue {
//public:
//
//    T value_;
//
//    T& getValue() {
//        return value_;
//    }
//
//    void setValue(T&& val) {
//        value_ = std::forward<T>(val);
//    }
//
//    // move only
//    ComplexFeatureValue(): FeatureValue(fvType::COMPLEX) {};
//    ComplexFeatureValue(const ComplexFeatureValue& other ) = delete;
//    ComplexFeatureValue(ComplexFeatureValue&& other)  noexcept = default;
//    ComplexFeatureValue& operator= (const ComplexFeatureValue) = delete;
//
//
//};
//
//// trivial type mem management is covered by dataframe's private arena
//class TrivialFeatureValue : FeatureValue {
//public:
//
//    TrivialFeatureValue(): FeatureValue(fvType::TRIVIAL), data_(nullptr) {};
//    // move only
//    TrivialFeatureValue(const TrivialFeatureValue& other) = delete;
//    TrivialFeatureValue&  operator=(const TrivialFeatureValue) = delete;
//    TrivialFeatureValue(TrivialFeatureValue&& other) noexcept {
//        std::swap(other.data_, data_);
//    }
//
//    void setValue(void* val) {
//        data_ = val;
//    }
//
//    void* getValue() {
//        return data_;
//    }
//
//    template<class T, std::enable_if_t<std::is_pointer<T>::value, int> = 0>
//    bool getValue(T& val ) {
//        switch (val_type_) {
//            case valType::INT:
//            case valType::STRING:
//                val = reinterpret_cast<T>(data_);
//                return true;
//            default:
//                return false;
//        }
//    }
//
//    template<class T, std::enable_if_t<std::is_pointer<T>::value, int> = 0>
//    bool setValue(T val) {
//        switch (val_type_) {
//            case valType::INT:
//            case valType::STRING:
//                data_ = reinterpret_cast<void *>(val);
//                return true;
//            default:
//                return false;
//        }
//    }
//
//    // data_ is managed by arena
//    ~TrivialFeatureValue() override = default;
//
//
//private:
//    void* data_ = nullptr;
//};
//
//
//// setValue: a unified function signature to set FeatureValue
//
//// overload 1
//// for complex feature value
//template< class T, std::enable_if_t<!std::is_pointer<T>::value, int> = 0>
//bool setValue(FeatureValue* fv, T&& val) {
//    auto cfv = dynamic_cast<ComplexFeatureValue<T>*>(fv);
//    if(cfv != nullptr) {
//        cfv.setValue(val);
//        return true;
//    }
//    return false;
//}
//
//// overload 2
//// for bare pointer, for trivial feature value
//template< class T, std::enable_if_t<std::is_pointer<T>::value, int> = 0>
//bool setValue(FeatureValue* fv, T val) {
//    auto tfv = dynamic_cast<TrivialFeatureValue*>(fv);
//    if(tfv != nullptr) {
//        tfv->setValue(reinterpret_cast<void *>(val));
//        return true;
//    }
//    return false;
//}
//
//
//
//template< class T, std::enable_if_t<!std::is_pointer<T>::value, int> = 0>
//bool getValue(const FeatureValue* fv, T& val) {
//    auto cfv = dynamic_cast<const ComplexFeatureValue<T>*>(fv);
//    if(cfv != nullptr) {
//        val = cfv->getValue();
//        return true;
//    }
//    return false;
//}
//
//
//template< class T, std::enable_if_t<std::is_pointer<T>::value, int> = 0>
//bool getValue(const FeatureValue* fv, T& val) {
//    auto tfv = dynamic_cast<const TrivialFeatureValue*>(fv);
//    if(tfv != nullptr) {
//        if(tfv->template getValue<T>(val)) {
//            return true;
//        }
//    }
//    return false;
//}
//
//
