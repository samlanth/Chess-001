#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBH<> Handler of 2 TB(TableBase) (White or Black to play a PieceSet) and childrens build/load/save hiearchy 
// EX: KPvK=>KQvK(=>KvK, PvK, KPv0), KvK, KPv0, PvK
//
//
#ifndef _AL_CHESS_TABLEBASE_TBH_HPP
#define _AL_CHESS_TABLEBASE_TBH_HPP

namespace chess
{
    // TBH_Base
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBH_Base
    {
    public:
        TBH_Base() {}
        virtual ~TBH_Base() {}

        virtual bool is_symmetry_TBH() const = 0;
        virtual bool load(TBH_IO_MODE mode) = 0;
        virtual bool save() const = 0;
        virtual bool build(TBH_IO_MODE mode, char verbose) = 0;
        virtual bool is_build_and_loaded() const = 0;
    };

    // TBH
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBH : public TBH_Base<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;

        template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
        bool friend build_base_vv(TBH<PieceID, _BoardSize>* tbh, TablebaseBase<PieceID, _BoardSize>* tb_W, TablebaseBase<PieceID, _BoardSize>* tb_B, char verbose);

        friend class TBH_Symmetry<PieceID, _BoardSize, 1>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 2>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 3>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 4>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 5>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 6>;
        friend class TBH_Symmetry<PieceID, _BoardSize, 7>;

