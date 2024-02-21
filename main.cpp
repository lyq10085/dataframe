#include <iostream>
#include <vector>
#include <unordered_map>
#include "dataframe.h"

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
    std::cout << "alloc 10000000 FeatureValue time cost(arena): " << duration.count() << " ms\n";
    df.appendColumn("test_col", col);


    start = std::chrono::high_resolution_clock::now();
    std::vector<std::shared_ptr<FeatureValue>> origin_col;
    for(int i =0; i < 10000000; i++) {
        std::shared_ptr<FeatureValue> fv;
        switch (i % 4) {
            case 0:
                fv = std::make_shared<ComplexFeatureValue<int>>();
                break;
            case 1:
                fv = std::make_shared<ComplexFeatureValue<std::string>>();
                break;
            case 2:
                fv = std::make_shared<ComplexFeatureValue<std::vector<int>>>();
                break;
            case 3:
                fv = std::make_shared<ComplexFeatureValue<std::unordered_map<int, std::string>>>();
                break;
            default:
                std::cout<<"unreachable"<<std::endl;
        }
        origin_col.push_back(fv);
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "alloc 10000000 FeatureValue time cost(make_shared): " << duration.count() << " ms\n";


    {
        DataFrame df1;
        for(size_t k = 0; k < 4; k++) {
            ColumnVector col;
            for (size_t i = 0; i < 100000; i++) {
                ValuePtr fv;
                std::string s = "hello";
                std::vector<int> vec;
                vec.push_back(1);
                vec.push_back(2);
                vec.push_back(i);
                std::unordered_map<int, std::string> map;
                map[1] = "hello";
                map[2] = "world";
                map[3] = std::to_string(i);
                switch (k % 4) {
                    case 0:
                        fv = df1.Allocate<int>();
                        setValue(fv, 1);
                        break;
                    case 1:
                        fv = df1.Allocate<std::string>();
                        s = "hello";
                        setValue(fv, std::move(s));
                        break;
                    case 2:
                        fv = df1.Allocate<std::vector<int>>();
                        setValue(fv, std::move(vec));
                        break;
                    case 3:
                        fv = df1.Allocate<std::unordered_map<int, std::string>>();
                        setValue(fv, std::move(map));
                        break;
                    default:
                        std::cout << "unreachable" << std::endl;
                }
                col.push_back(fv);
            }
            df1.appendColumn("test_col" + std::to_string(k), col);
            col.clear();
        }
    }


    {
        OldDataFrame df2;
        for(size_t k = 0; k < 4; k++) {
            OldColumnVector col;
            for (size_t i = 0; i < 100000; i++) {
                OldValuePtr fv;
                std::string s = "hello";
                std::vector<int> vec = {1, 2, 3, 4};
                std::unordered_map<int, std::string> map;
                map[1] = "hello";
                map[2] = "world";
                switch (k % 4) {
                    case 0:
                        fv = std::make_shared<ComplexFeatureValue<int>>();
                        setValue(fv.get(), 1);
                        break;
                    case 1:
                        fv = std::make_shared<ComplexFeatureValue<std::string>>();
                        setValue(fv.get(), std::move(s));
                        break;
                    case 2:
                        fv = std::make_shared<ComplexFeatureValue<std::vector<int>>>();
                        setValue(fv.get(), std::move(vec));
                        break;
                    case 3:
                        fv = std::make_shared<ComplexFeatureValue<std::unordered_map<int, std::string>>>();
                        setValue(fv.get(), std::move(map));
                        break;
                    default:
                        std::cout << "unreachable" << std::endl;
                }
                col.push_back(fv);
            }
            df2.appendColumn("test_col" + std::to_string(k), col);
            col.clear();
        }
    }


    return 0;
}
