#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// PartitionManger  : Manage the multiple partitions
//
//
#ifndef _AL_CHESS_PARTITIONMANAGER_H
#define _AL_CHESS_PARTITIONMANAGER_H

#include <map>
#include <string>
#include <memory>

namespace chess
{
    // forward
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class Domain;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class Partition;

    // PartitionManager
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class PartitionManager
    {
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    private:
        PartitionManager() {}

        PartitionManager(const PartitionManager&) = delete;
        PartitionManager & operator=(const PartitionManager &) = delete;
        PartitionManager(PartitionManager&&) = delete;

    public:
        ~PartitionManager()
        {
            if (_instance.operator bool())
            {
                _partitions.clear();
                _instance.release();
            }
        }

    public:
        static bool             add_partition(_Partition* p);
        static _Partition*      find_partition(const std::string partition_name);

        static const PartitionManager* instance();

        // Exemple
        static bool make_classic_partition();

    private:
        std::map<std::string, _Partition*>          _partitions;    // TODO std_unique<> ...
        static std::unique_ptr<PartitionManager>    _instance;
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::unique_ptr<PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>>
    PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_instance = nullptr;

    // make_classic_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::make_classic_partition()
    {
        _Partition* p = new _Partition("classic");
        if (!PartitionManager::instance()->add_partition(p)) return false;

        _Partition* p_classic = PartitionManager::instance()->find_partition("classic");
        if (p_classic == nullptr) return false;

        // create domains
        _Domain* dom_KK   = new DomainKvK< PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>("classic");
        _Domain* dom_KQvK = new DomainKQvK<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>("classic");

        if (!p_classic->add_domain(dom_KK))     return false;
        if (!p_classic->add_domain(dom_KQvK))   return false;

        _Domain* ptr_dom_KQvK = p_classic->find_domain("DomainKQvK", "0"); if (ptr_dom_KQvK == nullptr) return false;
        _Domain* ptr_dom_KK   = p_classic->find_domain("DomainKvK" , "0"); if (ptr_dom_KK   == nullptr) return false;

        // set children relationship
        if (!ptr_dom_KQvK->add_child(ptr_dom_KK)) return false;

        // test persistence
        ptr_dom_KK->save();
        ptr_dom_KQvK->save();

        ptr_dom_KK->load();
        ptr_dom_KQvK->load();

        p_classic->save();
        p_classic->load();
        return true;
    }

    // find_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* 
    PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::find_partition(const std::string partition_name)
    {
        if (_instance == nullptr) return nullptr;

        auto& iter = _instance->_partitions.find(partition_name);
        if (iter != _instance->_partitions.end())
        {
            return (iter->second);
        }
        return nullptr;
    }

    // add_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::add_partition(_Partition* p)
    {
        if (_instance == nullptr) return false;

        auto iter = _instance->_partitions.find(p->name());
        if (iter == _instance->_partitions.end())
        {
            _instance->_partitions.insert(std::pair<std::string, _Partition*>(p->name(), p));
            //_instance->_partitions[p->name()] = p;
            return true;
        }
        return false;
    }

    // instance()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    const PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* 
    PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::instance()
    {
        if (_instance == nullptr)
        {
            _instance = std::unique_ptr<PartitionManager>(new PartitionManager);
            return _instance.get();
        }
        return  _instance.get();;
    }

};

#endif
