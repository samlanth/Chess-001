#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//  GameDB  :   Hold best games for a domain so far
//              Best games are the one analyzed most extensively by best ga evolved players so far
//
//
#ifndef _AL_CHESS_GAME_GAMEDB_H
#define _AL_CHESS_GAME_GAMEDB_H

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize>
    struct GameDB_Record
    {
        using _Move = Move<PieceID>;

        uint8_t     _status;                            // (0=completed, tobe_removed, game_in_progress)
        uint8_t     _board_size = _BoardSize;
        std::vector<PieceID>  _pieces;
        std::vector<std::pair<uint8_t, uint8_t>>  _xy;
        PieceColor  _color; 
        std::list<_Move> _history_moves;
        ExactScore  _score;
        uint64_t    _total_num_nodes_explored;
        bool        _is_playW_internal;
        bool        _is_playB_internal;
        std::string _playerW_persist_id;
        std::string _playerB_persist_id;
        uint16_t    _playerW_elo;                       // (Should be estimated against known engine, 0==not known)
        uint16_t    _playerB_elo;
        BaseGame_Config _game_config;
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class GameDB
    {
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _GameDB_Record = GameDB_Record<PieceID, _BoardSize>;
        using _GameDB = GameDB<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PartitionManager = PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        GameDB(_Domain* domain, const std::string& db_keyname) 
            : _status(0), _domain(domain), _db_keyname(db_keyname), _size(0)
        {
            if (_domain == nullptr) return;

            _partition_key  = _domain->partition_key();
            _domainname_key = _domain->domainname_key();
            _instance_key   = _domain->instance_key();

            if (!open_create())
                _status = -1;

        }
        virtual ~GameDB() {}

        size_t      size()      { return _size; }
        uint8_t     status()    { return _status; }
        _DomainPlayer* domain() { return _domain; }

        //GameDB_Record     get_game(size_t index);
        bool        store_game(const _GameDB_Record& rec);
        //bool      remove_game(size_t index)
        //bool      load_pgn_file(const std::string filename);

   protected:
        uint8_t         _status;        // ok==0, bad=-1
        _Domain*        _domain;
        std::string     _db_keyname;
        
        // domain signature
        std::string _partition_key;
        std::string _domainname_key;
        std::string _instance_key;
        
        uint64_t    _size;
        std::vector<uint64_t> _vector_index_rec;
        uint64_t _pos_rec_file = 0;

    protected:
        bool open_create();
        bool store_game_rec(const _GameDB_Record& rec);
    };


    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool GameDB<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::open_create()
    {
        std::string f = PersistManager::instance()->get_stream_name("gamedb", _db_keyname);
        std::ifstream   filestream;
        filestream.open(f.c_str(), std::fstream::in);
        if (filestream.good())
        {
            // open
            filestream >> _partition_key;
            filestream >> _domainname_key;
            filestream >> _instance_key;
            filestream >> _size;

            long n; filestream >> n;
            assert(_size == n);

            long pos;
            for (size_t i = 0; i < n; i++)
            {
                filestream >> pos;
                _vector_index_rec.push_back(pos);
            }

            _Partition* p_partition = _PartitionManager::instance()->find_partition(_partition_key);
            if (p_partition == nullptr)
            { 
                filestream.close(); return false; 
            }

            _domain = p_partition->find_domain(_Domain::domain_key(_domainname_key, _instance_key));
            if (_domain == nullptr)
            { 
                filestream.close(); return false; 
            }

            // check
            if (_partition_key != _domain->partition_key()) { filestream.close(); return false; }
            if (_domainname_key != _domain->domainname_key()) { filestream.close(); return false; }
            if (_instance_key != _domain->instance_key()) { filestream.close(); return false; }

            filestream.close();
            return true;
        }
        else
        {
            // create
            filestream.close();
            std::ofstream   ofilestream;
            ofilestream.open(f.c_str(), std::ofstream::out | std::ofstream::trunc);
            if (ofilestream.good())
            {        
                ofilestream << _partition_key; ofilestream << std::endl;
                ofilestream << _domainname_key; ofilestream << std::endl;
                ofilestream << _instance_key;  ofilestream << std::endl;
                ofilestream << _size; ofilestream << std::endl;
                ofilestream << (long)0; ofilestream << std::endl;
                ofilestream.close();
                return true;
            }
            ofilestream.close();
            return false;
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool GameDB<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::store_game(const _GameDB_Record& rec)
    {
        if (_status != 0) return false;
        std::string f = PersistManager::instance()->get_stream_name("gamedb", _db_keyname);
        std::ofstream   ofilestream;
        ofilestream.open(f.c_str(), std::ofstream::out | std::ofstream::trunc); 
        if (ofilestream.good())
        {
            if (store_game_rec(rec))
            {
                _size++;
                _vector_index_rec.push_back(_pos_rec_file);
                ofilestream << _partition_key;  ofilestream << std::endl;
                ofilestream << _domainname_key; ofilestream << std::endl;
                ofilestream << _instance_key;   ofilestream << std::endl;
                ofilestream << _size;           ofilestream << std::endl;
                ofilestream << _vector_index_rec.size();           ofilestream << std::endl;
                for (auto &v : _vector_index_rec) { ofilestream << (long)v; ofilestream << std::endl; }
                ofilestream.close();

                return true;
            }
        }
        ofilestream.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool GameDB<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::store_game_rec(const _GameDB_Record& rec)
    {
        if (_status != 0) return false;
        std::string f = PersistManager::instance()->get_stream_name("gamedbrec", _db_keyname);
        std::ofstream ofilestream;
        ofilestream.open(f.c_str(), std::ofstream::out | std::ofstream::app);
        if (ofilestream.good())
        {
            ofilestream.seekp(0, ofilestream.ios_base::end);
            _pos_rec_file = ofilestream.tellp();
            ofilestream << (int)rec._status;         ofilestream << std::endl;
            ofilestream << (int)rec._board_size;     ofilestream << std::endl;
            ofilestream << (int)rec._pieces.size();   ofilestream << std::endl;
            for (auto &v : rec._pieces) { ofilestream << (int)v; ofilestream << std::endl; }
            ofilestream << (int)rec._xy.size();     ofilestream << std::endl;
            for (auto &v : rec._xy) 
            {   
                ofilestream << (int)v.first;     ofilestream << std::endl;
                ofilestream << (int)v.second;    ofilestream << std::endl;
            }
            char c = 1; if (rec._color == PieceColor::B) c = 0;
            ofilestream << (int)c;          ofilestream << std::endl;
            ofilestream << rec._total_num_nodes_explored;  ofilestream << std::endl;
            ofilestream << (int)rec._is_playW_internal;      ofilestream << std::endl;
            ofilestream << (int)rec._is_playB_internal;      ofilestream << std::endl;
            ofilestream << rec._playerW_persist_id;     ofilestream << std::endl;
            ofilestream << rec._playerB_persist_id;     ofilestream << std::endl;
            ofilestream << (int)rec._playerW_elo;    ofilestream << std::endl;
            ofilestream << (int)rec._playerB_elo;    ofilestream << std::endl;
            ofilestream << (long)rec._game_config._w_max_num_position_per_move; ofilestream << std::endl;
            ofilestream << (long)rec._game_config._w_max_depth_per_move; ofilestream << std::endl;
            ofilestream << (long)rec._game_config._b_max_num_position_per_move; ofilestream << std::endl;
            ofilestream << (long)rec._game_config._b_max_depth_per_move; ofilestream << std::endl;
            ofilestream << (long)rec._game_config._max_game_ply; ofilestream << std::endl;

            ofilestream.close();
            return true;
        }
        ofilestream.close();
        return false;
    }

};

#endif
