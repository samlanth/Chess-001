#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// PersistManager<>   : Provide io fstream for [class, instance_key] 
//
//
#ifndef _AL_CHESS_PERSIST_H
#define _AL_CHESS_PERSIST_H

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
            std::string filename = _root_folder + class_name + "_" + instance_key + ".txt";
            return filename;
        } 

        static const std::string create_persist_key()
        {
            _persist_key_counter++;
            std::stringstream ss;
            ss << _persist_key_counter;
            save();                         // TODO - save less frequently...memory mapped
            return ss.str();
        }

        static bool save()
        {
            std::string f = _root_folder + "persistmanager" + "_" + "0" + ".txt";
            std::ofstream   filestream;
            filestream.open(f.c_str(), std::fstream::out | std::fstream::trunc);
            if (filestream.good())
            {
                filestream << _persist_key_counter; filestream << std::endl;
                filestream << _root_folder;         filestream << std::endl;
                filestream << _default_root_folder; filestream << std::endl;                
                filestream.close();
                return true;
            }
            filestream.close();
            return false;
        }

        bool load()
        {
            std::string f = _root_folder + "persistmanager" + "_" + "0" + ".txt";
            std::ifstream   filestream;
            filestream.open(f.c_str(), std::fstream::in);
            if (filestream.good())
            {
                filestream >> _persist_key_counter;
                filestream >> _root_folder;
                filestream >> _default_root_folder;             
                filestream.close();
                return true;
            }
            filestream.close();
            return false;
        }

    private:
        static std::string                          _root_folder;
        std::map<std::string, std::string>          _files;   // key=class_name+instance_key, value=stream file name
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
