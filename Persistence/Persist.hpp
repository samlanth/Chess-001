#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// PersistManager<>   : Provide io fstream for [class, instance_key] 
//
//
#ifndef _AL_CHESS_PERSISTENCE_PERSIST_HPP
#define _AL_CHESS_PERSISTENCE_PERSIST_HPP

#include <windows.h>    // outch! - CreateDirectory - still no std::filesystem

namespace chess
{
    class PersistManager
    {
    private:
        PersistManager()
        {
            load();
        }

        PersistManager(const PersistManager&) = delete;
        PersistManager & operator=(const PersistManager &) = delete;
        PersistManager(PersistManager&&) = delete;

    public:
        ~PersistManager()
        {
            if (_instance.operator bool())
            {
                save();
                _instance.release();
            }
        }

        void set_root_folder(const std::string& s) { _root_folder = s; }
        std::string get_root_folder() const { return _root_folder; }
        void create_directory(std::string s) const 
        {
            CreateDirectory(s.c_str(), NULL);
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

        const std::string get_stream_name(const std::string class_name, const std::string instance_key) const
        { 
            std::string folder = _root_folder + class_name + "\\";
            if (!(dirExists(folder)))
            {
                create_directory(folder);
            }
            std::string filename = folder + instance_key + ".txt";
            return filename;
        } 

        const std::string get_stream_name(const std::string class1, const std::string class2, const std::string instance_key) const
        {
            std::string folder = _root_folder + class1 + "\\";
            if (!(dirExists(folder)))
            {
                create_directory(folder);
            }
            folder = _root_folder + class1 + "\\" + class2 + "\\";
            if (!(dirExists(folder)))
            {
                create_directory(folder);
            }
            std::string filename = folder + instance_key + ".txt";
            return filename;
        }

        static const std::string create_persist_key()
        {
            _persist_key_counter++;
            std::stringstream ss;
            ss << _persist_key_counter;
            save();                         // save less frequently...memory mapped
            return ss.str();
        }

        static bool save()
        {
            std::string f = _root_folder + "persistmanager" + ".txt";
            std::ofstream   is;
            is.open(f.c_str(), std::fstream::out | std::fstream::trunc);
            if (is.good())
            {
                is << _persist_key_counter; is << " ";
                is << _root_folder;         is << " ";
                is << _default_root_folder; is << " ";                
                is.close();
                return true;
            }
            is.close();
            return false;
        }

        bool load()
        {
            std::string f = _root_folder + "persistmanager" + ".txt";
            std::ifstream   is;
            is.open(f.c_str(), std::fstream::in);
            if (is.good())
            {
                is >> _persist_key_counter;
                is >> _root_folder;
                is >> _default_root_folder;             
                is.close();
                return true;
            }
            is.close();
            return false;
        }

        static bool dirExists(const std::string& dirName_in)
        {
            DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
            if (ftyp == INVALID_FILE_ATTRIBUTES) return false;
            if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;
            return false;
        }

    private:
        static std::string                          _root_folder;
        std::map<std::string, std::string>          _files;
        static std::unique_ptr<PersistManager>      _instance;
        static std::string                          _default_root_folder;
        static uint64_t                             _persist_key_counter;
    };

    std::unique_ptr<PersistManager> PersistManager::_instance = nullptr;
    std::string PersistManager::_root_folder = "C:\\tmp\\";
    std::string PersistManager::_default_root_folder = "C:\\tmp\\";
    uint64_t PersistManager::_persist_key_counter = 0;
}
#endif
