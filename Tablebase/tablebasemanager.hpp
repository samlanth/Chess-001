#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TablebaseManager
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASEMANAGER_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASEMANAGER_HPP

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseManager
    {
        using _Piece = Piece<PieceID, _BoardSize>;

    private:
        TablebaseManager() {}

        TablebaseManager(const TablebaseManager&) = delete;
        TablebaseManager & operator=(const TablebaseManager &) = delete;
        TablebaseManager(TablebaseManager&&) = delete;

    public:
        ~TablebaseManager()
        {
            if (_instance.operator bool())
            {
                _instance.release();
            }
        }
        static const TablebaseManager* instance();

        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 1>* tb) const;
        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 2>* tb) const;
        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 3>* tb) const;
 
        Tablebase<PieceID, _BoardSize, 1>* find_1(const std::string& name) const;
        Tablebase<PieceID, _BoardSize, 2>* find_2(const std::string& name) const;
        Tablebase<PieceID, _BoardSize, 3>* find_3(const std::string& name) const;

        // prototype
        TablebaseBaseHandlerCore<PieceID, _BoardSize>*  make_1(const std::vector<std::pair<PieceID, uint8_t>>& pw, const std::vector<std::pair<PieceID, uint8_t>>& pb) const;
        TablebaseBaseHandlerCore<PieceID, _BoardSize>*  make_1(const PieceSet<PieceID, _BoardSize>& set) const;

        TablebaseBaseHandlerCore<PieceID, _BoardSize>*  make_2(const std::vector<std::pair<PieceID, uint8_t>>& pw, const std::vector<std::pair<PieceID, uint8_t>>& pb) const;
        TablebaseBaseHandlerCore<PieceID, _BoardSize>*  make_2(const PieceSet<PieceID, _BoardSize>& set) const;

        std::vector<TablebaseBaseHandlerCore<PieceID, _BoardSize>*> make_child_3(const std::vector<std::pair<PieceID, uint8_t>>& pw,const std::vector<std::pair<PieceID, uint8_t>>& pb) const;
        std::vector<TablebaseBaseHandlerCore<PieceID, _BoardSize>*> make_child_3(const PieceSet<PieceID, _BoardSize>& set) const;

    public:
        static std::string name_pieces(const std::vector<PieceID>& v, PieceColor color_to_play)
        {
            // assume sorted
            std::string str_pieces;
            if (color_to_play == PieceColor::W) str_pieces = "W_"; else str_pieces = "B_";
            for (auto& vv : v) str_pieces += Piece<PieceID, _BoardSize>::to_str(vv, false);
            return str_pieces;
        }

    private:
        // Not owner
        mutable std::map<std::string, Tablebase<PieceID, _BoardSize, 1>*>  _tbs1;
        mutable std::map<std::string, Tablebase<PieceID, _BoardSize, 2>*>  _tbs2;
        mutable std::map<std::string, Tablebase<PieceID, _BoardSize, 3>*>  _tbs3;

        static std::unique_ptr<TablebaseManager> _instance;
    };

    //_instance
    template <typename PieceID, typename uint8_t _BoardSize>
    std::unique_ptr<TablebaseManager<PieceID, _BoardSize>>
    TablebaseManager<PieceID, _BoardSize>::_instance = nullptr;

    // add
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseManager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 1>* tb) const
    {
        if (_instance == nullptr) return false;
        {
            auto iter = _tbs1.find(name);
            if (iter == _tbs1.end())
            {
                _tbs1[name] = tb;
                return true;
            }
            return true;
        }
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseManager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 2>* tb) const
    {
        if (_instance == nullptr) return false;
        {
            auto iter = _tbs2.find(name);
            if (iter == _tbs2.end())
            {
                _tbs2[name] = tb;
                return true;
            }
            return true;
        }
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseManager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 3>* tb) const
    {
        if (_instance == nullptr) return false;
        {
            auto iter = _tbs3.find(name);
            if (iter == _tbs3.end())
            {
                _tbs3[name] = tb;
                return true;
            }
            return true;
        }
        return false;
    }

    // find
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase<PieceID, _BoardSize, 1>*  TablebaseManager<PieceID, _BoardSize>::find_1(const std::string& name) const
    {
        if (_instance == nullptr) return nullptr;
        {
            auto iter = _tbs1.find(name);
            if (iter != _tbs1.end())
            {
                return _tbs1[name];
            }
        }
        return nullptr;
    }
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase<PieceID, _BoardSize, 2>*  TablebaseManager<PieceID, _BoardSize>::find_2(const std::string& name) const
    {
        if (_instance == nullptr) return nullptr;
        {
            auto iter = _tbs2.find(name);
            if (iter != _tbs2.end())
            {
                return _tbs2[name];
            }
        }
        return nullptr;
    }
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase<PieceID, _BoardSize, 3>*  TablebaseManager<PieceID, _BoardSize>::find_3(const std::string& name) const
    {
        if (_instance == nullptr) return nullptr;
        {
            auto iter = _tbs3.find(name);
            if (iter != _tbs3.end())
            {
                return _tbs3[name];
            }
        }
        return nullptr;
    }

    // instance()
    template <typename PieceID, typename uint8_t _BoardSize>
    const TablebaseManager<PieceID, _BoardSize>* 
    TablebaseManager<PieceID, _BoardSize>::instance()
    {
        if (_instance == nullptr)
        {
            _instance = std::unique_ptr<TablebaseManager>(new TablebaseManager);
            return _instance.get();
        }
        return  _instance.get();
    }

    // Prototype....
    // make TB 1 handler
    template <typename PieceID, typename uint8_t _BoardSize>
    TablebaseBaseHandlerCore<PieceID, _BoardSize>*  TablebaseManager<PieceID, _BoardSize>::make_1(const PieceSet<PieceID, _BoardSize>& set) const
    {
        return make_1(set.wset(), set.bset());
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    TablebaseBaseHandlerCore<PieceID, _BoardSize>*  TablebaseManager<PieceID, _BoardSize>::make_1(
        const std::vector<std::pair<PieceID, uint8_t>>& pw, const std::vector<std::pair<PieceID, uint8_t>>& pb ) const
    {
        TablebaseBaseHandlerCore<PieceID, _BoardSize>* v = nullptr;
        PieceSet<PieceID, _BoardSize> set(pw, pb);
        if ((set.count_all_piece(PieceColor::W) == 1) && (set.count_all_piece(PieceColor::B) == 0))
        {
            //  Xv0
            std::vector<PieceID> _p;
            _p.push_back(pw[0].first);
            v = new TablebaseBaseHandler_Xv0<PieceID, _BoardSize>(_p);
        }
        else if ((set.count_all_piece(PieceColor::W) == 0) && (set.count_all_piece(PieceColor::B) == 1))
        {
            // 0vX
            std::vector<PieceID> _p;
            _p.push_back(pb[0].first);
            v = new TablebaseBaseHandler_0vX<PieceID, _BoardSize>(_p);
        }
        return v;
    }

    // make TB 2 handler
    template <typename PieceID, typename uint8_t _BoardSize>
    TablebaseBaseHandlerCore<PieceID, _BoardSize>* TablebaseManager<PieceID, _BoardSize>::make_2(const PieceSet<PieceID, _BoardSize>& set) const
    {
        return make_2(set.wset(), set.bset());
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    TablebaseBaseHandlerCore<PieceID, _BoardSize>* TablebaseManager<PieceID, _BoardSize>::make_2(
        const std::vector<std::pair<PieceID, uint8_t>>& pw, 
        const std::vector<std::pair<PieceID, uint8_t>>& pb) const
    {
        TablebaseBaseHandlerCore<PieceID, _BoardSize>* v = nullptr;
        PieceSet<PieceID, _BoardSize> set(pw, pb);

        uint16_t wret_count_0;
        uint16_t wret_count_1;
        uint16_t wrank_0 = set.find_rank_index(PieceColor::W, 0, wret_count_0); // W First rank piece index
        uint16_t wrank_1 = set.find_rank_index(PieceColor::W, 1, wret_count_1); // W 2nd rank piece index - can be a repeating piece
        uint16_t bret_count_0;
        uint16_t bret_count_1;
        uint16_t brank_0 = set.find_rank_index(PieceColor::B, 0, bret_count_0);
        uint16_t brank_1 = set.find_rank_index(PieceColor::B, 1, bret_count_1);

        if ((set.count_all_piece(PieceColor::W) == 2) && (set.count_all_piece(PieceColor::B) == 0))
        {
            // Xv0
            PieceID WKid = _Piece::get_id(chess::PieceName::K, chess::PieceColor::W);
            std::vector<PieceID> _p;
            if (pw[wrank_0].first == WKid) _p.push_back(WKid);
            else _p.push_back(pw[brank_1].first);
            v = new TablebaseHandler_Xv0<PieceID, _BoardSize>(_p);
        }
        else if ((set.count_all_piece(PieceColor::W) == 0) && (set.count_all_piece(PieceColor::B) == 2))
        {
            // 0vX
            PieceID BKid = _Piece::get_id(chess::PieceName::K, chess::PieceColor::B);
            std::vector<PieceID> _p;
            if (pb[brank_0].first == BKid) _p.push_back(BKid);
            else _p.push_back(pb[brank_1].first);
            v = new TablebaseHandler_0vX<PieceID, _BoardSize>(_p);
        }
        else if ((set.count_all_piece(PieceColor::W) == 1) && (set.count_all_piece(PieceColor::B) == 1))
        {
            // 1v1
            std::vector<PieceID> _p;
            _p.push_back(pw[wrank_0].first); 
            _p.push_back(pb[brank_0].first);
            v = new TablebaseHandler_1v1<PieceID, _BoardSize>(_p);
        }
        return v;
    }

    // Create all TB children handler of a 3 piece TB
    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<TablebaseBaseHandlerCore<PieceID, _BoardSize>*> TablebaseManager<PieceID, _BoardSize>::make_child_3(const PieceSet<PieceID, _BoardSize>& set) const
    {
        return make_3(set.wset(), set.bset());
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    std::vector<TablebaseBaseHandlerCore<PieceID, _BoardSize>*> TablebaseManager<PieceID, _BoardSize>::make_child_3(
        const std::vector<std::pair<PieceID, uint8_t>>& pw, const std::vector<std::pair<PieceID, uint8_t>>& pb) const
    {
        std::vector<TablebaseBaseHandlerCore<PieceID, _BoardSize>*> v;
        PieceSet<PieceID, _BoardSize> set(pw, pb);

        std::vector<std::pair<PieceID, uint8_t>> t;
        TablebaseBaseHandlerCore<PieceID, _BoardSize>* tb;
        for (size_t i = 0; i < set.children_size(PieceColor::W); i++)
        {
            t = set.children(PieceColor::W, i);
            if (t.size() > 0)
            {
                tb = make_2(t, pb);
                if (tb!=nullptr)
                    v.push_back(tb);
            }
        }
        for (size_t i = 0; i < set.children_size(PieceColor::B); i++)
        {
            t = set.children(PieceColor::B, i);
            if (t.size() > 0)
            {
                tb = make_2(pw, t);
                if (tb != nullptr)
                    v.push_back(tb);
            }
        }
        return v;
    }
};

#endif
