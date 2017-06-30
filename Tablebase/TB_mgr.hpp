#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TB_Manager : Track TB
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASEMANAGER_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASEMANAGER_HPP

namespace chess
{
    // TB_Manager
    template <typename PieceID, typename uint8_t _BoardSize>
    class TB_Manager
    {
        using _Piece = Piece<PieceID, _BoardSize>;

    private:
        TB_Manager() {}
        TB_Manager(const TB_Manager&)               = delete;
        TB_Manager & operator=(const TB_Manager &)  = delete;
        TB_Manager(TB_Manager&&)                    = delete;

    public:
        ~TB_Manager()
        {
            if (_instance.operator bool())
            {
                _instance.release();
            }
        }
        static const TB_Manager* instance();
        void clear() const;

        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 1>* tb) const;
        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 2>* tb) const;
        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 3>* tb) const;
        bool add(const std::string& name, Tablebase<PieceID, _BoardSize, 4>* tb) const;
 
        TablebaseBase<PieceID, _BoardSize>*  find(uint8_t n, const std::string& name) const;
        Tablebase<PieceID, _BoardSize, 1>* find_1(const std::string& name) const;
        Tablebase<PieceID, _BoardSize, 2>* find_2(const std::string& name) const;
        Tablebase<PieceID, _BoardSize, 3>* find_3(const std::string& name) const;
        Tablebase<PieceID, _BoardSize, 4>* find_4(const std::string& name) const;

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
        mutable std::map<std::string, Tablebase<PieceID, _BoardSize, 4>*>  _tbs4;

        static std::unique_ptr<TB_Manager> _instance;
    };

    //_instance
    template <typename PieceID, typename uint8_t _BoardSize>
    std::unique_ptr<TB_Manager<PieceID, _BoardSize>>
    TB_Manager<PieceID, _BoardSize>::_instance = nullptr;

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TB_Manager<PieceID, _BoardSize>::clear() const
    {
        _tbs1.clear();
        _tbs2.clear();
        _tbs3.clear();
        _tbs4.clear();
    }

    // add 1
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TB_Manager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 1>* tb) const
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

    // add 2
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TB_Manager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 2>* tb) const
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

    // add 3
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TB_Manager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 3>* tb) const
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

    // add 4
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TB_Manager<PieceID, _BoardSize>::add(const std::string& name, Tablebase<PieceID, _BoardSize, 4>* tb) const
    {
        if (_instance == nullptr) return false;
        {
            auto iter = _tbs4.find(name);
            if (iter == _tbs4.end())
            {
                _tbs4[name] = tb;
                return true;
            }
            return true;
        }
        return false;
    }

    // find
    template <typename PieceID, typename uint8_t _BoardSize>
    inline TablebaseBase<PieceID, _BoardSize>*  TB_Manager<PieceID, _BoardSize>::find(uint8_t n, const std::string& name) const
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
            else if (n == 4)
            {
                auto iter = _tbs4.find(name);
                if (iter != _tbs4.end())
                {
                    return (TablebaseBase<PieceID, _BoardSize>*)_tbs4[name];
                }
            }
        }
        return nullptr;
    }

    // find_1
    template <typename PieceID, typename uint8_t _BoardSize>
    inline Tablebase<PieceID, _BoardSize, 1>*  TB_Manager<PieceID, _BoardSize>::find_1(const std::string& name) const
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

    // find_2
    template <typename PieceID, typename uint8_t _BoardSize>
    inline Tablebase<PieceID, _BoardSize, 2>*  TB_Manager<PieceID, _BoardSize>::find_2(const std::string& name) const
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

    // find_3
    template <typename PieceID, typename uint8_t _BoardSize>
    inline Tablebase<PieceID, _BoardSize, 3>*  TB_Manager<PieceID, _BoardSize>::find_3(const std::string& name) const
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

    // find_4
    template <typename PieceID, typename uint8_t _BoardSize>
    inline Tablebase<PieceID, _BoardSize, 4>*  TB_Manager<PieceID, _BoardSize>::find_4(const std::string& name) const
    {
        if (_instance == nullptr) return nullptr;
        {
            auto iter = _tbs4.find(name);
            if (iter != _tbs4.end())
            {
                return _tbs4[name];
            }
        }
        return nullptr;
    }

    // instance()
    template <typename PieceID, typename uint8_t _BoardSize>
    const TB_Manager<PieceID, _BoardSize>* 
    TB_Manager<PieceID, _BoardSize>::instance()
    {
        if (_instance == nullptr)
        {
            _instance = std::unique_ptr<TB_Manager>(new TB_Manager);
            return _instance.get();
        }
        return  _instance.get();
    }

    // make_all_child_TBH() - Create all TB children handler of a TB
    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::vector<STRUCT_TBH<PieceID, _BoardSize>> TB_Manager<PieceID, _BoardSize>::make_all_child_TBH(const PieceSet<PieceID, _BoardSize>& set, TBH_OPTION option) const
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

            if (nb == 0)                        
            { 
                struct_tbh._t = TB_TYPE::tb_Xv0;
                struct_tbh._tbh = (TBH<PieceID, _BoardSize>*)new TBH_1<PieceID, _BoardSize>(vz[i], TB_TYPE::tb_Xv0, option);
                v.push_back(struct_tbh);
            } 
            else if (nw == 0)                   
            { 
                struct_tbh._t = TB_TYPE::tb_0vX;
                struct_tbh._tbh = (TBH<PieceID, _BoardSize>*)new TBH_1<PieceID, _BoardSize>(vz[i], TB_TYPE::tb_0vX, option);
                v.push_back(struct_tbh);
            }
            else if ((nb == 1) && (nw == 1))   
            { 
                struct_tbh._t = TB_TYPE::tb_1v1;
                struct_tbh._tbh = (TBH<PieceID, _BoardSize>*)new TBH_1v1<PieceID, _BoardSize>(vz[i], option);
                v.push_back(struct_tbh);
            } 
            else if ((nw == 2) && (nb == 1))
            {
                struct_tbh._t = TB_TYPE::tb_2v1;
                struct_tbh._tbh = (TBH<PieceID, _BoardSize>*)new TBH_2v1<PieceID, _BoardSize>(vz[i], option);                 
                v.push_back(struct_tbh);
            }
            else if ((nw == 1) && (nb == 2)) // child of the 2v2 
            {
                struct_tbh._t = TB_TYPE::tb_2v1_sym;
                PieceSet<PieceID, _BoardSize> r({ vz[i].bset(), vz[i].wset() });
                TBH<PieceID, _BoardSize>* tb3 = (TBH<PieceID, _BoardSize>*)new TBH_2v1<PieceID, _BoardSize>(r, option);
                struct_tbh._tbh = (TBH<PieceID, _BoardSize>*)new TBH_Symmetry<PieceID, _BoardSize, 3>(tb3, TB_TYPE::tb_2v1_sym, vz[i], option);
                v.push_back(struct_tbh);
            }
        }
        return v;
    }

};

#endif
