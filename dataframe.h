//
// Created by ByteDance on 2024/2/2.
//

#ifndef DATAFRAME_DATAFRAME_H
#define DATAFRAME_DATAFRAME_H

//
// Created by ByteDance on 2024/2/2.
//


#include <type_traits>
#include <vector>
#include <algorithm>
#include <exception>
#include "arena.h"
#include "FeatureValue.h"
#include <string>
#include <iostream>
#include <type_traits>


using ValuePtr = FeatureValue*;
using ColumnVector = std::vector<ValuePtr>;

class DataFrame {
public:
    DataFrame():arena_(new Arena),arena_complex_(new Arena) {};
    size_t appendColumn(const std::string& name, const ColumnVector& values);
    ~DataFrame() {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout<<column_names_.size()<<" "<<column_table_.size()<<std::endl;
        column_names_.clear();
        column_table_.clear();
        data_.clear();
        auto start1 = std::chrono::high_resolution_clock::now();
        delete arena_;


        // delete arena_complex_
        // todo: need to deconstruct inner value
        for(auto fv : fv_index_) {
            switch (fv.first) {
                case FeatureValue::valType::LIST_INT: {
                    ComplexFeatureValue<std::vector<int>> *vptr = nullptr;
                    vptr = reinterpret_cast<ComplexFeatureValue<std::vector<int>> *>(fv.second);
                    vptr->~ComplexFeatureValue<std::vector<int>>();
                }
                    break;
                case FeatureValue::valType::MAP_INT_STRING: {
                    ComplexFeatureValue<std::unordered_map<int, std::string>>* vptr;
                    vptr = reinterpret_cast<ComplexFeatureValue<std::unordered_map<int, std::string>> *>(fv.second);
                    vptr->~ComplexFeatureValue<std::unordered_map<int, std::string>>();
                }
                    break;
                case FeatureValue::valType::STRING: {
                    ComplexFeatureValue<std::string>* vptr;
                    vptr = reinterpret_cast<ComplexFeatureValue<std::string>*>(fv.second);
                    vptr->~ComplexFeatureValue<std::string>();
                }
                    break;
                default:

                    throw std::logic_error("not a non-pod object");
            }
        }
        delete arena_complex_;


        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::chrono::duration<double, std::milli> duration2 = end - start1;
        std::cout << "df deconstruct time cost(arena): " << duration.count() << " ms\n";
        std::cout<< "del arena time cost:"<< duration2.count() << "ms\n";
    }
public:
    template<class T>
    ValuePtr Allocate() {
        ComplexFeatureValue<T>* fv;
        if(std::is_pod<T>::value) {
            auto *buffer = arena_->Allocate(sizeof(ComplexFeatureValue<T>));
            fv = new(buffer) ComplexFeatureValue<T>();
            fv->setValType(getValType<T>());
            fv->df = this;
        } else {
            auto* buffer = arena_complex_->Allocate(sizeof (ComplexFeatureValue<T>));
            fv = new(buffer) ComplexFeatureValue<T>();
            fv->setValType(getValType<T>());
            fv->df = this;
            fv_index_.push_back({getValType<T>(), buffer});
        }
        return fv;
    }
    ColumnVector& addColumn(const std::string& name);
private:

    // list of column names
    std::vector<std::string> column_names_;
    // column name to vector index in data
    std::unordered_map<std::string, size_t> column_table_;
    // vector of column vectors
    std::vector<ColumnVector> data_;
    std::unordered_map<std::string, size_t> key_to_row_num_;
    std::vector<std::string> keys_;
    std::string workspace_;
    Arena* arena_; // for fv contains pod value
    Arena* arena_complex_{}; // for fv contains non-pod value
    std::vector<std::pair<FeatureValue::valType, char*>> fv_index_;
};


using OldValuePtr = std::shared_ptr<FeatureValue>;
using OldColumnVector = std::vector<OldValuePtr>;

class OldDataFrame {
public:
    OldDataFrame() = default;
    size_t appendColumn(const std::string& name, const OldColumnVector& values);
    ~OldDataFrame() {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout<<column_names_.size()<<" "<<column_table_.size()<<std::endl;
        column_names_.clear();
        column_table_.clear();
        data_.clear();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "df deconstruct time cost(make_shared): " << duration.count() << " ms\n";
    }
private:
    OldColumnVector& addColumn(const std::string& name);
    // list of column names
    std::vector<std::string> column_names_;
    // column name to vector index in data
    std::unordered_map<std::string, size_t> column_table_;
    // vector of column vectors
    std::vector<OldColumnVector> data_;
    std::unordered_map<std::string, size_t> key_to_row_num_;
    std::vector<std::string> keys_;
    std::string workspace_;
};


#endif //DATAFRAME_DATAFRAME_H
