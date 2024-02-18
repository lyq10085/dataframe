#include <iostream>
#include <vector>
#include <unordered_map>
#include "dataframe.h"
#include "arena.h"

int main() {


    // basic feature value test
    FeatureValue* fv1 = new ComplexFeatureValue<std::vector<int>>();
    auto vec1 = std::vector<int>{1,2,3};
    setValue(fv1, std::move(vec1));
    auto vec1_read = getValue<std::vector<int>>(fv1);


    FeatureValue* fv2 = new ComplexFeatureValue<std::unordered_map<int, std::string>>();
    std::unordered_map<int, std::string> map1;
    map1[1] = "hello";
    map1[2] = "world";
    setValue(fv2, std::move(map1));
    auto map1_read = getValue<std::unordered_map<int, std::string>>(fv2);

    delete fv1;

    delete fv2;

    auto vec2 = std::vector<int>{2,3,5,6,7,8,9,10};
    std::unique_ptr<FeatureValue> fv3 = std::make_unique<ComplexFeatureValue<std::vector<int>>>();
    setValue(fv3.get(), std::move(vec2));
    auto  vec2_read = getValue<std::vector<int>>(fv3.get());


    // in order to get rid of string mem management,
    // string seem to must be copied into feature value
    std::cout << "Hello, World!" << std::endl;


    FeatureValue* fv4 = new ComplexFeatureValue<int>();
    setValue(fv4, 1);
    auto int1_read = getValue<int>(fv4);

    FeatureValue* fv5 = new ComplexFeatureValue<std::string >();
    std::string s1 = "hello world";
    // revisit (not move cause error)
    setValue(fv5, std::move(s1));
    auto s1_read = getValue<std::string>(fv5);


    DataFrame df;
    ValuePtr fv6 = df.Allocate<std::vector<int>>();
    std::vector<int> vec3 = {4,5,6};
    setValue(fv6, std::move(vec3));
    auto vec3_read = getValue<std::vector<int>>(fv6);

    ValuePtr fv7 = df.Allocate<std::unordered_map<int, std::string>>();
    std::unordered_map<int, std::string> map2;
    map2[1] = "hell";
    map2[2] = "no!";
    setValue(fv7, std::move(map2));
    auto map2_read = getValue<std::unordered_map<int, std::string>>(fv7);

    auto start = std::chrono::high_resolution_clock::now();
    ColumnVector col;
    for(size_t i = 0; i < 10000000; i++) {
        ValuePtr fv;
        switch (i % 4) {
            case 0:
                fv = df.Allocate<int>();
                break;
            case 1:
                fv = df.Allocate<std::string>();
                break;
            case 2:
                fv = df.Allocate<std::vector<int>>();
                break;
            case 3:
                fv = df.Allocate<std::unordered_map<int, std::string>>();
                break;
            default:
               std::cout<<"unreachable"<<std::endl;
        }
        col.push_back(fv);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "time cost: " << duration.count() << " ms\n";

    df.appendColumn("test_col", col);

    return 0;
}
