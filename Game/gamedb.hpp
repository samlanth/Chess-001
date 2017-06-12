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

        char        _status;                            // (completed, tobe_removed, in_progress)
        uint8_t     _board_size = _BoardSize;
        std::vector<std::pair<uint8_t, uint8_t>>  _xy;  // Piece position x, y initial position
        PieceColor  _color;                             // initial color
        std::list<_Move> _history_moves;                //...maybe simplified later...
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
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _GameDB_Record = GameDB_Record<PieceID, _BoardSize>;

    public:
        struct GameDB_Header
        {
            // domain signature
            std::string _partition_key;
            std::string _domainname_key;
            std::string _instance_key;

            uint64_t    _size;
        };

        GameDB(_DomainPlayer* domain, const std::string& db_filename) 
            : _status(0), _domain(domain), _db_filename(db_filename), _size(0)
        {
            // open or create...

            // open _db_filename, read header
            // validate header
            // set _size
            // load some records in cache
        }
        virtual ~GameDB() {}

        size_t  size()      { return _size; }
        char    status()    { return _status; }
        _DomainPlayer* domain() { return _domain; }

        //GameDB_Record     get_game(size_t index);
        //bool              store_game(const GameDB_Record& rec);
        //
        //bool      remove_game(size_t index)
        //bool      load_pgn_file(const std::string filename);

   protected:
        char            _status;        // ok==0, bad_open, ...
        _DomainPlayer*  _domain;
        std::string     _db_filename;
        uint64_t        _size;          // number of records

        uint64_t        _max_cache_size = 1000;
        std::map<uint64_t, _GameDB_Record*> _cache;
    };

};

#endif
