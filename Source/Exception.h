/*================================================================
Filename: Exception.h
Date: 2018.3.2
Created by AirGuanZ
================================================================*/
#pragma once

#include "Common.h"

AGZ_NAMESPACE_BEGIN(AGZ)

class Exception
{
public:
    Exception(const std::string &msg)
        : msg_(msg)
    {

    }

    const std::string &What(void) const
    {
        return msg_;
    }

private:
    std::string msg_;
};

AGZ_NAMESPACE_END(AGZ)
