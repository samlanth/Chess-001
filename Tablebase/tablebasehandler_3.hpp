#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBHandler_3   : handle 3 pieces TB
//
//
#ifndef _AL_CHESS_TABLEBASE_TBHandler_3_HPP
#define _AL_CHESS_TABLEBASE_TBHandler_3_HPP

namespace chess
{
    // TBHandler_3
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBHandler_3 : public TBH<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TBHandler_3(const PieceSet<PieceID, _BoardSize>& ps, TB_TYPE t, TBH_OPTION option) : TBH(t, ps, 3, option)
        {
            Tablebase<PieceID, _BoardSize, 3>* tb_w = TablebaseManager<PieceID, _BoardSize>::instance()->find_3(ps.name(PieceColor::W));
            Tablebase<PieceID, _BoardSize, 3>* tb_b = TablebaseManager<PieceID, _BoardSize>::instance()->find_3(ps.name(PieceColor::B));
            std::vector<PieceID> v = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);

            // TB bases
            if (tb_w == nullptr)
            {
                _tb_W = new Tablebase_2v1<PieceID, _BoardSize>(v, PieceColor::W);
                TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::W), _tb_W);
            } 
            else { _tb_W = tb_w; }


            if (tb_b == nullptr)
            {
                _tb_B = new Tablebase_2v1<PieceID, _BoardSize>(v, PieceColor::B);
                TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::B), _tb_B);
            } 
            else { _tb_B = tb_b; }

            // Child handlers
            _tb_children_info = TablebaseManager<PieceID, _BoardSize>::instance()->make_all_child_TBH(ps, option);
            for (size_t i = 0; i < _tb_children_info.size(); i++)
            {
                if (_tb_children_info[i]._tbh != nullptr)
                {
                    // Building handlers
                    // TB was not found then a TBH was immediatley created by manager, so we can build it
                    _tbh_children.push_back(_tb_children_info[i]._tbh);
                    _tb_children_info[i]._owner = true;
                    _tb_children_info[i]._child_index = _tbh_children.size() - 1;
                }
                else
                {
                    // Visitor handlers
                    // TB was found and a TBH was not created by manager, we create a handler so we can traverse hiearchy as needed
                    if (_tb_children_info[i]._t == TB_TYPE::tb_2v1) // promo
                    {
                        _tbh_children.push_back(new TBHandler_2v1<PieceID, _BoardSize>(_tb_children_info[i]._ps, option));
                        _tb_children_info[i]._owner = true;
                        _tb_children_info[i]._child_index = _tbh_children.size() - 1;
                    }
                    if (_tb_children_info[i]._t == TB_TYPE::tb_1v1)
                    {
                        _tbh_children.push_back(new TBHandler_1v1<PieceID, _BoardSize>(_tb_children_info[i]._ps, option));
                        _tb_children_info[i]._owner = true;
                        _tb_children_info[i]._child_index = _tbh_children.size() - 1;
                    }
                    else if (_tb_children_info[i]._t == TB_TYPE::tb_Xv0)
                    {
                        _tbh_children.push_back(new TBHandler_Xv0<PieceID, _BoardSize>(_tb_children_info[i]._ps, option));
                        _tb_children_info[i]._owner = true;
                        _tb_children_info[i]._child_index = _tbh_children.size() - 1;
                    }
                    else if (_tb_children_info[i]._t == TB_TYPE::tb_0vX)
                    {
                        _tbh_children.push_back(new TBHandler_0vX<PieceID, _BoardSize>(_tb_children_info[i]._ps, option));
                        _tb_children_info[i]._owner = true;
                        _tb_children_info[i]._child_index = _tbh_children.size() - 1;
                    }
                }
            }
        }     

        virtual ~TBHandler_3() 
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

        bool load() override;
        bool save() const override;
        bool build(char verbose = 0) override;
        bool is_build() const override { return _tb_W->is_build() && _tb_B->is_build(); }

        Tablebase<PieceID, _BoardSize, 3>* tb_W() const { return _tb_W; }
        Tablebase<PieceID, _BoardSize, 3>* tb_B() const { return _tb_B; }

    protected:
        // Bases TB
        Tablebase<PieceID, _BoardSize, 3>* _tb_W;   // not owner
        Tablebase<PieceID, _BoardSize, 3>* _tb_B;   // not owner

        // Childs TBH
        std::vector<STRUCT_TBH<PieceID, _BoardSize>>    _tb_children_info;
        std::vector<TBH<PieceID, _BoardSize>*>          _tbh_children;

        void     print() const;
        bool     build_base(Tablebase<PieceID, _BoardSize, 3>* tb, Tablebase<PieceID, _BoardSize, 3>* tb_oppo, char verbose = 0);

        uint64_t set_mate_score(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb);
        uint64_t set_marker(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb, Tablebase<PieceID, _BoardSize, 3>* tb_oppo);
        uint64_t process_marker(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb, Tablebase<PieceID, _BoardSize, 3>* tb_oppo);
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_3<PieceID, _BoardSize>::save()  const
    {
        if (!_tb_W->save()) return false;
        if (!_tb_B->save()) return false;

        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            _tbh_children[i]->save();
        }
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_3<PieceID, _BoardSize>::load()
    {
        if (is_build()) return true;

        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            _tbh_children[i]->load();
        }

        if (!_tb_W->load()) return false;
        if (!_tb_B->load()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TBHandler_3<PieceID, _BoardSize>::print() const
    {
        _tb_W->print();
        _tb_B->print();
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_3<PieceID, _BoardSize>::build(char verbose)
    {
        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            _tbh_children[i]->build(verbose);
        }
        return build_base(_tb_W, _tb_B, verbose);
    }

    // set_mate_score
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBHandler_3<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb)
    {
        return set_mate_score_v<PieceID, _BoardSize, 3>(color_to_play, tb);
    }

    // setup_marker_v
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBHandler_3<PieceID, _BoardSize>::set_marker(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb, Tablebase<PieceID, _BoardSize,3>* tb_oppo)
    {
        return setup_marker_v<PieceID, _BoardSize, 3>(this, color_to_play, tb, tb_oppo);
    }

    // process_marker
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBHandler_3<PieceID, _BoardSize>::process_marker(PieceColor color_to_play, Tablebase<PieceID, _BoardSize,3>* tb, Tablebase<PieceID, _BoardSize, 3>* tb_oppo)
    {
        return process_marker_v<PieceID, _BoardSize, 3>(this, color_to_play, tb, tb_oppo);
    }

    // build_base
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_3<PieceID, _BoardSize>::build_base(Tablebase<PieceID, _BoardSize, 3>* tb, Tablebase<PieceID, _BoardSize, 3>* tb_oppo, char verbose)
    {
        return build_base_v<PieceID, _BoardSize, 3>(this, &_tb_W, &_tb_B, verbose);
    }

};
#endif

