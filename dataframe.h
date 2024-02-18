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


using ValuePtr = FeatureValue*;
using ColumnVector = std::vector<ValuePtr>;








class DataFrame {
public:
    DataFrame():arena_(new Arena){};
    size_t appendColumn(const std::string& name, const ColumnVector& values);
    ~DataFrame() {
        column_names_.clear();
        column_table_.clear();
        data_.clear();
        delete arena_;
    }
public:
    template<class T>
    ValuePtr Allocate() {
        auto *fv = (ComplexFeatureValue<T>*)arena_->Allocate(sizeof (ComplexFeatureValue<T>));
        fv->setValType(getValType<T>());
        fv->df = this;
        return fv;
    }

private:
    ColumnVector& addColumn(const std::string& name);

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


#endif //DATAFRAME_DATAFRAME_H
