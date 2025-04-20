#pragma once 
#include <optional>
#include <stdexcept>
#include <string>
#ifndef RESULT_UTILS_ISA
#define RESULT_UTILS_ISA
#include <optional>
#include <string>
#include <utility>
#include <stdexcept>

template<typename T>
class Result {
public:
    std::optional<T> value;
    std::optional<std::string> error;
    explicit operator T() const { return value(); }
    static Result Ok(T val) {
        return Result{std::move(val), std::nullopt};
    }

    static Result Err(std::string err) {
        return Result{std::nullopt, std::move(err)};
    }

    Result() = default;


    Result(std::optional<T> val, std::optional<std::string> err)
        : value(std::move(val)), error(std::move(err)) {}

    Result(const T& val) : value(val), error(std::nullopt) {}
    Result(T&& val) : value(std::move(val)), error(std::nullopt) {}

    Result& operator=(const T& val) {
        value = val;
        error.reset();
        return *this;
    }

    Result& operator=(T&& val) {
        value = std::move(val);
        error.reset();
        return *this;
    }

    Result& operator=(const char* err_str) {
        value.reset();
        error = std::string(err_str);
        return *this;
    }

    Result(const Result&) = default;
    Result(Result&&) noexcept = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&&) noexcept = default;

    bool is_ok() const { return value.has_value(); }
    bool is_err() const { return error.has_value(); }

    T unwrap() const {
        if (value) return *value;
        return "unwrap failed: " + error.value_or("unknown error");
    }

    T unwrap_or(T fallback) const {
        return value.value_or(std::move(fallback));
    }

    const T& operator*() const { return *value; }
    T& operator*() { return *value; }

    const T* operator->() const { return &*value; }
    T* operator->() { return &*value; }
};

#endif