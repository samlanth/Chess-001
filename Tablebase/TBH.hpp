#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBH<> Handler of a 2 TB(TableBase) (White or Black to play a PieceSet) and childrens build/load/save hiaerchy 
// EX: KPvK=>KQvK(=>KvK, PvK, KPv0), KvK, KPv0, PvK
//
//
#ifndef _AL_CHESS_TABLEBASE_TBH_HPP
#define _AL_CHESS_TABLEBASE_TBH_HPP

namespace chess
{
    // TBH
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBH
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;

        template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
        bool friend build_base_vv(TBH<PieceID, _BoardSize>* tbh, TablebaseBase<PieceID, _BoardSize>* tb_W, TablebaseBase<PieceID, _BoardSize>* tb_B, char verbose);

    public:
        TBH(TB_TYPE t, const PieceSet<PieceID, _BoardSize>& ps, uint8_t np, TBH_OPTION option) : _type(t), _NPIECE(np), _pieceSet({ ps.wset(), ps.bset() }), _option(option)
        {
            _piecesID = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);
        }
        virtual ~TBH();
        virtual bool is_symmetry_TBH() const { return false; }

        virtual bool load();
        virtual bool save() const;
        virtual bool build(char verbose) = 0;
        virtual bool is_build() const 
        {   
            if ((_TB_W == nullptr) || (_TB_B == nullptr)) return false;
            return _TB_W->is_build() && _TB_B->is_build(); 
        }
        virtual void print() const;
        virtual bool find_score_children_tb(const _Board& pos, PieceColor color, bool isPromo, ExactScore& ret_sc) const;

        std::string name(PieceColor color_toplay)   const;  // unique name of tB W or B to play
        TBH_OPTION  option()                        const { return _option; }
        uint8_t     get_NPIECE()                    const { return _NPIECE; }
        TB_TYPE     tb_type()                       const { return _type; }
        const PieceSet<PieceID, _BoardSize>& pieceSet()   const { return _pieceSet;}

    protected:
        TB_TYPE                         _type;
        uint8_t                         _NPIECE;
        PieceSet<PieceID, _BoardSize>   _pieceSet;
        TBH_OPTION                      _option;        // none, try_load_on_build, force_rebuild, memory_map_on_build
        std::vector<PieceID>            _piecesID;

        // TB w+b
        TablebaseBase<PieceID, _BoardSize>* _TB_W = nullptr;    // Not owner
        TablebaseBase<PieceID, _BoardSize>* _TB_B = nullptr;    // Not owner

        // Children TBH
        std::vector<STRUCT_TBH<PieceID, _BoardSize>>    _tb_children_info;
        std::vector<TBH<PieceID, _BoardSize>*>          _tbh_children;

        TablebaseBase<PieceID, _BoardSize>* TB_W() const { return  _TB_W; }
        TablebaseBase<PieceID, _BoardSize>* TB_B() const { return  _TB_B; }
        void set_TB_W(TablebaseBase<PieceID, _BoardSize>* t)  { _TB_W = t; }
        void set_TB_B(TablebaseBase<PieceID, _BoardSize>* t)  { _TB_B = t; }

        bool find_tbh_children(PieceSet<PieceID, _BoardSize>  ps, PieceColor color_to_play, size_t& ret_index) const;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    TBH<PieceID, _BoardSize>::~TBH()
    {
        for (size_t i = 0; i < _tb_children_info.size(); i++)
        {
            if ((_tb_children_info[i]._owner == true) && (_tb_children_info[i]._tbh != nullptr))
            {
                if (_tb_children_info[i]._child_index < _tbh_children.size())
                {
                    delete _tbh_children[_tb_children_info[i]._child_index];
                }
            }
        }
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBH<PieceID, _BoardSize>::save()  const
    {
        if ((_TB_W == nullptr) || (_TB_B == nullptr)) return false;
        if (!_TB_W->save()) return false;
        if (!_TB_B->save()) return false;
        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            if (!_tbh_children[i]->save())
                return false;
        }
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBH<PieceID, _BoardSize>::load()
    {
        if ((_TB_W == nullptr) || (_TB_B == nullptr)) return false;
        if (is_build()) return true;
        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            if (!_tbh_children[i]->load())
            return false;
        }
        if (!_TB_W->load()) return false;
        if (!_TB_B->load()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TBH<PieceID, _BoardSize>::print() const
    {
        if ((_TB_W == nullptr) || (_TB_B == nullptr)) return;
        _TB_W->print();
        _TB_B->print();
    }

    // STRUCT_TBH
    template <typename PieceID, typename uint8_t _BoardSize>
    struct STRUCT_TBH
    {
        STRUCT_TBH(PieceSet<PieceID, _BoardSize> p) : _ps(p.wset(), p.bset()) , _owner(false) {}
        ~STRUCT_TBH() {}

        TB_TYPE                         _t;
        PieceSet<PieceID, _BoardSize>   _ps;
        TBH<PieceID, _BoardSize>*       _tbh;
        uint16_t                        _nw;
        uint16_t                        _nb;
        bool                            _owner = false;
        size_t                          _child_index;
    };

    // name() - unique per PieceSet
    template <typename PieceID, typename uint8_t _BoardSize>
    inline std::string TBH<PieceID, _BoardSize>::name(PieceColor color_toplay) const
    {
        if (color_toplay == PieceColor::W) return _pieceSet.name(PieceColor::W);
        return _pieceSet.name(PieceColor::B);
    }

    // find_tbh_children - find chidren handler
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBH<PieceID, _BoardSize>::find_tbh_children(PieceSet<PieceID, _BoardSize>  ps, PieceColor color_to_play, size_t& ret_index) const
    {
        for (size_t i = 0; i < _tb_children_info.size(); i++)
        {
            if (_tb_children_info[i]._ps.name(color_to_play) == ps.name(color_to_play))
            {
                ret_index = i;
                return true;
            }
        }
        assert(false);
        return  false;
    }

    // find_score_children_tb
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBH<PieceID, _BoardSize>::find_score_children_tb(const _Board& pos, PieceColor color, bool isPromo, ExactScore& ret_sc) const
    {
        size_t      ret_child_index;
        PieceID     ret_id;
        uint16_t    ret_count;
        uint16_t    rank_ret_id;
        uint16_t    ret_instance;
        PieceColor  c_oppo = (color == PieceColor::W) ? PieceColor::B : PieceColor::W;
        std::vector<uint16_t> ret_child_sq;

        if (_pieceSet.find_child_index(color, pos, isPromo, ret_child_index))
        {
            PieceColor color_change = color;
            if (isPromo) color_change = c_oppo;
            std::vector<std::pair<PieceID, uint8_t>> v = _pieceSet.children(color_change, ret_child_index);
            PieceSet<PieceID, _BoardSize> child_set({ (color_change == PieceColor::W) ? v : _pieceSet.wset(), (color_change == PieceColor::W) ? _pieceSet.bset() : v });
            ret_child_sq.clear();
            for (size_t i = 0; i < get_NPIECE(); i++) // child is capture N-1 piece or promo N piece
            {
                rank_ret_id = child_set.find_rank_index(i, ret_id, ret_count, ret_instance);
                if (ret_count > 0)
                {
                    ret_child_sq.push_back(pos.get_square_ofpiece_instance(_Piece::get(ret_id)->get_name(), _Piece::get(ret_id)->get_color(), ret_instance));
                }
            }
            size_t ret_idx;
            if (find_tbh_children(child_set, color, ret_idx))
            {
                if (color == PieceColor::W)
                {
                    ret_sc = _tbh_children[ret_idx]->TB_W()->score_v(ret_child_sq);
                }
                else
                {
                    ret_sc = _tbh_children[ret_idx]->TB_B()->score_v(ret_child_sq);
                }
            }
            return true;
        }
        assert(false);
        return false;
    }

};
#endif

