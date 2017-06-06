#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// <... work in progress ...>
//
// PersistManager<>   : Provide io fstream for [class, name] 
//
//
#ifndef _AL_CHESS_PERSIST_H
#define _AL_CHESS_PERSIST_H

#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace chess
{
    class PersistManager
    {
    private:
        PersistManager() = default;

    public:
        ~PersistManager()
        {
            if (_instance.operator bool())
            {
                // ...
                _instance.release();
            }
        }

        static const PersistManager* instance()
        {
            if (!_instance.operator bool())
            {
                std::unique_ptr<PersistManager> _instance(new PersistManager); // work around std::make_unique public ct()
                return _instance.get();
            }
            return _instance.get();
        }

        bool load() { return false; }   // ...
        bool save() { return false; }   // ...

        std::string get_stream_name(std::string class_name, std::string object_name) { return ""; }  // ...

    private:
        std::string                                 _root_folder;
        std::map<std::string, std::string>          _files;   // key=class_name+object_name, value=stream file name
        static std::unique_ptr<PersistManager>      _instance;
    };

    std::unique_ptr<PersistManager> PersistManager::_instance = nullptr;
}
#endif
