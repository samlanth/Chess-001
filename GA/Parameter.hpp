#pragma once
//=================================================================================================
//          Modified work is Copyright (C) 2017 Alain Lanthier - All Rights Reserved  
//                      Original source is GALGO 2.0 (see github)         
//=================================================================================================
//
// class BaseParameter<T>;
// class Parameter<T, NBIT> : public BaseParameter<T>;
//
//
#ifndef _AL_CHESS_GA_PARAMETER_H
#define _AL_CHESS_GA_PARAMETER_H

namespace galgo 
{
    int sum(int first) {return first;}
    template <typename...Args> int sum(int first, Args...args) { return first + sum(args...); }

    // Parameter interface
    template <typename T>
    class BaseParameter
    {
    public:
       virtual ~BaseParameter() {}
       virtual std::string encode() const = 0;
       virtual std::string encode(T z) const = 0;
       virtual T decode(const std::string& y) const = 0;
       virtual int size() const = 0;
       virtual const std::vector<T>& getData() const = 0;
    };


    template <typename T, int NBIT>
    class Parameter : public BaseParameter<T>
    {
       template <typename K> friend class Chromosome;

    private:
       std::vector<T> data; // contains lower bound, upper bound and initial value (optional)

    public:
       Parameter() {}
       Parameter(const std::vector<T>& data) 
       {
          if (data.size() < 2) {
             throw std::invalid_argument("Error: in class galgo::Parameter<T,N>, argument must contain at least 2 elements of type T, the lower bound and the upper bound, please adjust.");
          }
          if (data[0] >= data[1]) {
             throw std::invalid_argument("Error: in class galgo::Parameter<T,N>, first argument (lower bound) cannot be equal or greater than second argument (upper bound), please amend.");
          }
          this->data = data;
       }
       // return encoded parameter size in number of bits
       int size() const override {
          return NBIT;
       }
       // return parameter initial data
       const std::vector<T>& getData() const override {
          return data;
       }
    private:
       // encoding random unsigned integer
       std::string encode() const override {
          std::string str = GetBinary(galgo::Randomize<NBIT>::uniform_uint64());
          return str.substr(str.size() - NBIT, NBIT);
       }
       // encoding known unsigned integer
       std::string encode(T z) const override {
          uint64_t value = (uint64_t )(Randomize<NBIT>::MAXVAL * (z - data[0]) / (data[1] - data[0]));
          std::string str = GetBinary(value);
          return str.substr(str.size() - NBIT, NBIT);
       }
       // decoding string to real value
       T decode(const std::string& str) const override {
          return data[0] + (GetValue(str) / static_cast<double>(Randomize<NBIT>::MAXVAL)) * (data[1] - data[0]);
       }
    };

}

#endif
