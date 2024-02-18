#include <iostream>
#include <vector>
#include <unordered_map>
#include "dataframe.h"

int main() {
    FeatureValue* fv1 = new ComplexFeatureValue<std::vector<int>>();
    fv1->setValType(FeatureValue::valType::LIST_INT);
    auto vec1 = std::vector<int>{1,2,3};
    setValue(fv1, std::move(vec1));
    std::vector<int> vec1_read;
    getValue(fv1, vec1_read);

    FeatureValue* fv2 = new ComplexFeatureValue<std::unordered_map<int, std::string>>();
    fv2->setValType(FeatureValue::valType::MAP_INT_STRING);
    std::unordered_map<int, std::string> map1;
    map1[1] = "hello";
    map1[2] = "world";
    setValue(fv2, std::move(map1));
    std::unordered_map<int, std::string> map1_read;
    getValue(fv2, map1_read);


    delete fv1;

    delete fv2;

    auto vec2 = std::vector<int>{2,3,5,6,7,8,9,10};
    std::unique_ptr<FeatureValue> fv3 = std::make_unique<ComplexFeatureValue<std::vector<int>>>();
    fv3->setValType(FeatureValue::valType::LIST_INT);
    setValue(fv3.get(), std::move(vec2));
    std::vector<int> vec2_read;
    getValue(fv3.get(), vec2_read);


    int num1 = 1024;
    std::unique_ptr<FeatureValue> fv4 = std::make_unique<TrivialFeatureValue>(FeatureValue::valType::INT);
    setValue(fv4.get(), &num1);
    const int* num1_read = nullptr;
    getValue(fv4.get(), num1_read);


    std::string_view  str1("hello world");
    std::unique_ptr<FeatureValue>  fv5 = std::make_unique<TrivialFeatureValue>(FeatureValue::valType::STRING);
    setValue(fv5.get(), &str1);
    std::string_view str1_read;
    getValue(fv5.get(), &str1_read);


    char* bytes = (char *) malloc(sizeof (char )*10);
    std::unique_ptr<FeatureValue>  fv6 = std::make_unique<TrivialFeatureValue>(FeatureValue::valType::STRING);
    setValue(fv5.get(), bytes, 10);
    std::string_view bytes_read;
    getValue(fv5.get(), &bytes_read);

    // in order to get rid of string mem management,
    // string seem to must be copied into feature value
    std::cout << "Hello, World!" << std::endl;


    return 0;
}
