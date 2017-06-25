#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TablebaseManager : Track TB
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASEMANAGER_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASEMANAGER_HPP

namespace chess
{
    // TablebaseManager
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseManager
    {
        using _Piece = Piece<PieceID, _BoardSize>;

    private:
        TablebaseManager() {}
        TablebaseManager(const TablebaseManager&)               = delete;
        TablebaseManager & operator=(const TablebaseManager &)  = delete;
        TablebaseManager(TablebaseManager&&)                    = delete;

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

        std::vector<TBH<PieceID, _BoardSize>*> make_all_child_TBH(const PieceSet<PieceID, _BoardSize>& set) const;

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
        // Would be owner... unique_ptr/weak_ptr
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
    inline bool TablebaseManager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 1>* tb) const
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
    inline bool TablebaseManager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 2>* tb) const
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
    inline bool TablebaseManager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 3>* tb) const
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

    // find_1
    template <typename PieceID, typename uint8_t _BoardSize>
    inline Tablebase<PieceID, _BoardSize, 1>*  TablebaseManager<PieceID, _BoardSize>::find_1(const std::string& name) const
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
    inline Tablebase<PieceID, _BoardSize, 2>*  TablebaseManager<PieceID, _BoardSize>::find_2(const std::string& name) const
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
    inline Tablebase<PieceID, _BoardSize, 3>*  TablebaseManager<PieceID, _BoardSize>::find_3(const std::string& name) const
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

    // Create all TB children handler of a TB
    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::vector<TBH<PieceID, _BoardSize>*> TablebaseManager<PieceID, _BoardSize>::make_all_child_TBH(const PieceSet<PieceID, _BoardSize>& set) const
    {
        std::vector<TBH<PieceID, _BoardSize>*> v;
        uint16_t nw; 
        uint16_t nb;
        std::vector<std::pair<PieceID, uint8_t>> empty_set;

        std::vector<PieceSet<PieceID, _BoardSize>> vz = set.get_all_child();
        for (size_t i = 0; i < vz.size(); i++)
        {
            nw = vz[i].count_all_piece(PieceColor::W);
            nb = vz[i].count_all_piece(PieceColor::B);

            if (nb == 0)                        { v.push_back(new TablebaseHandler_Xv0<PieceID, _BoardSize>(vz[i])); }  // Xv0
            else if (nw == 0)                   { v.push_back(new TablebaseHandler_0vX<PieceID, _BoardSize>(vz[i])); }  // 0vX
            else if ((nb == 1) && (nw == 1))    { v.push_back(new TablebaseHandler_1v1<PieceID, _BoardSize>(vz[i])); }  // 1v1
            else if (nb + nw == 3)
            {
                if (nw >= nb)                   
                { 
                    TablebaseHandler_2v1<PieceID, _BoardSize>* tb3 = new TablebaseHandler_2v1<PieceID, _BoardSize>(vz[i]);  // 2v1 
                    v.push_back(tb3);
                    v.push_back(new TBH_Symmetry<PieceID, _BoardSize, 3>(tb3, TB_TYPE::tb_2v1, vz[i]));  // sym 2v1 = 1v2 (reverse ps...)
                } 
            }
            else
            {
                //...
            }
        }
        return v;
    }

};

#endif
