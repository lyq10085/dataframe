#include "dataframe.h"

size_t DataFrame::appendColumn(const std::string &name, const ColumnVector &values) {
    ColumnVector& origin = addColumn(name);
    origin.insert(origin.end(), values.begin(), values.end());
}

ColumnVector& DataFrame::addColumn(const std::string& name) {
    auto iter = column_table_.find(name);
    if (iter != column_table_.end()) {
        return data_[iter->second];
    }

    data_.emplace_back(ColumnVector());
    column_table_.emplace(name, data_.size() - 1);
    column_names_.emplace_back(name);
    return data_.back();
}

