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

        PersistManager(const PersistManager&) = delete;
        PersistManager & operator=(const PersistManager &) = delete;
        PersistManager(PersistManager&&) = delete;

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
                _instance = std::unique_ptr<PersistManager>(new PersistManager);
                _instance.get()->_root_folder = PersistManager::_default_root_folder;
            }
            return _instance.get();
        }

        bool load() { return false; }   // ...
        bool save() { return false; }   // ...

        const std::string get_stream_name(const std::string class_name, const std::string object_key) const
        { 
            std::string filename = _root_folder + class_name + "_ " + object_key + ".txt";
            return filename;
        } 

    private:
        std::string                                 _root_folder;
        std::map<std::string, std::string>          _files;   // key=class_name+object_name, value=stream file name
        static std::unique_ptr<PersistManager>      _instance;
        static std::string                          _default_root_folder;
    };

    std::unique_ptr<PersistManager> PersistManager::_instance = nullptr;
    std::string PersistManager::_default_root_folder = ".\\";
}
#endif
