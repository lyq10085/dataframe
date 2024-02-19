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


using ValuePtr = FeatureValue*;
using ColumnVector = std::vector<ValuePtr>;

class DataFrame {
public:
    DataFrame():arena_(new Arena){};
    size_t appendColumn(const std::string& name, const ColumnVector& values);
    ~DataFrame() {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout<<column_names_.size()<<" "<<column_table_.size()<<std::endl;
        column_names_.clear();
        column_table_.clear();
        data_.clear();
        auto start1 = std::chrono::high_resolution_clock::now();
        delete arena_;
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::chrono::duration<double, std::milli> duration2 = end - start1;
        std::cout << "df deconstruct time cost(arena): " << duration.count() << " ms\n";
        std::cout<< "del arena time cost:"<< duration2.count() << "ms\n";
    }
public:
    template<class T>
    ValuePtr Allocate() {
        auto *fv = (ComplexFeatureValue<T>*)arena_->Allocate(sizeof (ComplexFeatureValue<T>));
        fv->setValType(getValType<T>());
        fv->df = this;
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
    Arena* arena_;
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