    public:
        TBH(TB_TYPE t, const PieceSet<PieceID, _BoardSize>& ps, uint8_t np, TBH_IO_MODE mode, TBH_OPTION option)
            : TBH_Base(), _type(t), _NPIECE(np), _pieceSet({ ps.wset(), ps.bset() }), _option(option)
        {
            _piecesID = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);
            _pieceSet.make_all_child_set(); //...
        }
        virtual ~TBH();

        virtual bool is_symmetry_TBH() const override  { return false; }
        virtual bool load(TBH_IO_MODE mode) override;
        virtual bool save() const override;
        virtual bool is_build_and_loaded() const  override
        {   
            if ((_TB_W == nullptr) || (_TB_B == nullptr)) return false;
            return _TB_W->is_build_and_loaded() && _TB_B->is_build_and_loaded(); 
        }
        virtual void print() const;
        virtual bool find_score_children_tb(const _Board& pos, PieceColor color_pos, bool isPromo, bool isCapture, ExactScore& ret_sc, uint8_t& ret_dtc) const;

        std::string name(PieceColor color_toplay)   const;  // unique name of tB W or B to play
        TBH_OPTION  option()                        const { return _option; }
        uint8_t     get_NPIECE()                    const { return _NPIECE; }
        TB_TYPE     tb_type()                       const { return _type; }
        const PieceSet<PieceID, _BoardSize>& pieceSet()   const { return _pieceSet;}

        TablebaseBase<PieceID, _BoardSize>* TB_W() const { return  _TB_W; }
        TablebaseBase<PieceID, _BoardSize>* TB_B() const { return  _TB_B; }
        const TablebaseBase<PieceID, _BoardSize>* TB_W_const() const { return  _TB_W; }
        const TablebaseBase<PieceID, _BoardSize>* TB_B_const() const { return  _TB_B; }

    protected:
        TB_TYPE                         _type;
        uint8_t                         _NPIECE;
        PieceSet<PieceID, _BoardSize>   _pieceSet;
        TBH_OPTION                      _option;        // none, try_load_on_build, force_rebuild, memory_map_on_build
        std::vector<PieceID>            _piecesID;
        mutable std::recursive_mutex    _mutex;

        // TB w+b
        TablebaseBase<PieceID, _BoardSize>* _TB_W = nullptr;    // Not owner
        TablebaseBase<PieceID, _BoardSize>* _TB_B = nullptr;    // Not owner

        // Children TBH
        std::vector<STRUCT_TBH<PieceID, _BoardSize>>    _tb_children_info;
        std::vector<TBH<PieceID, _BoardSize>*>          _tbh_children;
      
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
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if ((_TB_W == nullptr) || (_TB_B == nullptr)) return false;
        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            if (!_tbh_children[i]->save())
                return false;
        }
        if (!_TB_W->save()) return false;
        if (!_TB_B->save()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBH<PieceID, _BoardSize>::load(TBH_IO_MODE mode)
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if ((_TB_W == nullptr) || (_TB_B == nullptr)) return false;
        if (is_build_and_loaded()) return true;

        TBH_IO_MODE childmode = mode;
        if (mode == TBH_IO_MODE::tb_and_child) childmode = TBH_IO_MODE::tb_only;
        if (mode != TBH_IO_MODE::tb_only)
        {
            for (size_t i = 0; i < _tbh_children.size(); i++)
            {
                if (!_tbh_children[i]->load(childmode))
                    return false;
            }
        }
        if (!_TB_W->load()) return false;
        if (!_TB_B->load()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TBH<PieceID, _BoardSize>::print() const
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

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
        std::lock_guard<std::recursive_mutex> lock(_mutex);

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
    inline bool TBH<PieceID, _BoardSize>::find_score_children_tb(const _Board& pos, PieceColor color_pos, bool isPromo, bool isCapture, ExactScore& ret_sc, uint8_t& ret_dtc) const
    {
        size_t      ret_child_index;
        PieceID     ret_id;
        uint16_t    ret_count;
        uint16_t    rank_ret_id;
        uint16_t    ret_instance;
        std::vector<uint16_t> ret_child_sq;
        std::vector<PieceID> ret_child_id;

        bool has_wk = pos.has_piece(PieceName::K, PieceColor::W);
        bool has_bk = pos.has_piece(PieceName::K, PieceColor::B);

        if (!has_wk && has_bk)
        {
            // Black win
            ret_sc = ExactScore::LOSS;
            ret_dtc = 0;
            return true;
        }
        if (has_wk && !has_bk)
        {
            // White win
            ret_sc = ExactScore::WIN;
            ret_dtc = 0;
            return true;
        }
        if (!has_wk && !has_bk)
        {
            assert(false);
        }


        // LOCK ? - Only reading children TB (all_child_set must have been created)
        //std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (_pieceSet.find_all_child_index(color_pos, pos, isPromo, isCapture, ret_child_index))
        {
            const PieceSet<PieceID, _BoardSize>& child_set = _pieceSet.all_child_set_at(ret_child_index);

            // get square of uniform piece ordering
            ret_child_sq.clear();
            ret_child_id.clear();
            for (size_t i = 0; i < get_NPIECE(); i++) // N piece at most: child is capture (N-1) piece or promo (N) piece or both (N-1) pieces
            {
                rank_ret_id = child_set.find_rank_index(i, ret_id, ret_count, ret_instance);
                if (ret_count > 0)
                {
                    ret_child_sq.push_back(pos.get_square_ofpiece_instance(_Piece::get(ret_id)->get_name(), _Piece::get(ret_id)->get_color(), ret_instance));
                    ret_child_id.push_back(ret_id);
                }
            }

            size_t ret_idx;
            if (find_tbh_children(child_set, color_pos, ret_idx))
            {
                if (_tbh_children[ret_idx]->is_symmetry_TBH())
                {
                    // a capture made a child with nw < nb - lookup sym tB
                    const std::vector<PieceID> ret_child_id_const = ret_child_id;
                    std::string tb_name = TB_Manager<PieceID, _BoardSize>::instance()->name_pieces(ret_child_id_const, color_pos);
                    TablebaseBase<PieceID, _BoardSize>*  t = TB_Manager<PieceID, _BoardSize>::instance()->find_sym(tb_name);
                    if (t != nullptr)
                    {
                        if (!t->is_build_and_loaded())
                        {
                            if (t->load() == false)
                            {
                                std::stringstream ss_detail;
                                ss_detail << "find_score_children_tb TB sym not builded or not loaded "<< tb_name <<"\n";
                                std::cerr << ss_detail.str();
                            }
                        }
                        assert(t->is_build_and_loaded());
                        ret_sc = t->score_v(ret_child_sq);
                        ret_dtc = t->dtc_v(ret_child_sq);
                    }
                    else
                    {
                        assert(false);
                        return false;
                    }
                }
                else
                {
                    if (color_pos == PieceColor::W)
                    {
                        if (!_tbh_children[ret_idx]->TB_W()->is_build_and_loaded())
                        {
                            if (_tbh_children[ret_idx]->TB_W()->load() == false) 
                            {
                                std::stringstream ss_detail;
                                ss_detail << "find_score_children_tb TB not builded or not loaded " << child_set.name(PieceColor::W) << "\n";
                                std::cerr << ss_detail.str();
                            }
                        }
                        assert(_tbh_children[ret_idx]->TB_W()->is_build_and_loaded());
                        ret_sc = _tbh_children[ret_idx]->TB_W()->score_v(ret_child_sq);
                        ret_dtc = _tbh_children[ret_idx]->TB_W()->dtc_v(ret_child_sq);
                    }
                    else
                    {
                        if (!_tbh_children[ret_idx]->TB_B()->is_build_and_loaded())
                        {
                            if (_tbh_children[ret_idx]->TB_B()->load() == false)
                            {
                                std::stringstream ss_detail;
                                ss_detail << "find_score_children_tb TB not builded or not loaded " <<  child_set.name(PieceColor::B)  << "\n";
                                std::cerr << ss_detail.str();
                            }
                        }
                        assert(_tbh_children[ret_idx]->TB_B()->is_build_and_loaded());
                        ret_sc = _tbh_children[ret_idx]->TB_B()->score_v(ret_child_sq);
                        ret_dtc = _tbh_children[ret_idx]->TB_B()->dtc_v(ret_child_sq);
                    }
                }
                return true;
            }
        }
        return false;
    }

};
#endif

