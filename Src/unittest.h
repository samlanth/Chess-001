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
    struct check_test_def
    {
        T*          pT;                 // a concrete object of T
        bool        (T::*f)(uint32_t);  // member function of T to test
        uint32_t    id;
        std::string msg_err;
        bool        result;
        bool        is_except;
    };

    template <typename T>
    class TTest
    {
    public:
        TTest() {}
        TTest(std::string name) : _name(name) {}

        const bool run();

        void reset();
        void add(T* p, bool (T::*f)(uint32_t), uint32_t id, std::string msg_err);

        const size_t num_failed() const;
        const size_t num_except() const;
        const std::string report(bool detailed = false) const;

        std::vector<check_test_def<T>> get_checks() const;

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
        std::vector<check_test_def<T>> _checks_def;
    };

    template <typename T>
    inline std::vector<check_test_def<T>> TTest<T>::get_checks() const { return _checks_def; }

    template <typename T>
    inline const size_t TTest<T>::num_except() const
    {
        size_t n = 0;
        for (auto &v : _checks_def)
        {
            if (v.is_except == true) n++;
        }
        return n;
    }

    template <typename T>
    inline const size_t TTest<T>::num_failed() const
    {
        size_t n = 0;
        for (auto &v : _checks_def)
        {
            if (v.result == false) n++;
        }
        return n;
    }

    template <typename T>
    inline const std::string TTest<T>::report(bool detailed) const
    {
        std::stringstream ss;
        ss << "Number check/failed/except: " << _checks_def.size() << "-" << num_failed() << "-" << num_except();
        unittest::Logger::instance()->log(ss.str());
        ss << std::endl;

        if (detailed)
        {
            size_t n = 0;
            for (auto &v : _checks_def)
            {
                if (v.result == false)
                {
                    std::stringstream ss_detail;
                    if (!v.is_except)
                        ss_detail << "Failed error : (id=" << v.id << ") (msg=" << v.msg_err << ");";
                    else
                        ss_detail << "Failed except: (id=" << v.id << ") (msg=" << v.msg_err << ")";
                    unittest::Logger::instance()->log(ss_detail.str());

                    ss_detail << std::endl;
                    ss << ss_detail.str();
                }
            }
        }
        return ss.str();
    }

    template <typename T>
    inline const bool TTest<T>::run()
    {
        _status = true;
        _has_except = false;
        for (auto &v : _checks_def)
        {
            try
            {
                bool result = checkT(v.pT, v.f, v.id);
                v.result = result;
                v.is_except = false;
                if (result == false)  _status = false;
            }
            catch (...)
            {
                v.result = false;
                v.is_except = true;
                _status = false;
                _has_except = true;
            }
        }
        return _status;
    }

    template <typename T>
    inline void TTest<T>::reset() { _checks_def.clear(); }

    template <typename T>
    inline void TTest<T>::add(T* p, bool (T::*f)(uint32_t), uint32_t id, std::string msg_err)
    {
        _checks_def.push_back(check_test_def<T>{ p, f, id, msg_err, true, false });
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
            return "";
        }

        bool has_option(const std::string &option) const 
        {
            return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
        }

    private:
        std::vector <std::string> tokens;
    };

    //--------------
    class Logger
    {
    public:
        static Logger* instance()
        {
            if (!_instance.operator bool())
            {
                _instance = std::make_unique<Logger>();
            }
            return _instance.get();
        }

        void set_file(const std::string log_file)
        {
            if (_instance->_fstream.is_open())
                _instance->_fstream.close();

            _instance->_fstream.open(log_file.c_str(), std::ofstream::out | std::ofstream::app);
            // if fail...
        }

        void log(const std::string s) const
        {
            if (_instance->_fstream.is_open())
            {
                time_t rawtime;
                struct tm * timeinfo;
                char buffer[80];
                time(&rawtime);
                timeinfo = localtime(&rawtime);

                if (strftime(buffer, 80, "%I:%M:%S:%p", timeinfo))
                    _instance->_fstream << buffer << " : ";
                _instance->_fstream << s << std::endl;
            }
        }

        ~Logger() 
        {
            if (_instance->_fstream.is_open())
                _instance->_fstream.close();
        }

        Logger() {}

    private:
        std::ofstream   _fstream;
        static std::unique_ptr<Logger>  _instance;
    };

    std::unique_ptr<Logger> Logger::_instance = nullptr;
};

#endif
