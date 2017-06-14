#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Domain<...>      : Base chess domain interface
// DomainKQvK       : KQvK chess domain
// DomainKvK        : KvK  chess domain
//
//
#ifndef _AL_CHESS_DOMAIN_DOMAIN_HPP
#define _AL_CHESS_DOMAIN_DOMAIN_HPP

namespace chess
{
    enum class ExactScore  { LOSS, DRAW, WIN, UNKNOWN}; // white win is WIN, black win is LOSS
    enum class eDomainName {KvK, KQvK};

    int ExactScore_to_int(ExactScore c)
    {
        if      (c == ExactScore::LOSS) return 1;
        else if (c == ExactScore::DRAW) return 2;
        else if (c == ExactScore::WIN)  return 3;
        else return 0;
    }
    ExactScore int_to_ExactScore(int t)
    {
        if      (t == 1) return ExactScore::LOSS;
        else if (t == 2) return ExactScore::DRAW;
        else if (t == 3) return ExactScore::WIN;
        return ExactScore::UNKNOWN;
    }

    // Domain interface
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Domain 
    {
        using _Board        = Board<PieceID, _BoardSize>;
        using _Move         = Move<PieceID>;
        using _Domain       = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Partition    = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PartitionManager = PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _GameDB       = GameDB<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        friend class _DomainPlayer;

    protected:
        std::string             _partition_key;
        std::string             _domainname_key;
        std::string             _instance_key;
        std::vector<_Domain*>   _children;
        _DomainPlayer*          _attached_domain_playerW;   // only two W+B for now...
        _DomainPlayer*          _attached_domain_playerB;
        std::string             _gamedb_key;
        _GameDB*                _gameDB;

    public:
        Domain( const std::string partition_key,
                const std::string domainname_key,
                const std::string instance_key) :
            _partition_key(partition_key), _domainname_key(domainname_key), _instance_key(instance_key),
            _attached_domain_playerW(nullptr),
            _attached_domain_playerB(nullptr),
            _gameDB(nullptr)
        {
            // Would load _gameDB on demand after domain is created and linked to a partition
        }

        _GameDB* get_game_db()
        {
            if (_gameDB == nullptr)
            {
                _gamedb_key = persist_key();
                _gameDB = new _GameDB(this, _gamedb_key);
                if (_gameDB->status() != 0)
                {
                    delete _gameDB;
                    _gameDB = nullptr;
                }
            }
            return _gameDB;
        }

        virtual ~Domain()
        {
            if (_gameDB != nullptr)
                delete _gameDB;
        }

        virtual bool is_cond_feature_valid(_ConditionFeature& f) const = 0;
        virtual bool is_valu_feature_valid(_ValuationFeature& f) const = 0;

        virtual bool       has_known_score_move()  const = 0;
        virtual ExactScore get_known_score_move(const _Board& position, const std::vector<_Move>& m, size_t& ret_mv_idx) const = 0;

        const std::vector<_Domain*>& children() const { return _children;}

        const std::string partition_key()   const { return _partition_key; }
        const std::string domainname_key()  const { return _domainname_key; }
        const std::string instance_key()    const { return _instance_key;  }

        const std::string domain_key()      const { return _domainname_key+"_"+_instance_key; }
        static const std::string domain_key(const std::string& k1, const std::string& k2) { return k1 + "_" + k2; }
        const std::string persist_key()     const { return _partition_key + "_" + domain_key(); }

        virtual bool isInDomain(const _Board& position) const = 0;

        virtual bool save() const = 0;
        virtual bool load() = 0;

        virtual _Board get_random_position(bool no_check) const = 0;

        static _Domain* make(const std::string& partition_key, const std::string& domainname_key, const std::string& instance_key)
        {
            if      (domainname_key == getDomainName(eDomainName::KvK))  return new DomainKvK <PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(partition_key);
            else if (domainname_key == getDomainName(eDomainName::KQvK)) return new DomainKQvK<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(partition_key);
            return nullptr;
        }

        void attach_domain_playerW(_DomainPlayer* player) { _attached_domain_playerW = player; }
        void attach_domain_playerB(_DomainPlayer* player) { _attached_domain_playerB = player; }

        static const std::string getDomainName(eDomainName e)
        {
            if      (e == eDomainName::KvK)  return "DomainKvK";
            else if (e == eDomainName::KQvK) return "DomainKQvK";
            return "";
        }

    protected:
        bool save_root() const
        {
            std::string f = PersistManager::instance()->get_stream_name("domain", persist_key());
            std::ofstream   is;
            is.open(f.c_str(), std::ofstream::out | std::ofstream::trunc);
            if (save_detail(is))
            {
                is.close();
                return true;
            }
            is.close();
            return false;
        }

        bool save_detail(std::ofstream& os) const
        {
            if (os.good())
            {
                os << _partition_key;   os << " ";
                os << _domainname_key;  os << " ";
                os << _instance_key;    os << " ";

                os << _children.size(); os << " ";
                for (auto& v : _children)
                {
                    os << v->partition_key();   os << " ";
                    os << v->domainname_key();  os << " ";
                    os << v->instance_key();    os << " ";
                }
                return true;
            }
            return false;
        }

        bool load_root() const
        {
            std::string f = PersistManager::instance()->get_stream_name("domain", persist_key());
            std::ifstream   is;
            is.open(f.c_str(), std::fstream::in);
            if (load_detail(is))
            {
                is.close();
                return true;
            }
            is.close();
            return false;
        }

        bool load_detail(std::ifstream& is) const
        {
            if (is.good())
            {
                size_t n_child;
                std::string partition_key;
                std::string domainname_key;
                std::string instance_key;

                is >> partition_key;
                is >> domainname_key;
                is >> instance_key;

                // check
                assert(partition_key == _partition_key);
                assert(domainname_key == _domainname_key);
                assert(instance_key == _instance_key);

                is >> n_child;

                _Partition* p_partition = _PartitionManager::instance()->find_partition(partition_key);
                if (p_partition == nullptr) return false;

                _Domain* p_dom = p_partition->find_domain(domain_key(domainname_key, instance_key));
                if (p_dom == nullptr) return false;

                // reloading
                p_dom->_children.clear();

                bool ok = true;
                for (size_t i = 0; i < n_child; i++)
                {
                    ok = false;
                    is >> partition_key;
                    is >> domainname_key;
                    is >> instance_key;              
                    {
                        _Domain* p_dom_child = p_partition->find_domain(domain_key(domainname_key,instance_key));
                        if (p_dom_child != nullptr)
                        {
                            if (p_dom->add_child(p_dom_child))
                            {
                                ok = true;                              
                            }
                        }
                    }
                    if (!ok)
                    {
                        return false;
                    }
                }
                return true;
            }
            return false;
        }


    public:
        bool add_child(_Domain* p)
        {
            // check already exist...
            _children.push_back(p);
            return true;
        }

        Domain* get_child(size_t idx) const
        {
            if ((idx>=0) && (idx < _children.size())) return _children.at(idx);
            return nullptr;
        }

        size_t size_children() { return _children.size(); }
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class DomainKvK : public Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board  = Board<PieceID, _BoardSize>;
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Move = Move<PieceID>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature_isOppositeKinCheck = ConditionFeature_isOppositeKinCheck<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature_numberMoveForPiece = ValuationFeature_numberMoveForPiece<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        friend class _DomainPlayer;

    public:
        DomainKvK(const std::string partition_key) 
            : _Domain(partition_key, _Domain::getDomainName(eDomainName::KvK), "0") {}

        bool isInDomain(const _Board& position) const override 
        { 
            if ( (position.cnt_piece(PieceName::K, PieceColor::W) == 1) &&
                 (position.cnt_piece(PieceName::K, PieceColor::B) == 1) &&
                 (position.cnt_all_piece() == 2))
                return true;
            return false;
        }

        bool is_cond_feature_valid(_ConditionFeature& f) const override
        {
            //...
            return true;
        }
        bool is_valu_feature_valid(_ValuationFeature& f) const override
        {
            if (f.name() == ValuFeatureName::eValuationFeature_numberMoveForPiece)
            {
                _ValuationFeature_numberMoveForPiece& r = (_ValuationFeature_numberMoveForPiece&)f;
                if (r.piecename() == PieceName::K) return true;
                return false;
            }
            return true;
        }

        virtual bool has_known_score_move() const { return true; }
        virtual ExactScore get_known_score_move(const _Board& position, const std::vector<_Move>& m, size_t& ret_mv_idx) const
        { 
            if (position.is_allow_self_check())
            {
                if (position.can_capture_opposite_king(m, ret_mv_idx))
                {
                    if (position.get_color() == PieceColor::W) return ExactScore::WIN;
                    else if (position.get_color() == PieceColor::B) return ExactScore::LOSS;
                }
               // else  return ExactScore::DRAW; // must know also the move
            }
            return ExactScore::UNKNOWN;
        }

        virtual bool save() const override
        {
            return save_root();
        }
        virtual bool load() override 
        {
            return load_root();
        }

        _Board get_random_position(bool no_check) const override
        {     
            uint8_t wK = 0; 
            uint8_t bK = 0;
            while (wK == bK)
            {
                wK = (std::rand() % _BoardSize*_BoardSize);
                bK = (std::rand() % _BoardSize*_BoardSize);
            }
            _Board b; 
            b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::W), wK % _BoardSize, ((uint8_t)(wK / _BoardSize)));
            b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::B), bK % _BoardSize, ((uint8_t)(bK / _BoardSize)));
            b.set_color(PieceColor::W); 

            if (no_check)
            {
                if (b.is_in_check())
                {
                    return get_random_position(no_check);
                }
                else
                {
                    std::vector<_Move> m = b.generate_moves();
                    size_t mv;
                    if (b.can_capture_opposite_king(m, mv))
                        return get_random_position(no_check);
                }
            }
            return b;
        }
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class DomainKQvK : public Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Move = Move<PieceID>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature_isOppositeKinCheck = ConditionFeature_isOppositeKinCheck<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature_numberMoveForPiece = ValuationFeature_numberMoveForPiece<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        friend class _DomainPlayer;

    public:
        DomainKQvK(const std::string partition_key) 
            : _Domain(partition_key, _Domain::getDomainName(eDomainName::KQvK), "0")
        {
        }

        bool isInDomain(const _Board& position) const override
        {
            if ((position.cnt_piece(PieceName::K, PieceColor::W) == 1) &&
                (position.cnt_piece(PieceName::K, PieceColor::B) == 1) &&
                (position.cnt_piece(PieceName::Q, PieceColor::W) == 1) &&
                (position.cnt_all_piece() == 3))
                return true;
            return false;
        }

        bool is_cond_feature_valid(_ConditionFeature& f) const override
        {
            //...
            return true;
        }
        bool is_valu_feature_valid(_ValuationFeature& f) const override
        {
            if (f.name() == ValuFeatureName::eValuationFeature_numberMoveForPiece)
            {
                _ValuationFeature_numberMoveForPiece& r = (_ValuationFeature_numberMoveForPiece&)f;
                if      (r.piecename() == PieceName::K) return true;
                else if ((r.piecename() == PieceName::Q) && (r.piececolor() == PieceColor::W)) return true;
                return false;
            }
            return true;
        }

        virtual bool has_known_score_move() const { return true; }
        virtual ExactScore get_known_score_move(const _Board& position, const std::vector<_Move>& m, size_t& ret_mv_idx) const
        {
            if (position.is_allow_self_check())
            {
                if (position.can_capture_opposite_king(m, ret_mv_idx))
                {
                    if (position.get_color() == PieceColor::W) return ExactScore::WIN;
                    else if (position.get_color() == PieceColor::B) return ExactScore::LOSS;
                }
            }
            return ExactScore::UNKNOWN;
        }

        virtual bool save() const override  
        {
            return save_root();
        }
        virtual bool load() override
        {
            return load_root();
        }

        _Board get_random_position(bool no_check) const override
        {
            uint8_t wQ = 0;
            uint8_t wK = 0;
            uint8_t bK = 0;
            while ((wK == bK)||(wK==wQ)||(bK==wQ))
            {
                wQ = (std::rand() % (_BoardSize*_BoardSize));
                wK = (std::rand() % (_BoardSize*_BoardSize));
                bK = (std::rand() % (_BoardSize*_BoardSize));
            }
            _Board b;
            b.set_pieceid_at(_Piece::get_id(PieceName::Q, PieceColor::W), wQ % _BoardSize, ((uint8_t)(wQ / _BoardSize)));
            b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::W), wK % _BoardSize, ((uint8_t)(wK / _BoardSize)));
            b.set_pieceid_at(_Piece::get_id(PieceName::K, PieceColor::B), bK % _BoardSize, ((uint8_t)(bK / _BoardSize)));
            b.set_color(PieceColor::W);

            if (no_check)
            {
                if (b.is_in_check())
                {
                    return get_random_position(no_check);
                }
                else
                {
                    std::vector<_Move> m = b.generate_moves();
                    size_t mv;
                    if ( b.can_capture_opposite_king(m, mv) )
                        return get_random_position(no_check); 
                }
            }
            return b;
        }
    };

};

#endif
