#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TTest<T> 
//
// TTest handle unit testing on T
// T a class containing multiple tests
//
// cmd_parser parse main() arguments
// Logger log message to a file
//
//
#ifndef _AL_CHESS_UNITTEST_H
#define _AL_CHESS_UNITTEST_H

#include <time.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <chrono>

namespace unittest
{
    template <typename T> class TTest;

    template <typename T>
    struct test_definition
    {
        T*          pT;                 // a concrete object of T
        bool        (T::*f)(uint32_t);  // member function of T to test
        uint32_t    id;
        std::string msg_err;
        std::string name;
        bool        result;
        bool        is_exception;
        std::chrono::time_point<std::chrono::system_clock> _start;
        std::chrono::time_point<std::chrono::system_clock> _end;
    };

    template <typename T>
    class TTest
    {
    public:
        TTest() {}
        TTest(std::string name) : _name(name) {}

        const bool run();

        void reset();
        void add_test(T* p, bool (T::*f)(uint32_t), uint32_t id, std::string msg_err, std::string testname = "");

        const size_t num_failed() const;
        const size_t num_except() const;
        const std::string report(bool detailed = false) const;

        std::vector<test_definition<T>> get_tests() const;

        const bool status()       const { return _status; }
        const bool has_except()   const { return _has_except; }
        const std::string name()  const { return _name; }

    private:
        const bool checkT(T* p, bool (T::*f)(uint32_t), uint32_t id)
        {
            return (p->*f)(id);
        }

        std::string _name;
        bool        _status;
        bool        _has_except;
        std::vector<test_definition<T>> _tests_def;
    };

    template <typename T>
    inline std::vector<test_definition<T>> TTest<T>::get_tests() const { return _tests_def; }

    template <typename T>
    inline const size_t TTest<T>::num_except() const
    {
        size_t n = 0;
        for (auto &v : _tests_def)
        {
            if (v.is_exception == true) n++;
        }
        return n;
    }

    template <typename T>
    inline const size_t TTest<T>::num_failed() const
    {
        size_t n = 0;
        for (auto &v : _tests_def)
        {
            if (v.result == false) n++;
        }
        return n;
    }

    template <typename T>
    inline const std::string TTest<T>::report(bool detailed) const
    {
        std::stringstream ss;
        ss << "Number check/failed/except: " << _tests_def.size() << "-" << num_failed() << "-" << num_except();
        unittest::Logger::instance()->log(ss.str());
        ss << std::endl;

        if (detailed)
        {
            size_t n = 0;
            for (auto &v : _tests_def)
            {
                std::chrono::duration<double> elapsed_seconds = v._end - v._start;
                std::stringstream ss_detail;
                if (v.result)
                {
                    ss_detail << "OK: (id=" << v.id << ") (elapsed sec= " << elapsed_seconds.count() << ")";
                    ss_detail << " [" << v.name << "]";
                }
                else
                {
                    std::stringstream ss_detail;
                    if (!v.is_exception)
                        ss_detail << "Error: (id=" << v.id << ") (msg=" << v.msg_err << ") (elapsed sec= " << elapsed_seconds.count() << ")";
                    else
                        ss_detail << "Except: (id=" << v.id << ") (msg=" << v.msg_err << ") (elapsed sec= " << elapsed_seconds.count() << ")";
                    ss_detail << " [" << v.name << "]";
                }        
                unittest::Logger::instance()->log(ss_detail.str());
                ss_detail << std::endl;
                ss << ss_detail.str();
            }
        }
        return ss.str();
    }

    template <typename T>
    inline const bool TTest<T>::run()
    {
        _status = true;
        _has_except = false;
        for (auto &v : _tests_def)
        {
            try
            {
                v._start = std::chrono::system_clock::now();
                bool result = checkT(v.pT, v.f, v.id);
                v._end = std::chrono::system_clock::now();

                v.result = result;
                v.is_exception = false;
                if (result == false)  _status = false;
            }
            catch (...)
            {
                v._end = std::chrono::system_clock::now();
                v.result = false;
                v.is_exception = true;
                _status = false;
                _has_except = true;
            }
        }
        return _status;
    }

    template <typename T>
    inline void TTest<T>::reset() { _tests_def.clear(); }

    template <typename T>
    inline void TTest<T>::add_test(T* p, bool (T::*f)(uint32_t), uint32_t id, std::string msg_err, std::string testname)
    {
        _tests_def.push_back(test_definition<T>{ p, f, id, msg_err, testname, true, false });
    }


    //--------------
    class cmd_parser 
    {
    public:
        cmd_parser(int &argc, char **argv)
        {
            for (int i = 1; i < argc; ++i)
                this->tokens.push_back(std::string(argv[i]));
        }

        const std::string& get_option(const std::string &option) const 
        {
            auto &itr = std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()) 
            {
                return *itr;
            }
            static const std::string s_empty = std::string("");
            return s_empty;
        }

        bool has_option(const std::string &option) const 
        {
            return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
        }

    private:
        std::vector<std::string> tokens;
    };

    //--------------
    class Logger
    {
    private:
        Logger() = default;
        static const std::string default_logfile;

    public:
        Logger(const Logger&) = delete;
        Logger & operator=(const Logger &) = delete;
        Logger(Logger&&) = delete;

        static Logger* instance()
        {
            if (!_instance.operator bool())
            {
                _instance = std::unique_ptr<Logger>(new Logger);

                _instance->_fstream.open(default_logfile.c_str(), std::ofstream::out | std::ofstream::app);
                if (!_instance->_fstream.good())
                {
                    //...
                }
            }
            return _instance.get();
        }

        bool set_file(const std::string log_file)
        {
            if (_instance.operator bool())
            {
                if (_instance->_fstream.is_open())
                    _instance->_fstream.close();

                _instance->_fstream.open(log_file.c_str(), std::ofstream::out | std::ofstream::app);
                if (_instance->_fstream.good())
                {
                    return true;
                }
                else
                {
                    _instance->_fstream.open(default_logfile.c_str(), std::ofstream::out | std::ofstream::app);
                    if (_instance->_fstream.good())
                        return true;
                }
            }
            return false;
        }

        void close()
        {
            if (_instance.operator bool())
            {
                if (_instance->_fstream.is_open())
                    _instance->_fstream.close();

                Logger* p = _instance.release();
                delete p;
            }
        }

        void log(const std::string s) const
        {
            if (_instance.operator bool())
            {
                if (_instance->_fstream.is_open())
                {
                    time(&_instance->_rawtime);
                    _instance->_timeinfo = localtime(&_instance->_rawtime);
                    if (strftime(_instance->_time_buffer, 80, "%I:%M:%S:%p", _instance->_timeinfo))
                        _instance->_fstream << _instance->_time_buffer << " : ";

                    _instance->_fstream << s << std::endl;
                }
            }
            else
            {
            }
        }

        ~Logger() 
        {
            if (_instance.operator bool())
            {
                if (_instance->_fstream.is_open())
                    _instance->_fstream.close();

                _instance.release();
            }
        }

    private:
        time_t          _rawtime;
        struct tm *     _timeinfo;
        char            _time_buffer[80];

        std::ofstream   _fstream;
        static std::unique_ptr<Logger>  _instance;
    };

    std::unique_ptr<Logger> Logger::_instance = nullptr;
    const std::string Logger::default_logfile = ".\\chess_test_log.txt";
};

#endif
