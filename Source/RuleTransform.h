/*================================================================
Filename: RuleTransform.h
Date: 2018.3.6
Created by AirGuanZ
================================================================*/
#pragma once

/*
    ��ݹ��滻�ı任����任��
        ���任��A    := A b | c
             => A    := c rest | c
                rest := b | b rest
        ��任��T[ ]{ A(c) }            = A(c)
                T[ ]{ A(c, <rest>) }    = T[c]{ <rest> }
                T[L]{ rest(b, <rest>) } = T[A(L, b)]{ <rest> }
                T[L]{ rest(b) }         = A(L, b)
    ��������ȡ�ı任����任��
        ���任��A    := a B | a C
             => A    := a rest
                rest := B | C
        ��任��T[ ]{ A(a, <rest>) } = T[a]{ <rest> }
                T[L]{ rest(B) }      = A(L, B)
                T[L]{ rest(C) }      = A(L, C)
*/

#include "Common.h"

AGZ_NAMESPACE_BEGIN(AGZ)



AGZ_NAMESPACE_END(AGZ)
