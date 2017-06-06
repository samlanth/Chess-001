#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// <... work in progress ...>
//
// Partiton<...>    : A partition of the chess space into hiearchical domains
//
//
#ifndef _AL_CHESS_PARTITION_H
#define _AL_CHESS_PARTITION_H

#include <map>
#include <string>
#include <memory>

namespace chess
{
    // forward
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Domain;

    // Partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Partition
    {
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        Partition() = default;
        Partition(std::string aname) : _name(aname) { }
        const std::string name()   const { return _name; }

        // Persistence
        bool save() const   { return false; } //...
        bool load()         { return false; } //...

        bool add_domain(_Domain* p)
        {
            auto iter = _domains.find(p->domain_key()); 
            if (iter == _domains.end())
            {
                _domains.insert({ p->domain_key(), p });
                return true;
            }
            return false;
        }

        _Domain* find_domain(const std::string& domain_key) const
        {
            auto iter = _domains.find(domain_key);
            if (iter != _domains.end())
            {
                _Domain* p = iter->second;
                return p;
            }
            return nullptr;
        }
        _Domain* find_domain(const std::string& classname_key, const std::string& instance_key) const
        {
            return find_domain(_Domain::domain_key(classname_key, instance_key));
        }

    private:
        std::string                      _name;
        std::map<std::string, _Domain*>  _domains;  // on heap - owner (std::unique_ptr...)
    };
};

#endif
