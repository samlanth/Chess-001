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
        void clear() const;

        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 1>* tb) const;
        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 2>* tb) const;
        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 3>* tb) const;
 
        TablebaseBase<PieceID, _BoardSize>*  find(uint8_t n, const std::string& name) const;
        Tablebase<PieceID, _BoardSize, 1>* find_1(const std::string& name) const;
        Tablebase<PieceID, _BoardSize, 2>* find_2(const std::string& name) const;
        Tablebase<PieceID, _BoardSize, 3>* find_3(const std::string& name) const;

        std::vector<TB_TYPE> get_all_child_TB_TYPE(const PieceSet<PieceID, _BoardSize>& set) const;
        std::vector<STRUCT_TBH<PieceID, _BoardSize>> make_all_child_TBH(const PieceSet<PieceID, _BoardSize>& set, TBH_OPTION option) const;

    public:
        static std::string name_pieces(const std::vector<PieceID>& v, PieceColor color_to_play)
        {
            // assume sorted
            std::string str_pieces;
            if (color_to_play == PieceColor::W) str_pieces = "W_"; else str_pieces = "B_";
            for (auto& vv : v) str_pieces += Piece<PieceID, _BoardSize>::to_str2(vv);
            return str_pieces;
        }

    private:
        // Would be owner of TB... unique_ptr/weak_ptr
        mutable std::map<std::string, Tablebase<PieceID, _BoardSize, 1>*>  _tbs1;
        mutable std::map<std::string, Tablebase<PieceID, _BoardSize, 2>*>  _tbs2;
        mutable std::map<std::string, Tablebase<PieceID, _BoardSize, 3>*>  _tbs3;

        static std::unique_ptr<TablebaseManager> _instance;
    };

    //_instance
    template <typename PieceID, typename uint8_t _BoardSize>
    std::unique_ptr<TablebaseManager<PieceID, _BoardSize>>
    TablebaseManager<PieceID, _BoardSize>::_instance = nullptr;

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TablebaseManager<PieceID, _BoardSize>::clear() const
    {
        _tbs1.clear();
        _tbs2.clear();
        _tbs3.clear();
    }

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

    // find
    template <typename PieceID, typename uint8_t _BoardSize>
    inline TablebaseBase<PieceID, _BoardSize>*  TablebaseManager<PieceID, _BoardSize>::find(uint8_t n, const std::string& name) const
    {
        if (_instance == nullptr) return nullptr;
        {
            if (n == 1)
            {
                auto iter = _tbs1.find(name);
                if (iter != _tbs1.end())
                {
                    return (TablebaseBase<PieceID, _BoardSize>*)_tbs1[name];
                }
            }
            else if (n == 2)
            {
                auto iter = _tbs2.find(name);
                if (iter != _tbs2.end())
                {
                    return (TablebaseBase<PieceID, _BoardSize>*)_tbs2[name];
                }
            }
            else if (n == 3)
            {
                auto iter = _tbs3.find(name);
                if (iter != _tbs3.end())
                {
                    return (TablebaseBase<PieceID, _BoardSize>*)_tbs3[name];
                }
            }
        }
        return nullptr;
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
    inline std::vector<STRUCT_TBH<PieceID, _BoardSize>> TablebaseManager<PieceID, _BoardSize>::make_all_child_TBH(const PieceSet<PieceID, _BoardSize>& set, TBH_OPTION option) const
    {
        std::vector<STRUCT_TBH<PieceID, _BoardSize>> v;
        uint16_t nw; 
        uint16_t nb;

        std::vector<PieceSet<PieceID, _BoardSize>> vz = set.get_all_child();
        for (size_t i = 0; i < vz.size(); i++)
        {
            nw = vz[i].count_all_piece(PieceColor::W);
            nb = vz[i].count_all_piece(PieceColor::B);

            STRUCT_TBH<PieceID, _BoardSize> struct_tbh(vz[i]);
            struct_tbh._t = TB_TYPE::tb_unknown;
            struct_tbh._nw = nw;
            struct_tbh._nb = nb;
            struct_tbh._tbh = nullptr;

            // If a TB already exist, an handler is/was handling (building, loading, etc) it (do a clear() first) or check build status
            // Launch a handler as necessary, the handler will create and add the TB to the repository immediately (is_build is false initially)
            if (nb == 0)                        
            { 
                // Xv0
                struct_tbh._t = TB_TYPE::tb_Xv0;
                Tablebase<PieceID, _BoardSize, 1>* tb = find_1(vz[i].name(PieceColor::W));
                if (tb == nullptr) struct_tbh._tbh = (TBH<PieceID, _BoardSize>*)new TBHandler_Xv0<PieceID, _BoardSize>(vz[i], option);
                v.push_back(struct_tbh);
            } 
            else if (nw == 0)                   
            { 
                // 0vX
                struct_tbh._t = TB_TYPE::tb_0vX;
                Tablebase<PieceID, _BoardSize, 1>* tb = find_1(vz[i].name(PieceColor::W));
                if (tb == nullptr) struct_tbh._tbh = (TBH<PieceID, _BoardSize>*)new TBHandler_0vX<PieceID, _BoardSize>(vz[i], option);
                v.push_back(struct_tbh);
            }
            else if ((nb == 1) && (nw == 1))   
            { 
                // 1v1
                struct_tbh._t = TB_TYPE::tb_1v1;
                Tablebase<PieceID, _BoardSize, 2>* tb = find_2(vz[i].name(PieceColor::W));
                if (tb == nullptr) struct_tbh._tbh = (TBH<PieceID, _BoardSize>*)new TBHandler_1v1<PieceID, _BoardSize>(vz[i], option);
                v.push_back(struct_tbh);
            } 
            if ((nw == 2) && (nb == 1))
            {
                // 2v1
                struct_tbh._t = TB_TYPE::tb_2v1;
                Tablebase<PieceID, _BoardSize, 3>* tb = find_3(vz[i].name(PieceColor::W));
                if (tb == nullptr)
                {
                    struct_tbh._tbh = (TBH<PieceID, _BoardSize>*)new TBHandler_2v1<PieceID, _BoardSize>(vz[i], option);                 
                }
                v.push_back(struct_tbh);
            }
            else if ((nw == 1) && (nb == 2)) // child of a 4 pieces 2v2 
            {
                //v.push_back(new TBH_Symmetry<PieceID, _BoardSize, 3>(tb3, TB_TYPE::tb_2v1, vz[i]));  // reverse pieceset vz[i]
            }

        }
        return v;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::vector<TB_TYPE> TablebaseManager<PieceID, _BoardSize>::get_all_child_TB_TYPE(const PieceSet<PieceID, _BoardSize>& set) const
    {
        std::vector<TB_TYPE> v;
        uint16_t nw;
        uint16_t nb;

        std::vector<PieceSet<PieceID, _BoardSize>> vz = set.get_all_child();
        for (size_t i = 0; i < vz.size(); i++)
        {
            nw = vz[i].count_all_piece(PieceColor::W);
            nb = vz[i].count_all_piece(PieceColor::B);

            if (nb == 0)                        v.push_back(TB_TYPE::tb_Xv0);
            else if (nw == 0)                   v.push_back(TB_TYPE::tb_0vX);
            else if ((nb == 1) && (nw == 1))    v.push_back(TB_TYPE::tb_1v1);
            else if (nb + nw == 3)              v.push_back(TB_TYPE::tb_2v1); // and  v.push_back(TB_TYPE::tb_2v1_sym); ...
        }
        return v;
    }

};

#endif
