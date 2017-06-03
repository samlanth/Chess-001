#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// MoveUnit
// Move<T>
//
// Struct holding chess move definition/data
//
//
#ifndef _AL_CHESS_MOVE_H
#define _AL_CHESS_MOVE_H

#include <string>

namespace chess
{
    struct MoveUnit
    {
        enum struct FLAG { none = 0, conditional = 1 };

        MoveUnit(int8_t _x, int8_t _y, uint8_t _len, FLAG _flag = FLAG::none, std::string _flag_param = "")
            : x(_x), y(_y), len(_len), flag(_flag), flag_spec(_flag_param), context_extra(""){}

        int8_t	    x;				// x direction
        int8_t	    y;				// y direction
        uint8_t	    len;			// max horizon 1..len
        FLAG        flag;			// move category (normal or special) 
        std::string flag_spec;		// special moveunit specification
        std::string context_extra;	// extra info for move from a contextual position
    };

    template <typename PieceID>
    struct Move
    {
        uint8_t src_x;
        uint8_t src_y;
        uint8_t dst_x;
        uint8_t dst_y;
        PieceID prev_src_id;
        PieceID prev_dst_id;
        MoveUnit mu;
    };

};
#endif
