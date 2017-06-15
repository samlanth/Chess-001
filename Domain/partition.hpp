#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Partiton<...>    : A partition of the chess space into hiearchical domains
//
//
#ifndef _AL_CHESS_DOMAIN_PARTITION_HPP
#define _AL_CHESS_DOMAIN_PARTITION_HPP

namespace chess
{
    // Partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Partition
    {
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PartitionManager = PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        Partition() = default;
        ~Partition() = default;

        Partition(std::string aname) : _name(aname) { }
        const std::string name()   const { return _name; }

        // Persistence
        bool save() const;
        bool load();

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
        _Domain* find_domain(const std::string& domainname_key, const std::string& instance_key) const
        {
            return find_domain(_Domain::domain_key(domainname_key, instance_key));
        }

     private:
        std::string                      _name;
        std::map<std::string, _Domain*>  _domains;  // on heap - owner (TODO std::unique_ptr...)
    };

    // save()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save() const
    {
        std::string f = PersistManager::instance()->get_stream_name("partition", _name);
        std::ofstream   is;
        is.open(f.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (is.good())
        {
            is << _name;            is << " ";
            is << _domains.size();  is << " ";
            for (auto& v : _domains)
            {
                is << v.second->domainname_key();   is << " ";
                is << v.second->instance_key();     is << " ";
            }
        }
        else
        {
            is.close();
            return false;
        }
        is.close();
        return true;
    }

    // load()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load()
    {
        std::string f = PersistManager::instance()->get_stream_name("partition", _name);
        std::ifstream   is;
        is.open(f.c_str(), std::fstream::in);
        if (is.good())
        {
            size_t n_child;
            std::string partition_key;
            std::string domainname_key;
            std::string instance_key;

            is >> partition_key;
            assert(partition_key == _name); // check
            is >> n_child;

            // reloading or creating
            _domains.clear();

            bool ok = true;
            for (size_t i = 0; i < n_child; i++)
            {
                ok = false;
                is >> domainname_key;
                is >> instance_key;
                {
                    _Domain* ptr_dom = _Domain::make(partition_key, domainname_key, instance_key);
                    if (ptr_dom != nullptr)
                    {
                        if (add_domain(ptr_dom))
                        {                          
                            ok = true;
                        }
                    }
                }
                if (!ok)
                {
                    is.close();
                    return false;
                }
            }

            // Load children and data of domains
            _Domain* p;
            for (auto& v : _domains)
            {
                p = v.second;
                if (p != nullptr)
                {
                    if (p->load() == false)
                    {
                        is.close();
                        return false;
                    }
                }
            }

            is.close();
            return true;
        }
        else
        {
            is.close();
            return false;
        }
    }
};

#endif
