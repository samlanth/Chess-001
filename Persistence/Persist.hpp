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
    template <typename PieceID, typename uint8_t _BoardSize>
    class PersistManager
    {
        using _Piece = Piece<PieceID, _BoardSize>;

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

        static void create_directory(std::string s)
        {
            CreateDirectory(s.c_str(), NULL);
        }

        static const PersistManager* instance()
        {
            if (!_instance.operator bool())
            {
                PieceID id = 0;
                std::string skey = "CFG_" + std::to_string((int)_BoardSize) + std::string(typeid(id).name()) + "\\";
                skey.erase(std::remove(skey.begin(), skey.end(), ' '), skey.end());

                if (!(dirExists(PersistManager::_default_root_folder)))
                {
                    create_directory(PersistManager::_default_root_folder);
                }
                std::string sroot = PersistManager::_default_root_folder + skey;
                if (!(dirExists(sroot)))
                {
                    create_directory(sroot);
                }

                _instance = std::unique_ptr<PersistManager>(new PersistManager);
                _instance.get()->_root_folder = sroot;
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
            std::string filename = folder + instance_key + _FILE_EXTENSION;
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
            std::string filename = folder + instance_key + _FILE_EXTENSION;
            return filename;
        }

        std::string create_persist_key() const
        {
            _persist_key_counter++;
            std::stringstream ss;
            ss << _persist_key_counter;
            save();                         // save less frequently...memory mapped
            return ss.str();
        }

        bool save() const
        {
            std::string f = _root_folder + _PERSISTMANAGER_FILENAME + _FILE_EXTENSION;
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
            std::string f = _root_folder + _PERSISTMANAGER_FILENAME + _FILE_EXTENSION;
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
        const std::string   _PERSISTMANAGER_FILENAME = "persistmanager";
        const std::string   _FILE_EXTENSION = ".dat";

        static std::string                          _root_folder;
        std::map<std::string, std::string>          _files;
        static std::unique_ptr<PersistManager>      _instance;
        static std::string                          _default_root_folder;
        static uint64_t                             _persist_key_counter;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    std::unique_ptr<PersistManager<PieceID, _BoardSize>> PersistManager<PieceID, _BoardSize>::_instance = nullptr;

    template <typename PieceID, typename uint8_t _BoardSize>
    std::string PersistManager<PieceID, _BoardSize>::_root_folder = "C:\\tmp\\";

    template <typename PieceID, typename uint8_t _BoardSize>
    std::string PersistManager<PieceID, _BoardSize>::_default_root_folder = "C:\\tmp\\";

    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t PersistManager<PieceID, _BoardSize>::_persist_key_counter = 0;
}
#endif
